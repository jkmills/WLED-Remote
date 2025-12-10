#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Arduino.h>

#define MAX_RETRIES 25
#define DEBOUNCE_DELAY 50  // Button debounce time in milliseconds
#define DEBUG_MODE true    // Set to false to enable deep sleep

// Modify with the WLED wifi channel
#define CHANNEL 1

// Button pin definitions for DOIT ESP32 DEVKIT V1
// IMPORTANT: Only RTC GPIO pins can wake from deep sleep!
// RTC GPIO pins: 0, 2, 4, 12, 13, 14, 15, 25, 26, 27, 32, 33
#define BTN_ON_OFF    4   // GPIO4  - ON/OFF toggle
#define BTN_PRESET1   15  // GPIO15 - Preset 1
#define BTN_PRESET2   13  // GPIO13 - Preset 2
#define BTN_PRESET3   12  // GPIO12 - Preset 3
#define BTN_PRESET4   14  // GPIO14 - Preset 4
#define BTN_BRIGHT_UP 27  // GPIO27 - Brightness Up
#define BTN_BRIGHT_DN 26  // GPIO26 - Brightness Down
#define BTN_NIGHT     25  // GPIO25 - Night Mode

#define LED_BUILTIN 2     // Built-in LED on GPIO2 for DEVKIT V1

// Constants used by WLED from wled00/remote.cpp
#define ON 1
#define OFF 2
#define NIGHT 3
#define PRESET_ONE 16
#define PRESET_TWO 17
#define PRESET_THREE 18
#define PRESET_FOUR 19
#define BRIGHT_UP 9
#define BRIGHT_DOWN 8

// Button configuration structure
struct ButtonConfig {
  uint8_t pin;
  uint8_t command;
  const char* name;
  bool needsState;  // true if button requires state tracking (like ON/OFF)
};

// Define all buttons and their WLED commands
ButtonConfig buttons[] = {
  {BTN_ON_OFF,    0,            "ON/OFF",       true},   // Command set dynamically based on state
  {BTN_PRESET1,   PRESET_ONE,   "Preset 1",     false},
  {BTN_PRESET2,   PRESET_TWO,   "Preset 2",     false},
  {BTN_PRESET3,   PRESET_THREE, "Preset 3",     false},
  {BTN_PRESET4,   PRESET_FOUR,  "Preset 4",     false},
  {BTN_BRIGHT_UP, BRIGHT_UP,    "Bright Up",    false},
  {BTN_BRIGHT_DN, BRIGHT_DOWN,  "Bright Down",  false},
  {BTN_NIGHT,     NIGHT,        "Night Mode",   false}
};

const int NUM_BUTTONS = sizeof(buttons) / sizeof(buttons[0]);

// State of the LED - stored in RTC memory to persist through deep sleep
RTC_DATA_ATTR bool is_lightOn = false;

// Sequence number - stored in RTC memory
RTC_DATA_ATTR uint32_t seq = 1;

// Store which button was pressed - stored in RTC memory
RTC_DATA_ATTR uint8_t lastButtonPressed = 255;

// Configure up to 4 WLED targets. Add or remove entries to match your setup.
// Example shows 3 targets: Living Room, Kitchen, and Desk. Replace the MAC
// addresses and names with your own WLED devices.
struct WLEDTarget {
  uint8_t mac[6];
  const char* name;
};

WLEDTarget targets[] = {
  {{0x30, 0xC6, 0xF7, 0x27, 0x58, 0x60}, "Stage-Left"},
  {{0x30, 0xC6, 0xF7, 0x1F, 0xD2, 0xD0}, "Stage-Right"}
};

const int NUM_TARGETS = sizeof(targets) / sizeof(targets[0]);

// Map each button to one or more WLED targets. Set an entry to true to send
// the button's command to that target. This lets a single button control
// multiple instances.
bool buttonTargets[NUM_BUTTONS][NUM_TARGETS] = {
  // ON/OFF      Living Room, Kitchen, Desk
  {true,        true,        false},
  // Preset 1
  {true,        false,       true},
  // Preset 2
  {false,       true,        true},
  // Preset 3
  {true,        true,        true},
  // Preset 4
  {true,        false,       false},
  // Bright Up
  {true,        true,        true},
  // Bright Down
  {true,        true,        true},
  // Night Mode
  {true,        true,        false}
};

int retriesCount = 0;

esp_now_peer_info_t peerInfo;
uint8_t activeTargetMac[6] = {0};

// Message structure from wled00/remote.cpp
typedef struct message_structure {
  uint8_t program = 0x81;      // 0x91 for ON button, 0x81 for all others
  uint8_t seq[4];       // Incremental sequence number 32 bit unsigned integer LSB first
  uint8_t byte5 = 32;   // Unknown
  uint8_t button;       // Identifies which button is being pressed
  uint8_t byte8 = 1;    // Unknown, but always 0x01
  uint8_t byte9 = 100;  // Unknown, but always 0x64

  uint8_t byte10;  // Unknown, maybe checksum
  uint8_t byte11;  // Unknown, maybe checksum
  uint8_t byte12;  // Unknown, maybe checksum
  uint8_t byte13;  // Unknown, maybe checksum
} message_structure;

message_structure message;

// Callback on data sent. If the message delivery fails, retry until success or MAX_RETRIES
// Updated signature for newer ESP32 Arduino core (v3.x with IDF 5.x)
void sentStatusAndRetries(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Delivery Status: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Success");
    retriesCount = 0;
    return;
  }

  Serial.println("Fail. Retrying");

  if (retriesCount < MAX_RETRIES) {
    retriesCount += 1;
    esp_now_send(activeTargetMac, (uint8_t *)&message, sizeof(message));
    return;
  }

  retriesCount = 0;
}

void sendMessage(int button, int targetIndex) {
  // Increase seq number
  seq += 1;

  // Format seq number (LSB first)
  message.seq[0] = seq;
  message.seq[1] = seq >> 8;
  message.seq[2] = seq >> 16;
  message.seq[3] = seq >> 24;

  Serial.print("SEQ: ");
  Serial.print(seq);
  Serial.print(" | Command: ");
  Serial.println(button);
  Serial.print("Target: ");
  Serial.println(targets[targetIndex].name);

  message.button = button;

  memcpy(activeTargetMac, targets[targetIndex].mac, 6);
  esp_err_t result = esp_now_send(activeTargetMac, (uint8_t *)&message, sizeof(message));

  // Display the error
  switch (result) {
    case ESP_OK:
      Serial.println("Message queued successfully");
      break;

    case ESP_ERR_ESPNOW_NOT_INIT:
      Serial.println("ESP-NOW not initialized");
      break;

    case ESP_ERR_ESPNOW_ARG:
      Serial.println("Invalid argument");
      break;

    case ESP_ERR_ESPNOW_INTERNAL:
      Serial.println("Internal error");
      break;

    case ESP_ERR_ESPNOW_NO_MEM:
      Serial.println("Out of memory");
      break;

    case ESP_ERR_ESPNOW_NOT_FOUND:
      Serial.println("Peer not found");
      break;

    case ESP_ERR_ESPNOW_IF:
      Serial.println("Interface error");
      break;

    default:
      Serial.print("Unknown error code: ");
      Serial.println(result);
      break;
  }
}

void addPeers() {
  for (int i = 0; i < NUM_TARGETS; i++) {
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, targets[i].mac, 6);
    peerInfo.channel = CHANNEL;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.print("Failed to add peer: ");
      Serial.println(targets[i].name);
    } else {
      Serial.print("Added peer: ");
      Serial.println(targets[i].name);
    }
  }
}

// Check which button was pressed
int getButtonPressed() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (digitalRead(buttons[i].pin) == LOW) {  // Button pressed (active LOW with pull-up)
      delay(DEBOUNCE_DELAY);  // Debounce
      if (digitalRead(buttons[i].pin) == LOW) {  // Confirm still pressed
        return i;  // Return button index
      }
    }
  }
  return -1;  // No button pressed
}

void setup(){
  Serial.begin(115200);

  // Longer delay for serial to initialize and connect
  delay(2000);

  Serial.println("\n\n=== ESP32 DEVKIT V1 WLED Multi-Button Remote ===");

  // Initialize WiFi first to get MAC address
  WiFi.mode(WIFI_STA);
  delay(100);  // Give WiFi time to initialize

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Initialize all button pins with internal pull-up resistors
  Serial.println("\nInitializing buttons:");
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
    Serial.print("  - GPIO");
    Serial.print(buttons[i].pin);
    Serial.print(": ");
    Serial.println(buttons[i].name);
  }

  // Initialize built-in LED as status indicator
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // LED on during initialization

  // WiFi configuration for DEVKIT V1 (already initialized above for MAC address)
  WiFi.disconnect();  // Ensure clean state

  // For newer ESP-IDF versions, channel setting is handled differently
  // The channel is set when adding the peer

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Message sending callback function
  esp_now_register_send_cb(sentStatusAndRetries);

  addPeers();

  Serial.println("ESP-NOW Initialized Successfully");
  Serial.println("\nWaiting for button press...");

  // Give user time to view Serial output (10 seconds)
  // Press any button during this time to proceed
  Serial.println("Press any button within 10 seconds to test...");
  unsigned long startTime = millis();
  int buttonIndex = -1;

  while (millis() - startTime < 10000) {
    buttonIndex = getButtonPressed();
    if (buttonIndex >= 0) break;
    delay(100);
  }

  // Check which button woke us up or is currently pressed
  if (buttonIndex < 0) {
    buttonIndex = getButtonPressed();
  }

  if (buttonIndex >= 0) {
    Serial.print("\nButton Pressed: ");
    Serial.println(buttons[buttonIndex].name);

    uint8_t commandToSend;

    // Handle ON/OFF button specially (needs state tracking)
    if (buttons[buttonIndex].needsState) {
      if (is_lightOn) {
        commandToSend = OFF;
        is_lightOn = false;
        Serial.println("Sending: Light OFF");
        digitalWrite(LED_BUILTIN, LOW);
      } else {
        commandToSend = ON;
        is_lightOn = true;
        Serial.println("Sending: Light ON");
        digitalWrite(LED_BUILTIN, HIGH);
      }
    } else {
      // All other buttons send their configured command
      commandToSend = buttons[buttonIndex].command;
      Serial.print("Sending command: ");
      Serial.println(commandToSend);

      // Blink LED to indicate button press
      for (int i = 0; i < 2; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN, LOW);
        delay(50);
      }
    }

    // Send the message to all configured targets
    bool sentToAny = false;
    for (int i = 0; i < NUM_TARGETS; i++) {
      if (buttonTargets[buttonIndex][i]) {
        sendMessage(commandToSend, i);
        sentToAny = true;
      }
    }

    if (!sentToAny) {
      Serial.println("No targets configured for this button. Update buttonTargets matrix.");
    }

    // Store which button was pressed
    lastButtonPressed = buttonIndex;

    // Wait for button release
    while (digitalRead(buttons[buttonIndex].pin) == LOW) {
      delay(10);
    }
    Serial.println("Button released");
  } else {
    Serial.println("No button detected - this shouldn't happen!");
  }

  // Delay to complete the sending
  delay(200);

  if (DEBUG_MODE) {
    Serial.println("\n=== DEBUG MODE: Deep sleep disabled ===");
    Serial.println("Press any button to test...\n");
  } else {
    Serial.println("\nEntering deep sleep...");
    Serial.println("Press any button to wake up");
    Serial.flush();  // Ensure all serial data is sent

    // Configure all buttons as wake-up sources (EXT0 only supports one pin, so we use EXT1)
    // EXT1 can wake on multiple pins with ANY_HIGH or ALL_LOW
    uint64_t buttonMask = 0;
    for (int i = 0; i < NUM_BUTTONS; i++) {
      buttonMask |= (1ULL << buttons[i].pin);
    }

    // Wake up when ANY button is pressed LOW (with pull-up resistors)
    esp_sleep_enable_ext1_wakeup(buttonMask, ESP_EXT1_WAKEUP_ALL_LOW);

    // Deep sleep to reduce power consumption
    esp_deep_sleep_start();
  }
}

void loop(){
  if (DEBUG_MODE) {
    // In debug mode, continuously check for button presses
    int buttonIndex = getButtonPressed();

    if (buttonIndex >= 0) {
      Serial.print("Button Pressed: ");
      Serial.println(buttons[buttonIndex].name);

      uint8_t commandToSend;

      // Handle ON/OFF button specially (needs state tracking)
      if (buttons[buttonIndex].needsState) {
        if (is_lightOn) {
          commandToSend = OFF;
          is_lightOn = false;
          Serial.println("Sending: Light OFF");
          digitalWrite(LED_BUILTIN, LOW);
        } else {
          commandToSend = ON;
          is_lightOn = true;
          Serial.println("Sending: Light ON");
          digitalWrite(LED_BUILTIN, HIGH);
        }
      } else {
        // All other buttons send their configured command
        commandToSend = buttons[buttonIndex].command;
        Serial.print("Sending command: ");
        Serial.println(commandToSend);

        // Blink LED to indicate button press
        for (int i = 0; i < 2; i++) {
          digitalWrite(LED_BUILTIN, HIGH);
          delay(50);
          digitalWrite(LED_BUILTIN, LOW);
          delay(50);
        }
      }

      // Send the message to all configured targets
      bool sentToAny = false;
      for (int i = 0; i < NUM_TARGETS; i++) {
        if (buttonTargets[buttonIndex][i]) {
          sendMessage(commandToSend, i);
          sentToAny = true;
        }
      }

      if (!sentToAny) {
        Serial.println("No targets configured for this button. Update buttonTargets matrix.");
      }

      // Wait for button release
      while (digitalRead(buttons[buttonIndex].pin) == LOW) {
        delay(10);
      }
      Serial.println("Button released\n");

      delay(100);  // Debounce
    }

    delay(50);  // Small delay to prevent excessive CPU usage
  }
  // If not in debug mode, this loop never runs (deep sleep)
}

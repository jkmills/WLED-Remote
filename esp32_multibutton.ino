#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Arduino.h>

#define MAX_RETRIES 25
#define DEBUG_MODE true    // Set to false to enable deep sleep

// Multi-press timing
const unsigned long MULTI_DEBOUNCE_MS   = 30;
const unsigned long MULTI_DOUBLE_MS     = 400;
const unsigned long MULTI_LONG_PRESS_MS = 800;

// Modify with the WLED wifi channel
#define CHANNEL 1

// Button pin definitions for DOIT ESP32 DEVKIT V1
// IMPORTANT: Only RTC GPIO pins can wake from deep sleep!
// RTC GPIO pins: 0, 2, 4, 12, 13, 14, 15, 25, 26, 27, 32, 33
#define BTN_ON_OFF    4   // GPIO4  - ON/OFF toggle
#define BTN_ACTION1   15  // GPIO15 - Action 1 (was Preset 1 / Blue)
#define BTN_ACTION2   13  // GPIO13 - Action 2 (was Preset 2 / Green)
#define BTN_ACTION3   12  // GPIO12 - Action 3 (was Preset 3 / Yellow)
#define BTN_ACTION4   14  // GPIO14 - Action 4 (was Preset 4 / Red)
#define BTN_ACTION5   27  // GPIO27 - Action 5 (was Bright Up / White)
#define BTN_ACTION6   26  // GPIO26 - Action 6 (was Bright Down / Black)
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
#define PRESET_FIVE 20
#define PRESET_SIX 21
#define PRESET_SEVEN 22
#define PRESET_EIGHT 23
#define PRESET_NINE 24
#define PRESET_TEN 25
#define PRESET_ELEVEN 26
#define PRESET_TWELVE 27
#define PRESET_98 98
#define PRESET_99 99
#define BRIGHT_UP 9
#define BRIGHT_DOWN 8
#define NO_CMD 0xFF  // Sentinel for "no alternate command"

// Button configuration structure
struct ButtonConfig {
  uint8_t pin;
  uint8_t command;
  const char* name;
  bool needsState;  // true if button requires state tracking (like ON/OFF)
};

// Define all buttons and their primary WLED commands
ButtonConfig buttons[] = {
  {BTN_ON_OFF,    0,            "ON/OFF",       true},   // Command set dynamically based on state
  {BTN_ACTION1,   PRESET_ONE,   "Action 1",     false},  // e.g., Blue
  {BTN_ACTION2,   PRESET_TWO,   "Action 2",     false},  // e.g., Green
  {BTN_ACTION3,   PRESET_THREE, "Action 3",     false},  // e.g., Yellow
  {BTN_ACTION4,   PRESET_FOUR,  "Action 4",     false},  // e.g., Red
  {BTN_ACTION5,   PRESET_98,    "Action 5",     false},  // e.g., White
  {BTN_ACTION6,   PRESET_99,    "Action 6",     false},  // e.g., Black
  {BTN_NIGHT,     NIGHT,        "Night Mode",   false}
};

// Optional alternate commands for double / long presses (NO_CMD to disable)
uint8_t doubleCommands[] = {
  NO_CMD,     // ON/OFF double
  PRESET_FIVE,   // Action 1 double -> Preset 5
  PRESET_SIX,    // Action 2 double -> Preset 6
  PRESET_SEVEN,  // Action 3 double -> Preset 7
  PRESET_EIGHT,  // Action 4 double -> Preset 8
  NO_CMD,     // Action 5 double
  NO_CMD,     // Action 6 double
  NO_CMD      // Night Mode double
};

uint8_t longCommands[] = {
  NO_CMD,     // ON/OFF long
  NO_CMD,     // Action 1 long
  NO_CMD,     // Action 2 long
  NO_CMD,     // Action 3 long
  NO_CMD,     // Action 4 long
  NO_CMD,     // Action 5 long
  NO_CMD,     // Action 6 long
  NO_CMD      // Night Mode long
};

const int NUM_BUTTONS = sizeof(buttons) / sizeof(buttons[0]);

// State of the LED - stored in RTC memory to persist through deep sleep
RTC_DATA_ATTR bool is_lightOn = false;

// Sequence number - stored in RTC memory
RTC_DATA_ATTR uint32_t seq = 1;

// Store which button was pressed - stored in RTC memory
RTC_DATA_ATTR uint8_t lastButtonPressed = 255;

// Explicit peers for reliability (Stage-Left, Stage-Right). Update to match your WLED devices.
struct WLEDTarget {
  uint8_t mac[6];
  const char* name;
};

WLEDTarget targets[] = {
  {{0x30, 0xC6, 0xF7, 0x27, 0x58, 0x60}, "Stage-Left"},
  {{0x30, 0xC6, 0xF7, 0x1F, 0xD2, 0xD0}, "Stage-Right"}
};

const int NUM_TARGETS = sizeof(targets) / sizeof(targets[0]);

int retriesCount = 0;

esp_now_peer_info_t peerInfo;
uint8_t activeTargetMac[6] = {0};

// Multi-press tracking
enum PressType { PRESS_NONE, PRESS_SINGLE, PRESS_DOUBLE, PRESS_LONG };

struct ButtonState {
  bool lastReading = HIGH;
  bool stableState = HIGH;
  unsigned long lastDebounce = 0;
  unsigned long pressStart = 0;
  unsigned long lastRelease = 0;
  uint8_t clickCount = 0;
  bool longReported = false;
};

ButtonState buttonStates[NUM_BUTTONS];

struct PressEvent {
  int buttonIndex = -1;
  PressType type = PRESS_NONE;
};

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

// Forward declarations
struct PressEvent;
void handlePressEvent(const PressEvent &evt);
bool pollButtonEvent(PressEvent &evt);

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

void sendMessage(int button) {
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
  message.button = button;

  for (int i = 0; i < NUM_TARGETS; i++) {
    memcpy(activeTargetMac, targets[i].mac, 6);
    Serial.print("Target: ");
    Serial.println(targets[i].name);

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
}

const char* pressTypeName(PressType t) {
  switch (t) {
    case PRESS_SINGLE: return "SINGLE";
    case PRESS_DOUBLE: return "DOUBLE";
    case PRESS_LONG:   return "LONG";
    default:           return "NONE";
  }
}

uint8_t resolveCommandForPress(int buttonIndex, PressType type) {
  if (type == PRESS_SINGLE) {
    if (buttons[buttonIndex].needsState) {
      if (is_lightOn) {
        is_lightOn = false;
        digitalWrite(LED_BUILTIN, LOW);
        return OFF;
      } else {
        is_lightOn = true;
        digitalWrite(LED_BUILTIN, HIGH);
        return ON;
      }
    }
    return buttons[buttonIndex].command;
  }

  if (type == PRESS_DOUBLE) {
    if (doubleCommands[buttonIndex] != NO_CMD) {
      return doubleCommands[buttonIndex];
    }
    // Fallback: behave like single
    return resolveCommandForPress(buttonIndex, PRESS_SINGLE);
  }

  if (type == PRESS_LONG) {
    if (longCommands[buttonIndex] != NO_CMD) {
      return longCommands[buttonIndex];
    }
    // Fallback: behave like single
    return resolveCommandForPress(buttonIndex, PRESS_SINGLE);
  }

  return NO_CMD;
}

void handlePressEvent(const PressEvent &evt) {
  if (evt.buttonIndex < 0 || evt.buttonIndex >= NUM_BUTTONS) return;

  uint8_t commandToSend = resolveCommandForPress(evt.buttonIndex, evt.type);

  Serial.print("\nButton Pressed: ");
  Serial.print(buttons[evt.buttonIndex].name);
  Serial.print(" | Type: ");
  Serial.println(pressTypeName(evt.type));

  if (commandToSend == NO_CMD) {
    Serial.println("No command configured for this press type.");
    return;
  }

  // Non-stateful feedback: blink LED (single=2 blinks, double=3, long=hold)
  if (!buttons[evt.buttonIndex].needsState) {
    if (evt.type == PRESS_LONG) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      int blinks = (evt.type == PRESS_DOUBLE) ? 3 : 2;
      for (int i = 0; i < blinks; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN, LOW);
        delay(50);
      }
    }
  }

  Serial.print("Sending command: ");
  Serial.println(commandToSend);

  sendMessage(commandToSend);
  lastButtonPressed = evt.buttonIndex;
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

bool pollButtonEvent(PressEvent &evt) {
  unsigned long now = millis();
  for (int i = 0; i < NUM_BUTTONS; i++) {
    ButtonState &st = buttonStates[i];
    int reading = digitalRead(buttons[i].pin);

    if (reading != st.lastReading) {
      st.lastDebounce = now;
      st.lastReading = reading;
    }

    if ((now - st.lastDebounce) > MULTI_DEBOUNCE_MS && reading != st.stableState) {
      st.stableState = reading;

      if (st.stableState == LOW) {  // Pressed
        st.pressStart = now;
        st.longReported = false;
      } else {  // Released
        unsigned long pressDuration = now - st.pressStart;
        (void)pressDuration;  // duration is inferred by longReported or timing below
        if (!st.longReported) {
          st.clickCount++;
          st.lastRelease = now;
        } else {
          st.clickCount = 0;  // Already handled long
        }
      }
    }

    // Long press detection while held
    if (!st.longReported && st.stableState == LOW && (now - st.pressStart) >= MULTI_LONG_PRESS_MS) {
      evt.buttonIndex = i;
      evt.type = PRESS_LONG;
      st.longReported = true;
      st.clickCount = 0;
      return true;
    }

    // Single / double determination after release
    if (st.clickCount == 1 && (now - st.lastRelease) > MULTI_DOUBLE_MS) {
      evt.buttonIndex = i;
      evt.type = PRESS_SINGLE;
      st.clickCount = 0;
      return true;
    } else if (st.clickCount >= 2) {
      evt.buttonIndex = i;
      evt.type = PRESS_DOUBLE;
      st.clickCount = 0;
      return true;
    }
  }

  return false;
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
  PressEvent evt;

  while (millis() - startTime < 10000) {
    if (pollButtonEvent(evt)) break;
    delay(5);
  }

  if (evt.type != PRESS_NONE) {
    handlePressEvent(evt);
  } else {
    Serial.println("No button detected during startup window.");
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
    // In debug mode, continuously check for button presses (single, double, long)
    PressEvent evt;
    if (pollButtonEvent(evt)) {
      handlePressEvent(evt);
      Serial.println("Button released\n");
      delay(100);  // Debounce after handling
    }

    delay(50);  // Small delay to prevent excessive CPU usage
  }
  // If not in debug mode, this loop never runs (deep sleep)
}

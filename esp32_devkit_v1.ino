#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Arduino.h>

#define MAX_RETRIES 25
#define TOUCH_THRESHOLD 40

// Modify with the WLED wifi channel
#define CHANNEL 1

// DOIT ESP32 DEVKIT V1 specific pin definitions
// Touch-capable GPIO pins on DOIT ESP32 DEVKIT V1:
// T0=GPIO4, T1=GPIO0, T2=GPIO2, T3=GPIO15, T4=GPIO13,
// T5=GPIO12, T6=GPIO14, T7=GPIO27, T8=GPIO33, T9=GPIO32
#define TOUCH_PIN T0  // GPIO4 - easily accessible on DEVKIT V1
#define LED_BUILTIN 2 // Built-in LED on GPIO2 for DEVKIT V1

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

// State of the LED - stored in RTC memory to persist through deep sleep
RTC_DATA_ATTR bool is_lightOn = false;

// Sequence number - stored in RTC memory
RTC_DATA_ATTR uint32_t seq = 1;

// Modify with the WLED mac address
const uint8_t macAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

int retriesCount = 0;

esp_now_peer_info_t peerInfo;

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
  } else {
    Serial.println("Fail. Retrying");

    if(retriesCount < MAX_RETRIES){
      esp_now_send(macAddress, (uint8_t *)&message, sizeof(message));
      retriesCount += 1;
    }
    retriesCount = 0;
  }
}

void sendMessage(int button) {
  // Increase seq number
  seq += 1;
  Serial.println(button);

  // Format seq number (LSB first)
  message.seq[0] = seq;
  message.seq[1] = seq >> 8;
  message.seq[2] = seq >> 16;
  message.seq[3] = seq >> 24;

  Serial.print("SEQ:");
  Serial.println(seq);

  message.button = button;

  esp_err_t result = esp_now_send(macAddress, (uint8_t *)&message, sizeof(message));

  // Display the error
  switch (result) {
    case ESP_OK:
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

void setup(){
  Serial.begin(115200);

  // Brief delay for serial to initialize
  delay(100);

  Serial.println("\n\n=== ESP32 DEVKIT V1 WLED Remote ===");
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Initialize built-in LED as status indicator
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // WiFi configuration for DEVKIT V1
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // Ensure clean state

  // For newer ESP-IDF versions, channel setting is handled differently
  // The channel is set when adding the peer

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Message sending callback function
  esp_now_register_send_cb(sentStatusAndRetries);

  memcpy(peerInfo.peer_addr, macAddress, 6);
  peerInfo.channel = CHANNEL;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW Initialized Successfully");

  // Setup touch on T0 (GPIO4) as the wake up source from deep sleep
  // GPIO4 is readily accessible on DEVKIT V1
  touchSleepWakeUpEnable(TOUCH_PIN, TOUCH_THRESHOLD);

  Serial.print("Current light state: ");
  Serial.println(is_lightOn ? "ON" : "OFF");

  // Toggle light state
  if(is_lightOn){
    sendMessage(OFF);
    is_lightOn = false;
    Serial.println("Sending: Light OFF");
    digitalWrite(LED_BUILTIN, LOW);
  }
  else{
    sendMessage(ON);
    is_lightOn = true;
    Serial.println("Sending: Light ON");
    digitalWrite(LED_BUILTIN, HIGH);
  }

  // Delay to complete the sending and show LED status
  delay(200);

  Serial.println("Entering deep sleep...");
  Serial.flush();  // Ensure all serial data is sent

  // Deep sleep to reduce power consumption
  // Will wake up on touch of GPIO4
  esp_deep_sleep_start();
}

void loop(){
  // This will never be called due to deep sleep
}

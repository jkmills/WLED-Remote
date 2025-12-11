// WLED HTTP Preset Remote (ESP32 DEVKIT V1)
// Connects to a WiFi AP and triggers presets on WLED instances via HTTP API.
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>

// ---------- WiFi Config ----------
const char* WIFI_SSID     = "MONMOUTHLIGHTS-AP";
const char* WIFI_PASSWORD = "wled1234";

// ---------- WLED Targets ----------
// Add the IPs (or mDNS hostnames) of your WLED instances here.
const char* WLED_TARGETS[] = {
  "192.168.9.101",  // Stage-Left
  "192.168.9.100"   // Stage-Right
};
const int NUM_WLED_TARGETS = sizeof(WLED_TARGETS) / sizeof(WLED_TARGETS[0]);

// Human-friendly target masks
const uint8_t TARGET_LEFT  = 1 << 0;
const uint8_t TARGET_RIGHT = 1 << 1;
const uint8_t TARGET_BOTH  = TARGET_LEFT | TARGET_RIGHT;

// ---------- Button / Preset Mapping ----------
// Pins for DOIT ESP32 DEVKIT V1 (active LOW with INPUT_PULLUP)
#define BTN_ON_OFF    4   // Toggle last state via preset (set to a preset number or 0 to skip)
#define BTN_ACTION1   15  // Preset 1
#define BTN_ACTION2   13  // Preset 2
#define BTN_ACTION3   12  // Preset 3
#define BTN_ACTION4   14  // Preset 4
#define BTN_ACTION5   27  // Preset 5
#define BTN_ACTION6   26  // Preset 6
#define BTN_NIGHT     25  // Night preset (or set to 0 to disable)

#define LED_BUILTIN 2

struct ButtonConfig {
  uint8_t pin;
  uint8_t preset;      // WLED preset number to trigger (0 disables)
  const char* name;
  uint8_t targetMask;  // Use TARGET_LEFT, TARGET_RIGHT, or TARGET_BOTH
};

ButtonConfig buttons[] = {
  {BTN_ON_OFF,  0,  "On/Off",   TARGET_BOTH},  // adjust to a preset that represents "on"; use 0 to disable
  {BTN_ACTION1, 1,  "Action 1 RIGHT", TARGET_RIGHT},
  {BTN_ACTION2, 1,  "Action 1 LEFT", TARGET_LEFT},
  {BTN_ACTION3, 10,  "Action 10 RIGHT", TARGET_RIGHT},
  {BTN_ACTION4, 10,  "Action 10 LEFT", TARGET_LEFT},
  {BTN_ACTION5, 99, "Action 5", TARGET_BOTH},
  {BTN_ACTION6, 98, "Action 6", TARGET_BOTH},
  {BTN_NIGHT,   0,  "Night",    TARGET_BOTH}
};

const int NUM_BUTTONS = sizeof(buttons) / sizeof(buttons[0]);

// Multi-press timing
const unsigned long DEBOUNCE_MS      = 40;
const unsigned long DOUBLE_PRESS_MS  = 400;
const unsigned long LONG_PRESS_MS    = 800;

// Multi-press types/state tracking
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

struct PressEvent {
  int buttonIndex = -1;
  PressType type = PRESS_NONE;
};

ButtonState states[NUM_BUTTONS];

// Action for a press type
struct PressAction {
  uint8_t preset;
  uint8_t targetMask;
};

// Forward declarations to avoid Arduino auto-proto ordering issues
bool pollPressEvent(PressEvent &evt);
PressAction actionForEvent(int idx, PressType t);

// Optional alternate actions for double / long presses (preset + targetMask). preset=0 disables.
PressAction doubleActions[] = {
  {0,   TARGET_BOTH}, // On/Off double
  {99,   TARGET_RIGHT}, // Action 1 double
  {99,   TARGET_LEFT}, // Action 2 double
  {99,   TARGET_RIGHT}, // Action 3 double
  {99,   TARGET_LEFT}, // Action 4 double
  {99,  TARGET_BOTH}, // Action 5 double
  {98,  TARGET_BOTH}, // Action 6 double
  {0,   TARGET_BOTH}  // Night double
};

PressAction longActions[] = {
  {0,   TARGET_BOTH}, // On/Off long
  {10,  TARGET_BOTH}, // Action 1 long
  {11,  TARGET_BOTH}, // Action 2 long
  {12,  TARGET_BOTH}, // Action 3 long
  {13,  TARGET_BOTH}, // Action 4 long
  {14,  TARGET_BOTH}, // Action 5 long
  {15,  TARGET_BOTH}, // Action 6 long
  {0,   TARGET_BOTH}  // Night long
};

// ---------- Helpers ----------
void blinkStatus(int times, int onMs = 80, int offMs = 80) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(onMs);
    digitalWrite(LED_BUILTIN, LOW);
    delay(offMs);
  }
}

bool sendPresetToTarget(const char* target, uint8_t preset) {
  if (preset == 0) return false;

  HTTPClient http;
  http.setTimeout(1500);

  // Use JSON API (preferred): POST /json/state {"ps":<preset>}
  String url = String("http://") + target + "/json/state";
  String body = String("{\"ps\":") + preset + "}";

  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(body);

  // If JSON fails, fall back to classic API /win&P=<preset>
  if (code <= 0 || code >= 400) {
    http.end();
    http.begin(String("http://") + target + "/win&P=" + preset);
    code = http.GET();
  }

  http.end();
  return (code >= 200 && code < 300);
}

void triggerPreset(uint8_t preset, uint8_t targetMask) {
  Serial.print("Triggering preset ");
  Serial.println(preset);

  bool anySuccess = false;
  for (int i = 0; i < NUM_WLED_TARGETS; i++) {
    if (targetMask & (1 << i)) {
      bool ok = sendPresetToTarget(WLED_TARGETS[i], preset);
      Serial.print("  -> ");
      Serial.print(WLED_TARGETS[i]);
      Serial.print(" : ");
      Serial.println(ok ? "OK" : "FAIL");
      anySuccess = anySuccess || ok;
    }
  }

  // Blink LED to indicate success/failure
  if (anySuccess) {
    blinkStatus(2);
  } else {
    blinkStatus(1, 50, 200);  // slower blink for failure
  }
}

bool pollPressEvent(PressEvent &evt) {
  unsigned long now = millis();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    ButtonState &st = states[i];
    int reading = digitalRead(buttons[i].pin);

    if (reading != st.lastReading) {
      st.lastDebounce = now;
      st.lastReading = reading;
    }

    if ((now - st.lastDebounce) > DEBOUNCE_MS && reading != st.stableState) {
      st.stableState = reading;

      if (st.stableState == LOW) {  // pressed
        st.pressStart = now;
        st.longReported = false;
      } else {  // released
        if (!st.longReported) {
          st.clickCount++;
          st.lastRelease = now;
        } else {
          st.clickCount = 0;  // already handled long
        }
      }
    }

    // Long press while held
    if (!st.longReported && st.stableState == LOW && (now - st.pressStart) >= LONG_PRESS_MS) {
      evt.buttonIndex = i;
      evt.type = PRESS_LONG;
      st.longReported = true;
      st.clickCount = 0;
      return true;
    }

    // Single / double after release
    if (st.clickCount == 1 && (now - st.lastRelease) > DOUBLE_PRESS_MS) {
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

PressAction actionForEvent(int idx, PressType t) {
  PressAction act = {0, TARGET_BOTH};
  if (idx < 0 || idx >= NUM_BUTTONS) return act;

  if (t == PRESS_DOUBLE && doubleActions[idx].preset > 0) return doubleActions[idx];
  if (t == PRESS_LONG   && longActions[idx].preset   > 0) return longActions[idx];

  act.preset = buttons[idx].preset;
  act.targetMask = buttons[idx].targetMask;
  return act;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Buttons
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }

  Serial.println("\n=== WLED HTTP Preset Remote ===");
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
    blinkStatus(3);
  } else {
    Serial.println("WiFi connect failed. Continuing without network.");
    blinkStatus(5, 100, 100);
  }

  Serial.println("Ready. Press buttons to trigger presets.");
}

// ---------- Loop ----------
void loop() {
  PressEvent evt;
  if (pollPressEvent(evt)) {
    PressAction action = actionForEvent(evt.buttonIndex, evt.type);

    Serial.print("Button: ");
    Serial.print(buttons[evt.buttonIndex].name);
    Serial.print(" | Type: ");
    Serial.print(evt.type == PRESS_SINGLE ? "SINGLE" :
                 evt.type == PRESS_DOUBLE ? "DOUBLE" : "LONG");
    Serial.print(" | Preset: ");
    Serial.print(action.preset);
    Serial.print(" | Targets: ");
    Serial.println(action.targetMask, BIN);

    if (WiFi.status() == WL_CONNECTED && action.preset > 0) {
      triggerPreset(action.preset, action.targetMask);
    } else {
      Serial.println("Skipped: WiFi not connected or preset disabled.");
      blinkStatus(1, 50, 200);
    }
  }

  delay(5);
}

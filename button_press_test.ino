// Button press tester for ESP32 DEVKIT V1
// Detects single, double, and long presses on all configured buttons.
#include <Arduino.h>

// Button pin definitions for DOIT ESP32 DEVKIT V1
#define BTN_ON_OFF    4
#define BTN_PRESET1   15
#define BTN_PRESET2   13
#define BTN_PRESET3   12
#define BTN_PRESET4   14
#define BTN_BRIGHT_UP 27
#define BTN_BRIGHT_DN 26
#define BTN_NIGHT     25

#define LED_BUILTIN 2  // On-board LED

struct ButtonConfig {
  uint8_t pin;
  const char* name;
};

ButtonConfig buttons[] = {
  {BTN_ON_OFF,    "ON/OFF"},
  {BTN_PRESET1,   "Preset 1"},
  {BTN_PRESET2,   "Preset 2"},
  {BTN_PRESET3,   "Preset 3"},
  {BTN_PRESET4,   "Preset 4"},
  {BTN_BRIGHT_UP, "Bright Up"},
  {BTN_BRIGHT_DN, "Bright Down"},
  {BTN_NIGHT,     "Night Mode"}
};

const int NUM_BUTTONS = sizeof(buttons) / sizeof(buttons[0]);

// Timing parameters (tweak as needed)
const unsigned long DEBOUNCE_MS   = 30;
const unsigned long DOUBLE_MS     = 400;
const unsigned long LONG_PRESS_MS = 800;

struct ButtonState {
  bool lastReading = HIGH;
  bool stableState = HIGH;
  unsigned long lastDebounce = 0;
  unsigned long pressStart = 0;
  unsigned long lastRelease = 0;
  uint8_t clickCount = 0;
  bool longReported = false;
};

ButtonState states[NUM_BUTTONS];

void indicateEvent(uint8_t flashes, bool longHold) {
  if (longHold) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    return;
  }

  for (uint8_t i = 0; i < flashes; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(80);
    digitalWrite(LED_BUILTIN, LOW);
    delay(80);
  }
}

void reportEvent(const char* name, const char* type) {
  Serial.print("[");
  Serial.print(name);
  Serial.print("] ");
  Serial.println(type);

  if (strcmp(type, "LONG") == 0) {
    indicateEvent(0, true);
  } else if (strcmp(type, "DOUBLE") == 0) {
    indicateEvent(2, false);
  } else {
    indicateEvent(1, false);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=== ESP32 Button Press Tester ===");
  Serial.print("Buttons monitored: ");
  Serial.println(NUM_BUTTONS);
  Serial.println("Detects: SINGLE, DOUBLE (<=400ms), LONG (>=800ms)");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
    Serial.print("  - GPIO");
    Serial.print(buttons[i].pin);
    Serial.print(": ");
    Serial.println(buttons[i].name);
  }

  Serial.println("\nPress buttons to see detected events...\n");
}

void loop() {
  unsigned long now = millis();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    ButtonState &st = states[i];
    int reading = digitalRead(buttons[i].pin);

    // Debounce on any change
    if (reading != st.lastReading) {
      st.lastDebounce = now;
      st.lastReading = reading;
    }

    if ((now - st.lastDebounce) > DEBOUNCE_MS && reading != st.stableState) {
      st.stableState = reading;

      if (st.stableState == LOW) {  // Pressed
        st.pressStart = now;
        st.longReported = false;
      } else {  // Released
        unsigned long pressDuration = now - st.pressStart;
        if (!st.longReported) {
          st.clickCount++;
          st.lastRelease = now;
        } else {
          st.clickCount = 0;  // Long already handled
        }
      }
    }

    // Long press detection while held
    if (!st.longReported && st.stableState == LOW && (now - st.pressStart) >= LONG_PRESS_MS) {
      reportEvent(buttons[i].name, "LONG");
      st.longReported = true;
      st.clickCount = 0;
    }

    // Double / single decision after release
    if (st.clickCount == 1 && (now - st.lastRelease) > DOUBLE_MS) {
      reportEvent(buttons[i].name, "SINGLE");
      st.clickCount = 0;
    } else if (st.clickCount >= 2) {
      reportEvent(buttons[i].name, "DOUBLE");
      st.clickCount = 0;
    }
  }

  delay(5);  // Light touch delay to reduce CPU churn
}

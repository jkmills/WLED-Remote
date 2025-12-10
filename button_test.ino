// Simple button test for ESP32
// Tests GPIO15 button connection

#define TEST_BUTTON 15  // GPIO15
#define LED_BUILTIN 2   // Built-in LED

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=== Button Test ===");
  Serial.println("Testing GPIO15 button");

  pinMode(TEST_BUTTON, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Press button on GPIO15...");
}

void loop() {
  int buttonState = digitalRead(TEST_BUTTON);

  if (buttonState == LOW) {  // Button pressed
    Serial.println("BUTTON PRESSED!");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(50);
}

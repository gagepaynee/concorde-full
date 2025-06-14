#define REED_PIN D5  // GPIO14

bool lastState = HIGH;

void setup() {
  pinMode(REED_PIN, INPUT_PULLUP);  // use internal pull-up
  Serial.begin(115200);
}

void loop() {
  bool currentState = digitalRead(REED_PIN);

  if (currentState != lastState) {
    if (currentState == HIGH) {
      Serial.println("Cabinet opened");
      // send WebSocket message here
    } else {
      Serial.println("Cabinet closed");
    }
    lastState = currentState;
  }

  delay(50); // debounce delay
}

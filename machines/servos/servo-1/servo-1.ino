#include "../../servo/ServoController.h"
#include "../../shared/shared_secrets.h"
#include "secrets.h"

void setup() {
  servoControllerSetup(SERVO_ID);
}

void loop() {
  servoControllerLoop();
}

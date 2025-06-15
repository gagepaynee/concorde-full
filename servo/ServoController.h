#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H
#include <Arduino.h>

void servoControllerSetup(const String &deviceId);
void servoControllerLoop();

#endif

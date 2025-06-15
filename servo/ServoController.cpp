#include "ServoController.h"
#include <Servo.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include "../shared/shared_secrets.h"

static Servo myservo;
static const int servoPin = D4;
static String servoId;
static WebSocketsClient webSocket;
static bool isSpinning = false;
static int currentPos = 0;
static int stepDir = 1;

void servoControllerSetup(const String &deviceId) {
  servoId = deviceId;
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println(" connected!");

  webSocket.beginSSL(SERVER_IP, atoi(SERVER_PORT), "/");
  webSocket.onEvent([](WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
      case WStype_CONNECTED: {
        Serial.println("WebSocket connected");
        String payloadStr = String("{\"event\":\"register\",\"id\":\"") + servoId + "\"}";
        webSocket.sendTXT(payloadStr);
        break;
      }
      case WStype_DISCONNECTED:
        Serial.println("WebSocket disconnected");
        break;
      case WStype_TEXT:
        Serial.printf("WS Message: %s\n", payload);
        if (strstr((char*)payload, "start_spin")) {
          isSpinning = true;
        } else if (strstr((char*)payload, "stop_spin")) {
          isSpinning = false;
          currentPos = 0;
          stepDir = 1;
          myservo.write(0);
        }
        break;
      default:
        break;
    }
  });
  webSocket.setReconnectInterval(5000);

  Serial.print("Connecting to WebSocket...");
  while (!webSocket.isConnected()) {
    webSocket.loop();
    delay(100);
  }
  Serial.println(" connected!");

  myservo.attach(servoPin);
}

void servoControllerLoop() {
  webSocket.loop();
  if (isSpinning) {
    myservo.write(currentPos);
    currentPos += stepDir;
    if (currentPos >= 180) {
      currentPos = 180;
      stepDir = -1;
    } else if (currentPos <= 0) {
      currentPos = 0;
      stepDir = 1;
    }
    delay(5);
  } else {
    delay(10);
  }
}

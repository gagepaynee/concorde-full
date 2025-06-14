#include <Servo.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include "secrets.h"

Servo myservo;  // create servo object
const int servoPin = D4;  // GPIO2
const String ssid = WIFI_SSID;
const String password = WIFI_PASSWORD;
const String servoId = SERVO_ID;
WebSocketsClient webSocket;
bool isSpinning = false;
int currentPos = 0;
int stepDir = 1;

void setup() {
    Serial.begin(115200);
    // Connect to Wi-Fi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected!");

  // Initialize WebSocket connection
  // SERVER_IP is defined as a String in secrets.h, but beginSSL expects a
  // const char*. Use c_str() to convert the String to the expected type.
  webSocket.beginSSL(SERVER_IP.c_str(), SERVER_PORT.toInt(), "/");
  webSocket.onEvent([](WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
      case WStype_CONNECTED: {
        Serial.println("WebSocket connected");
        // Register this reader with the server using a predefined id
        String payload = "{\"event\":\"register\",\"id\":\"" + servoId + "\"}";
        webSocket.sendTXT(payload);
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

  // Wait until the WebSocket connection is established before proceeding
  Serial.print("Connecting to WebSocket...");
  while (!webSocket.isConnected()) {
    webSocket.loop();
    delay(100);
  }
  Serial.println(" connected!");

  myservo.attach(servoPin);  // attaches the servo on D4
}

void loop() {
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

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include "secrets.h"

#define REED_PIN D5  // GPIO14

WebSocketsClient webSocket;
bool lastState = HIGH;

const String ssid = WIFI_SSID;
const String password = WIFI_PASSWORD;
const String magnetId = MAGNET_ID;

void setup() {
  pinMode(REED_PIN, INPUT_PULLUP);  // use internal pull-up
  Serial.begin(115200);

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println(" connected!");

  // SERVER_IP is defined as a String in secrets.h, but beginSSL expects a const char*
  webSocket.beginSSL(SERVER_IP.c_str(), SERVER_PORT.toInt(), "/");
  webSocket.onEvent([](WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
      case WStype_CONNECTED: {
        Serial.println("WebSocket connected");
        String payload = "{\"event\":\"register\",\"id\":\"" + magnetId + "\"}";
        webSocket.sendTXT(payload);
        break;
      }
      case WStype_DISCONNECTED:
        Serial.println("WebSocket disconnected");
        break;
      case WStype_TEXT:
        Serial.printf("WS Message: %s\n", payload);
        break;
      default:
        break;
    }
  });
  webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2);

  Serial.print("Connecting to WebSocket...");
  while (!webSocket.isConnected()) {
    webSocket.loop();
    delay(100);
  }
  Serial.println(" connected!");
}

void loop() {
  webSocket.loop();
  bool currentState = digitalRead(REED_PIN);

  if (currentState != lastState) {
    String stateStr = currentState == HIGH ? "open" : "closed";
    Serial.printf("Cabinet %s\n", stateStr.c_str());
    sendStateToServer(stateStr);
    lastState = currentState;
  }

  delay(50); // debounce delay
}

void sendStateToServer(String state) {
  if (WiFi.status() == WL_CONNECTED && webSocket.isConnected()) {
    String payload = "{\"event\":\"magnet\",\"id\":\"" + magnetId + "\",\"state\":\"" + state + "\"}";
    webSocket.sendTXT(payload);
  } else {
    Serial.println("WiFi/WebSocket not connected. Cannot send state.");
    webSocket.loop();
  }
}

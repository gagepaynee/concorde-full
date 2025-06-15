#include "MagnetReader.h"
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include "../shared/shared_secrets.h"

#define REED_PIN D5

static String magnetId;
static WebSocketsClient webSocket;
static bool lastState = HIGH;

void magnetReaderSetup(const String &deviceId) {
  magnetId = deviceId;
  pinMode(REED_PIN, INPUT_PULLUP);
  Serial.begin(115200);

  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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
        String payload = String("{\"event\":\"register\",\"id\":\"") + magnetId + "\"}";
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

static void sendStateToServer(String state) {
  if (WiFi.status() == WL_CONNECTED && webSocket.isConnected()) {
    String payload = String("{\"event\":\"magnet\",\"id\":\"") + magnetId + "\",\"state\":\"" + state + "\"}";
    webSocket.sendTXT(payload);
  } else {
    Serial.println("WiFi/WebSocket not connected. Cannot send state.");
    webSocket.loop();
  }
}

void magnetReaderLoop() {
  webSocket.loop();
  bool currentState = digitalRead(REED_PIN);

  if (currentState != lastState) {
    String stateStr = currentState == HIGH ? "open" : "closed";
    Serial.printf("Cabinet %s\n", stateStr.c_str());
    sendStateToServer(stateStr);
    lastState = currentState;
  }
  delay(50);
}

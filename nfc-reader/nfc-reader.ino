#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include "secrets.h"

const String ssid = WIFI_SSID;
const String password = WIFI_PASSWORD;
const String readerId = READER_ID;


///////////////////////
// NFC Setup
///////////////////////
PN532_I2C pn532_i2c(Wire);
PN532 nfc(pn532_i2c);
WebSocketsClient webSocket;

void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1); // SDA, SCL on ESP8266

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
        String payload = "{\"event\":\"register\",\"id\":\"" + readerId + "\"}";
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

  // Wait until the WebSocket connection is established before proceeding
  Serial.print("Connecting to WebSocket...");
  while (!webSocket.isConnected()) {
    webSocket.loop();
    delay(100);
  }
  Serial.println(" connected!");

  // NFC init
  nfc.begin();
  uint32_t version = nfc.getFirmwareVersion();
  if (!version) {
    Serial.println("Didn't find PN53x board");
    while (1);
  }
  nfc.SAMConfig();  // enable secure access module
  Serial.println("Waiting for an NFC tag...");
}

void loop() {
  webSocket.loop();
  uint8_t uid[7];
  uint8_t uidLength;

  // Poll quickly so the WebSocket client can continue handling reconnects
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100)) {
    Serial.print("Tag UID: ");
    String uidStr = "";
    for (uint8_t i = 0; i < uidLength; i++) {
      if (uid[i] < 0x10) uidStr += "0";
      uidStr += String(uid[i], HEX);
      uidStr += (i < uidLength - 1) ? ":" : "";
    }
    Serial.println(uidStr);

    sendUIDtoServer(uidStr);

    // Wait for tag removal before reading again
    while (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50)) {
      webSocket.loop();
      delay(50);
    }
    Serial.println("Tag removed. Waiting for next...");
  }
  // Small delay so webSocket.loop() runs often even when no tag is present
  delay(10);
}

void sendUIDtoServer(String uid) {
  if (WiFi.status() == WL_CONNECTED && webSocket.isConnected()) {
    String payload = "{\"event\":\"nfc\",\"id\":\"" + readerId + "\",\"uuid\":\"" + uid + "\"}";
    webSocket.sendTXT(payload);
    Serial.println("UID sent over WebSocket");
  } else {
    Serial.println("WiFi/WebSocket not connected. Cannot send UID.");
    // give the WebSocket client a chance to reconnect
    webSocket.loop();
  }
}


#include "NfcReader.h"
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include "../shared/shared_secrets.h"

static String readerId;
static PN532_I2C pn532_i2c(Wire);
static PN532 nfc(pn532_i2c);
static WebSocketsClient webSocket;

void nfcReaderSetup(const String &deviceId) {
  readerId = deviceId;
  Serial.begin(115200);
  Wire.begin(D2, D1);

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
        String payloadStr = String("{\"event\":\"register\",\"id\":\"") + readerId + "\"}";
        webSocket.sendTXT(payloadStr);
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

  nfc.begin();
  uint32_t version = nfc.getFirmwareVersion();
  if (!version) {
    Serial.println("Didn't find PN53x board");
    while (1);
  }
  nfc.SAMConfig();
  Serial.println("Waiting for an NFC tag...");
}

static void sendUIDtoServer(String uid) {
  if (WiFi.status() == WL_CONNECTED && webSocket.isConnected()) {
    String payload = String("{\"event\":\"nfc\",\"id\":\"") + readerId + "\",\"uuid\":\"" + uid + "\"}";
    webSocket.sendTXT(payload);
    Serial.println("UID sent over WebSocket");
  } else {
    Serial.println("WiFi/WebSocket not connected. Cannot send UID.");
    webSocket.loop();
  }
}

void nfcReaderLoop() {
  webSocket.loop();
  uint8_t uid[7];
  uint8_t uidLength;

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

    while (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50)) {
      webSocket.loop();
      delay(50);
    }
    Serial.println("Tag removed. Waiting for next...");
  }
  delay(10);
}

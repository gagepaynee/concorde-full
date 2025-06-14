#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include "secrets.h"

const String ssid = WIFI_SSID;
const String password = WIFI_PASSWORD;

const String serverUrl = SERVER_URL+":"+SERVER_PORT;  // Change to your Pi's IP + endpoint

///////////////////////
// NFC Setup
///////////////////////
PN532_I2C pn532_i2c(Wire);
PN532 nfc(pn532_i2c);

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
  uint8_t uid[7];
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000)) {
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
    while (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100)) {
      delay(100);
    }
    Serial.println("Tag removed. Waiting for next...");
  }
}

void sendUIDtoServer(String uid) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;               // Step 1: Create WiFiClient
    HTTPClient http;

    http.begin(client, serverUrl);   // Step 2: Use new begin() signature
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"uid\":\"" + uid + "\"}";
    int response = http.POST(payload);

    Serial.print("Sent to server. Status: ");
    Serial.println(response);
    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot send UID.");
  }
}


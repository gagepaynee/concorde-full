# Concorde Escape Room Controller

This repository contains all code for a simple WebSocket-based server and a companion NFC reader used in an escape room setting.

```
concorde-full/
├── concorde-controller/  # TypeScript WebSocket server
├── nfc-reader/           # ESP8266/PN532 based NFC reader project
```

## Getting Started

### Server Setup

The server lives in `concorde-controller`. Install dependencies with npm and start the server:

```bash
cd concorde-controller
npm install
npm start
```

The server requires the following environment variables (see `.env.example`):

- `KEY_PATH` – path to your TLS private key
- `CERT_PATH` – path to the TLS certificate
- `PORT` – port number to listen on

Create a `.env` file with these values before starting the server.

### NFC Reader

The `nfc-reader` folder contains an Arduino sketch (`nfc-reader.ino`) built for an ESP8266 and PN532 module. Compile and flash it using the Arduino IDE or PlatformIO. The sketch expects a `secrets.h` file (not included) defining Wi‑Fi credentials and server connection details.

```
#define WIFI_SSID "your-ssid"
#define WIFI_PASSWORD "your-password"
#define SERVER_IP "your.server"
#define SERVER_PORT "3000"
#define READER_ID "reader-01"
```

Build and upload the sketch to your microcontroller, then open the serial monitor to verify that it connects.


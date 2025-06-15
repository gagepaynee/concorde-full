# Concorde Escape Room Controller

This repository contains the WebSocket server and Arduino code used to control puzzles in an escape room.

```
concorde-full/
├── concorde-controller/  # TypeScript WebSocket server
├── nfc-reader/           # Shared NFC reader logic
├── magnet-reader/        # Shared reed switch logic
├── servo/                # Shared servo controller logic
├── machines/             # Device specific sketches
├── shared/               # Common configuration headers
```

## Getting Started

### Server Setup

```bash
cd concorde-controller
npm install
npm start
```

Create a `.env` based on `.env.example` with your TLS certificate paths and port.

### Arduino Devices

Copy `shared/shared_secrets.example.h` to `shared/shared_secrets.h` and set your Wi-Fi credentials and server address.
For each device under `machines/`, copy the `secrets.example.h` file to `secrets.h` and set a unique ID.
Open the `.ino` file in the Arduino IDE or PlatformIO and upload it to your ESP8266.

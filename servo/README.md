# Servo Motor Controller

This folder contains an Arduino sketch (`servo.ino`) for driving a hobby servo via WebSocket messages on an ESP8266.

## Hardware Required

- ESP8266 microcontroller
- Servo motor connected to pin **D4** (GPIO2)

## `secrets.h`

Create a `secrets.h` file alongside the sketch and define the following macros:

```
#define WIFI_SSID "your-ssid"
#define WIFI_PASSWORD "your-password"
#define SERVER_IP "your.server"
#define SERVER_PORT "3000"
#define SERVO_ID "servo-01"
```

## Building and Uploading

Open `servo.ino` with the Arduino IDE or PlatformIO. Select your ESP8266 board, compile and flash the sketch. In PlatformIO, run:

```bash
pio run -t upload
```

After uploading, open the serial monitor to confirm Wi-Fi and WebSocket connections.

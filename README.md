# DNSLab IoT

DNSLab is an IoT connectivity library for ESP32.

The first version provides:

- WiFi connection
- Automatic WiFi reconnect
- MQTT connection
- Automatic MQTT reconnect
- MQTT publish
- MQTT subscribe
- MQTT message callbacks
- Simple API

## Requirements

- ESP32
- Arduino IDE or PlatformIO
- PubSubClient library

## Installation

Install the `PubSubClient` library from the Arduino Library Manager.

Then install DNSLab.

## Basic Usage

```cpp
#include <DNSLab.h>

DNSLab dns;

void setup() {

    dns.begin(
        "WiFi_Name",
        "WiFi_Password"
    );
}

void loop() {

    dns.loop();
}
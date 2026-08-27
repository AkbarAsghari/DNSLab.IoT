# DNSLab.IoT

Arduino / ESP32 / ESP8266 IoT Library for connecting devices to the DNSLab MQTT Hub.

DNSLab.IoT provides a simple API for:

* WiFi connection
* MQTT connection
* Tenant & Device identification
* Publish
* Subscribe
* Message callbacks
* Automatic MQTT reconnect
* Automatic subscription restore
* Configurable MQTT buffer size
* MQTT topic namespace

---

## Features

* ESP32 support
* ESP8266 support
* WiFi management
* MQTT management
* Automatic MQTT reconnect
* Automatic subscription restore
* Tenant-based MQTT namespace
* Device-based MQTT namespace
* Publish messages
* Subscribe to topics
* Message callbacks
* Configurable MQTT packet buffer
* Simple Arduino-style API

---

## Installation

### Arduino IDE

Go to:

**Sketch → Include Library → Add .ZIP Library...**

Then select the DNSLab.IoT ZIP file.

You can also manually install the library in:

```
Documents/
└── Arduino/
    └── libraries/
        └── DNSLab.IoT/
```

Restart Arduino IDE after installation.

---

## Supported Boards

Currently supported:

* ESP32
* ESP8266

Arduino boards without WiFi support are not currently supported.

---

# Basic Usage

The simplest way to use DNSLab.IoT:

```
#include <DNSLab.h>

DNSLab dnslab;

void setup()
{
    Serial.begin(115200);

    dnslab.begin();

    dnslab.Wifi.begin(
        "YOUR_WIFI_NAME",
        "YOUR_WIFI_PASSWORD"
    );
}

void loop()
{
    dnslab.loop();
}
```

---

# MQTT Usage

To connect a device to the DNSLab MQTT Hub:

```
#include <DNSLab.h>

DNSLab dnslab;

void setup()
{
    Serial.begin(115200);

    dnslab.begin();

    dnslab.Wifi.begin(
        "YOUR_WIFI_NAME",
        "YOUR_WIFI_PASSWORD"
    );

    dnslab.MQTT.begin(
        "Project1",
        "Device456"
    );
}

void loop()
{
    dnslab.loop();
}
```

The default MQTT server is:

```
mqtt.dnslab.ir
```

Default port:

```
8883
```

---

# Custom MQTT Server

The default server can be changed:

```
dnslab.begin(
    "mqtt.example.com",
    8883
);
```

For example:

```
dnslab.begin(
    "mqtt.dnslab.ir",
    8883
);
```

If no server is specified:

```
dnslab.begin();
```

the library uses:

```
mqtt.dnslab.ir:8883
```

---

# WiFi

DNSLab.IoT provides its own WiFi manager.

```
dnslab.Wifi.begin(
    "YOUR_WIFI_NAME",
    "YOUR_WIFI_PASSWORD"
);
```

Check WiFi status:

```
if (dnslab.Wifi.connected())
{
    Serial.println("WiFi Connected");
}
```

---

# MQTT

Initialize MQTT with a Tenant ID and Device ID:

```
dnslab.MQTT.begin(
    "Project1",
    "Device456"
);
```

The parameters are:

* Tenant ID
* Device ID

Example:

```
Tenant: Project1
Device: Device456
```

---

# MQTT Topic Namespace

DNSLab.IoT automatically creates a device-specific MQTT namespace.

If you subscribe to:

```
dnslab.MQTT.subscribe(
    "Notification"
);
```

the actual MQTT topic becomes:

```
tenant/Project1/device/Device456/Notification
```

The user does not need to manually construct the full topic.

---

# Subscribe

Subscribe to a topic:

```
dnslab.MQTT.subscribe(
    "Notification"
);
```

Multiple subscriptions are supported:

```
dnslab.MQTT.subscribe("Notification");

dnslab.MQTT.subscribe("Command");

dnslab.MQTT.subscribe("Update");
```

Subscriptions are automatically restored after MQTT reconnect.

---

# Message Callback

Register a callback:

```
dnslab.MQTT.onMessage(
    [](const char* topic, const char* message)
    {
        Serial.print("Topic: ");
        Serial.println(topic);

        Serial.print("Message: ");
        Serial.println(message);
    }
);
```

Complete example:

```
#include <DNSLab.h>

DNSLab dnslab;

void setup()
{
    Serial.begin(115200);

    dnslab.begin();

    dnslab.Wifi.begin(
        "YOUR_WIFI_NAME",
        "YOUR_WIFI_PASSWORD"
    );

    dnslab.MQTT.begin(
        "Project1",
        "Device456"
    );

    dnslab.MQTT.onMessage(
        [](const char* topic, const char* message)
        {
            Serial.print("Topic: ");
            Serial.println(topic);

            Serial.print("Message: ");
            Serial.println(message);
        }
    );

    dnslab.MQTT.subscribe(
        "Notification"
    );
}

void loop()
{
    dnslab.loop();
}
```

---

# Publish

Publish a message:

```
dnslab.MQTT.publish(
    "Notification",
    "Hello from ESP32"
);
```

The actual topic becomes:

```
tenant/Project1/device/Device456/Notification
```

---

# Publish String

You can also publish an Arduino `String`:

```
String message = "Hello DNSLab";

dnslab.MQTT.publish(
    "Notification",
    message
);
```

---

# Buffer Size

For larger MQTT messages, the MQTT packet buffer can be configured:

```
dnslab.MQTT.setBufferSize(
    2048
);
```

Example:

```
dnslab.begin();

dnslab.Wifi.begin(
    "YOUR_WIFI_NAME",
    "YOUR_WIFI_PASSWORD"
);

dnslab.MQTT.setBufferSize(
    2048
);

dnslab.MQTT.begin(
    "Project1",
    "Device456"
);
```

Larger buffers consume more RAM.

---

# MQTT Connection Status

Check whether MQTT is connected:

```
if (dnslab.MQTT.connected())
{
    Serial.println("MQTT Connected");
}
else
{
    Serial.println("MQTT Disconnected");
}
```

---

# Automatic Reconnect

DNSLab.IoT automatically attempts to reconnect to MQTT when the connection is lost.

No additional reconnect code is required.

Make sure the main loop contains:

```
void loop()
{
    dnslab.loop();
}
```

The library uses this loop to:

* Process MQTT packets
* Detect disconnections
* Reconnect to MQTT
* Restore subscriptions

---

# Disconnect

You can manually disconnect MQTT:

```
dnslab.MQTT.disconnect();
```

---

# Unsubscribe

Remove a subscription:

```
dnslab.MQTT.unsubscribe(
    "Notification"
);
```

---

# Debug

Debug logging is enabled by default.

Disable it:

```
dnslab.MQTT.setDebug(false);
```

Enable it:

```
dnslab.MQTT.setDebug(true);
```

Check debug status:

```
if (dnslab.MQTT.debugEnabled())
{
    Serial.println("Debug enabled");
}
```

Example debug output:

```
[DNSLab] MQTT initialized
[DNSLab] Tenant: Project1
[DNSLab] Device: Device456
[DNSLab] Client ID: DNSLab-XXXXXXXX
[DNSLab] Connecting to MQTT...
[DNSLab] MQTT connected
[DNSLab] Subscribed: tenant/Project1/device/Device456/Notification -> OK
```

---

# Client ID

The MQTT Client ID is generated automatically by the library.

For ESP32:

```
DNSLab-XXXXXXXX
```

For ESP8266:

```
DNSLab-XXXXXXXX
```

The Device ID is used for the MQTT topic namespace and is independent from the automatically generated MQTT Client ID.

---

# Complete Example

```
#include <DNSLab.h>

DNSLab dnslab;

const char* WIFI_SSID =
    "YOUR_WIFI_NAME";

const char* WIFI_PASSWORD =
    "YOUR_WIFI_PASSWORD";

const char* TENANT_ID =
    "Project1";

const char* DEVICE_ID =
    "Device456";

void setup()
{
    Serial.begin(115200);

    delay(1000);

    // Default:
    // mqtt.dnslab.ir:8883

    dnslab.begin();

    // WiFi

    dnslab.Wifi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    // MQTT Buffer

    dnslab.MQTT.setBufferSize(
        2048
    );

    // MQTT

    dnslab.MQTT.begin(
        TENANT_ID,
        DEVICE_ID
    );

    // Message Callback

    dnslab.MQTT.onMessage(
        [](const char* topic, const char* message)
        {
            Serial.print("Topic: ");
            Serial.println(topic);

            Serial.print("Message: ");
            Serial.println(message);
        }
    );

    // Subscribe

    dnslab.MQTT.subscribe(
        "Notification"
    );
}

void loop()
{
    dnslab.loop();
}
```

---

# Project Structure

```
DNSLab.IoT/
│
├── src/
│   ├── DNSLab.h
│   ├── DNSLab.cpp
│   │
│   ├── DNSLabWifi.h
│   ├── DNSLabWifi.cpp
│   │
│   ├── DNSLabMQTT.h
│   └── DNSLabMQTT.cpp
│
├── examples/
│   │
│   ├── Basics/
│   │   └── Basics.ino
│   │
│   └── MQTT/
│       └── MQTT.ino
│
├── library.properties
├── README.md
└── LICENSE
```

---

# Architecture

DNSLab.IoT is divided into three main components:

```
DNSLab
 │
 ├── Wifi
 │      └── WiFi connection & status
 │
 └── MQTT
        ├── MQTT connection
        ├── Tenant
        ├── Device
        ├── Publish
        ├── Subscribe
        ├── Callback
        ├── Reconnect
        └── Topic namespace
```

---

# MQTT Topic Architecture

DNSLab.IoT uses the following namespace:

```
tenant/{TenantId}/device/{DeviceId}/{Topic}
```

Example:

```
tenant/Project1/device/Device456/Notification
```

This allows multiple tenants and devices to use the same MQTT Hub while maintaining separate device namespaces.

---

# Examples

The library includes examples for common use cases.

## Basics

Basic WiFi connection and status:

```
examples/Basics/Basics.ino
```

## MQTT

Basic MQTT connection:

```
examples/MQTT/MQTT.ino
```

---

# Requirements

* Arduino IDE
* ESP32 or ESP8266 board
* WiFi network
* DNSLab MQTT Hub

Required Arduino libraries:

* PubSubClient

---

# License

MIT License

Copyright (c) DNSLab

See the `LICENSE` file for details.

---

# DNSLab IoT

DNSLab.IoT is part of the DNSLab IoT ecosystem.

The library is designed to make connecting embedded devices to the DNSLab MQTT Hub as simple as possible.

```
Device
   │
   │ WiFi
   ▼
DNSLab IoT Library
   │
   │ MQTT
   ▼
DNSLab MQTT Hub
   │
   ├── Tenant
   │
   ├── Device
   │
   └── Topics
```

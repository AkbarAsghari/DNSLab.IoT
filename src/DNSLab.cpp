#include "DNSLab.h"


// ==================================================
// CONSTRUCTOR
// ==================================================

DNSLab::DNSLab()
    : Wifi(),
      MQTT(),
      _mqttHost("mqtt.dnslab.ir"),
      _mqttPort(1883),
      _started(false),
      _debug(true)
{
}


// ==================================================
// BEGIN
// ==================================================

void DNSLab::begin()
{
    begin(
        "mqtt.dnslab.ir",
        1883
    );
}


// ==================================================
// BEGIN CUSTOM SERVER
// ==================================================

void DNSLab::begin(
    const char* host,
    uint16_t port
)
{
    if (!host)
    {
        return;
    }

    _mqttHost = host;
    _mqttPort = port;

    configureMQTT();

    _started = true;

    if (_debug)
    {
        Serial.println(
            "[DNSLab] Initialized"
        );

        Serial.print(
            "[DNSLab] MQTT Server: "
        );

        Serial.print(
            _mqttHost
        );

        Serial.print(":");

        Serial.println(
            _mqttPort
        );
    }
}


// ==================================================
// CONFIGURE MQTT
// ==================================================

void DNSLab::configureMQTT()
{
    MQTT.setServer(
        _mqttHost.c_str(),
        _mqttPort
    );
}


// ==================================================
// LOOP
// ==================================================

void DNSLab::loop()
{
    if (!_started)
    {
        return;
    }

    // -----------------------------
    // WiFi
    // -----------------------------

    Wifi.loop();

    // -----------------------------
    // MQTT
    // -----------------------------

    if (Wifi.connected())
    {
        MQTT.loop();
    }
}


// ==================================================
// DEBUG
// ==================================================

void DNSLab::setDebug(
    bool enabled
)
{
    _debug = enabled;

    Wifi.setDebug(
        enabled
    );

    MQTT.setDebug(
        enabled
    );
}
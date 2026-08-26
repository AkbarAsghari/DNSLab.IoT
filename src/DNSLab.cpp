#include "DNSLab.h"

#if defined(ESP32)

#include <WiFi.h>

DNSLab::DNSLab()
    : mqtt(),
      _wifiSSID(nullptr),
      _wifiPassword(nullptr),
      _mqttHost(nullptr),
      _mqttPort(1883),
      _lastWiFiAttempt(0),
      _wifiRetryInterval(5000) {
}

bool DNSLab::begin(
    const char* wifiSSID,
    const char* wifiPassword,
    const char* mqttHost,
    uint16_t mqttPort
) {
    _wifiSSID = wifiSSID;
    _wifiPassword = wifiPassword;
    _mqttHost = mqttHost;
    _mqttPort = mqttPort;

    WiFi.mode(WIFI_STA);

    connectWiFi();

    mqtt.begin(
        _mqttHost,
        _mqttPort
    );

    return true;
}

void DNSLab::loop() {

    // -------------------------
    // Wi-Fi Auto Reconnect
    // -------------------------

    if (WiFi.status() != WL_CONNECTED) {

        unsigned long now = millis();

        if (now - _lastWiFiAttempt >= _wifiRetryInterval) {
            connectWiFi();
        }

        return;
    }

    // -------------------------
    // MQTT
    // -------------------------

    mqtt.loop();
}

void DNSLab::connectWiFi() {

    _lastWiFiAttempt = millis();

    if (!_wifiSSID || !_wifiPassword) {
        return;
    }

    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    WiFi.begin(
        _wifiSSID,
        _wifiPassword
    );
}

bool DNSLab::connected() {
    return mqtt.connected();
}

bool DNSLab::wifiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void DNSLab::disconnect() {

    mqtt.disconnect();

    WiFi.disconnect(true);
}

#else

#error "DNSLab v0.1.0 currently supports ESP32 only."

#endif
#include "DNSLab.h"

#if defined(ESP32)

DNSLab::DNSLab()
    : mqtt(),
      _wifiSSID(nullptr),
      _wifiPassword(nullptr),
      _mqttHost(nullptr),
      _mqttPort(1883),
      _debug(false),
      _wifiStarted(false),
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

    if (!_wifiSSID || !_wifiPassword) {

        printDebug(
            "[DNSLab] Invalid WiFi credentials"
        );

        return false;
    }

    printDebug(
        "[DNSLab] Starting..."
    );

    WiFi.mode(WIFI_STA);

    connectWiFi();

    mqtt.begin(
        _mqttHost,
        _mqttPort
    );

    return true;
}

void DNSLab::loop() {

    // --------------------------------
    // WiFi
    // --------------------------------

    if (WiFi.status() != WL_CONNECTED) {

        unsigned long now = millis();

        if (
            !_wifiStarted ||
            now - _lastWiFiAttempt >= _wifiRetryInterval
        ) {

            connectWiFi();
        }

        return;
    }

    // --------------------------------
    // WiFi Connected
    // --------------------------------

    static bool wasConnected = false;

    if (!wasConnected) {

        wasConnected = true;

        printDebug(
            "[DNSLab] WiFi connected"
        );

        if (_debug) {

            Serial.print(
                "[DNSLab] IP: "
            );

            Serial.println(
                WiFi.localIP()
            );
        }
    }

    // --------------------------------
    // MQTT
    // --------------------------------

    mqtt.loop();
}

void DNSLab::connectWiFi() {

    if (!_wifiSSID || !_wifiPassword) {
        return;
    }

    // Already connected
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    // --------------------------------
    // IMPORTANT
    // Don't call WiFi.begin() while
    // ESP32 is already connecting.
    // --------------------------------

    if (_wifiStarted) {

        wl_status_t status = WiFi.status();

        if (
            status != WL_NO_SSID_AVAIL &&
            status != WL_CONNECT_FAILED &&
            status != WL_DISCONNECTED
        ) {

            return;
        }
    }

    _lastWiFiAttempt = millis();

    printDebug(
        "[DNSLab] Connecting to WiFi..."
    );

    WiFi.begin(
        _wifiSSID,
        _wifiPassword
    );

    _wifiStarted = true;
}

bool DNSLab::connected() {

    return
        WiFi.status() == WL_CONNECTED &&
        mqtt.connected();
}

bool DNSLab::wifiConnected() {

    return WiFi.status() == WL_CONNECTED;
}

void DNSLab::disconnect() {

    printDebug(
        "[DNSLab] Disconnecting..."
    );

    mqtt.disconnect();

    WiFi.disconnect(true);

    _wifiStarted = false;
}

void DNSLab::setDebug(bool enabled) {

    _debug = enabled;

    mqtt.setDebug(enabled);
}

void DNSLab::printDebug(
    const char* message
) {

    if (!_debug) {
        return;
    }

    Serial.println(message);
}

#else

#error "DNSLab currently supports ESP32 only."

#endif
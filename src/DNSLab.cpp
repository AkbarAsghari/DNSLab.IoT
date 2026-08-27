#include "DNSLab.h"

#if defined(ESP32)

DNSLab::DNSLab()
    : mqtt(),
      _wifiSSID(nullptr),
      _wifiPassword(nullptr),
      _wifiStarted(false),
      _lastWiFiAttempt(0),
      _wifiRetryInterval(5000)
{
}

bool DNSLab::begin(
    const char* wifiSSID,
    const char* wifiPassword,
    const char* tenantId,
    const char* deviceId
)
{
    if (!wifiSSID || !wifiPassword)
    {
        printDebug(
            "[DNSLab] Invalid WiFi credentials"
        );

        return false;
    }

    if (!tenantId || !deviceId)
    {
        printDebug(
            "[DNSLab] Invalid Tenant ID or Device ID"
        );

        return false;
    }

    _wifiSSID = wifiSSID;
    _wifiPassword = wifiPassword;

    // --------------------------------
    // Configure MQTT identity
    // --------------------------------

    mqtt.setTenantId(tenantId);
    mqtt.setDeviceId(deviceId);

    // --------------------------------
    // WiFi
    // --------------------------------

    WiFi.mode(WIFI_STA);

    connectWiFi();

    // --------------------------------
    // MQTT
    // --------------------------------

    mqtt.begin();

    return true;
}

void DNSLab::loop()
{
    wl_status_t wifiStatus =
        WiFi.status();

    // --------------------------------
    // WiFi disconnected
    // --------------------------------

    if (wifiStatus != WL_CONNECTED)
    {
        connectWiFi();

        return;
    }

    // --------------------------------
    // MQTT
    // --------------------------------

    mqtt.loop();
}

void DNSLab::connectWiFi()
{
    wl_status_t status =
        WiFi.status();

    // Already connected
    if (status == WL_CONNECTED)
    {
        return;
    }

    // --------------------------------
    // ESP32 is already trying to connect
    // --------------------------------

    if (
        status == WL_IDLE_STATUS ||
        status == WL_NO_SSID_AVAIL ||
        status == WL_SCAN_COMPLETED
    )
    {
        return;
    }

    unsigned long now =
        millis();

    if (
        _wifiStarted &&
        now - _lastWiFiAttempt <
        _wifiRetryInterval
    )
    {
        return;
    }

    _lastWiFiAttempt = now;

    printDebug(
        "[DNSLab] Connecting to WiFi..."
    );

    WiFi.begin(
        _wifiSSID,
        _wifiPassword
    );

    _wifiStarted = true;
}

bool DNSLab::connected()
{
    return
        wifiConnected() &&
        mqtt.connected();
}

bool DNSLab::wifiConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

void DNSLab::disconnect()
{
    mqtt.disconnect();

    WiFi.disconnect(false);

    _wifiStarted = false;

    printDebug(
        "[DNSLab] Disconnected"
    );
}

void DNSLab::setDebug(
    bool enabled
)
{
    mqtt.setDebug(enabled);
}

void DNSLab::printDebug(
    const char* message
)
{
    if (!mqtt.debugEnabled())
    {
        return;
    }

    Serial.println(message);
}

#else

#error "DNSLab currently supports ESP32 only."

#endif
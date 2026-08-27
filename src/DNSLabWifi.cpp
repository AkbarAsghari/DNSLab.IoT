#include "DNSLabWifi.h"


// ==================================================
// CONSTRUCTOR
// ==================================================

DNSLabWifi::DNSLabWifi()
    : _ssid(nullptr),
      _password(nullptr),
      _started(false),
      _connecting(false),
      _lastAttempt(0),
      _retryInterval(5000),
      _debug(true)
{
}


// ==================================================
// BEGIN
// ==================================================

bool DNSLabWifi::begin(
    const char* ssid,
    const char* password
)
{
    if (!ssid || !password)
    {
        debug(
            "[DNSLab] Invalid WiFi credentials"
        );

        return false;
    }

    _ssid = ssid;
    _password = password;

#if defined(ESP32) || defined(ESP8266)

    WiFi.mode(
        WIFI_STA
    );

#endif

    _started = true;

    connect();

    return true;
}


// ==================================================
// LOOP
// ==================================================

void DNSLabWifi::loop()
{
    if (!_started)
    {
        return;
    }

    if (connected())
    {
        _connecting = false;
        return;
    }

    unsigned long now =
        millis();

    if (
        now - _lastAttempt >=
        _retryInterval
    )
    {
        connect();
    }
}


// ==================================================
// CONNECT
// ==================================================

void DNSLabWifi::connect()
{
    if (!_ssid || !_password)
    {
        return;
    }

    if (connected())
    {
        return;
    }

    unsigned long now =
        millis();

    if (
        _connecting &&
        now - _lastAttempt < 1000
    )
    {
        return;
    }

    _lastAttempt = now;

    _connecting = true;

    debug(
        "[DNSLab] Connecting to WiFi..."
    );

    WiFi.begin(
        _ssid,
        _password
    );
}


// ==================================================
// CONNECTED
// ==================================================

bool DNSLabWifi::connected()
{
    return WiFi.status() ==
           WL_CONNECTED;
}


// ==================================================
// DISCONNECT
// ==================================================

void DNSLabWifi::disconnect()
{
    WiFi.disconnect();

    _connecting = false;

    debug(
        "[DNSLab] WiFi disconnected"
    );
}


// ==================================================
// STATUS
// ==================================================

wl_status_t DNSLabWifi::status()
{
    return WiFi.status();
}


// ==================================================
// LOCAL IP
// ==================================================

IPAddress DNSLabWifi::localIP()
{
    return WiFi.localIP();
}


// ==================================================
// SIGNAL
// ==================================================

int DNSLabWifi::signalStrength()
{
    if (!connected())
    {
        return 0;
    }

    return WiFi.RSSI();
}


// ==================================================
// DEBUG
// ==================================================

void DNSLabWifi::setDebug(
    bool enabled
)
{
    _debug = enabled;
}


bool DNSLabWifi::debugEnabled()
{
    return _debug;
}


void DNSLabWifi::debug(
    const char* message
)
{
    if (!_debug)
    {
        return;
    }

    Serial.println(
        message
    );
}
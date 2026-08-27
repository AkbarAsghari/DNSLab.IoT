#ifndef DNSLAB_WIFI_H
#define DNSLAB_WIFI_H

#include <Arduino.h>

#if defined(ESP32)

#include <WiFi.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>

#else

#error "DNSLab.IoT supports ESP32 and ESP8266 only."

#endif


class DNSLabWifi
{
public:

    DNSLabWifi();

    bool begin(
        const char* ssid,
        const char* password
    );

    void loop();

    bool connected();

    void disconnect();

    IPAddress localIP();

    int signalStrength();

    wl_status_t status();

    void setDebug(
        bool enabled
    );

    bool debugEnabled();

private:

    const char* _ssid;

    const char* _password;

    bool _started;

    bool _connecting;

    unsigned long _lastAttempt;

    unsigned long _retryInterval;

    bool _debug;

    void connect();

    void debug(
        const char* message
    );
};

#endif
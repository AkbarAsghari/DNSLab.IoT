#ifndef DNSLAB_H
#define DNSLAB_H

#include <Arduino.h>
#include <WiFi.h>
#include "DNSLabMQTT.h"

class DNSLab {
public:

    DNSLab();

    bool begin(
        const char* wifiSSID,
        const char* wifiPassword,
        const char* mqttHost = "dnslab.link",
        uint16_t mqttPort = 1883
    );

    void loop();

    bool connected();

    bool wifiConnected();

    void disconnect();

    void setDebug(bool enabled);

    DNSLabMQTT mqtt;

private:

    const char* _wifiSSID;
    const char* _wifiPassword;

    const char* _mqttHost;
    uint16_t _mqttPort;

    bool _debug;

    bool _wifiStarted;

    unsigned long _lastWiFiAttempt;
    unsigned long _wifiRetryInterval;

    void connectWiFi();

    void printDebug(const char* message);
};

#endif
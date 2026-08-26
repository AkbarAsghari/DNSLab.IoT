#ifndef DNSLAB_H
#define DNSLAB_H

#include <Arduino.h>
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

    DNSLabMQTT mqtt;

private:
    const char* _wifiSSID;
    const char* _wifiPassword;
    const char* _mqttHost;
    uint16_t _mqttPort;

    unsigned long _lastWiFiAttempt;
    unsigned long _wifiRetryInterval;

    void connectWiFi();
};

#endif
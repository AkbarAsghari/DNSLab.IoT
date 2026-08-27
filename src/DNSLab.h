#ifndef DNSLAB_H
#define DNSLAB_H

#include <Arduino.h>

#include "DNSLabWifi.h"
#include "DNSLabMQTT.h"

class DNSLab
{
public:

    DNSLab();

    // Default:
    // mqtt.dnslab.ir:1883
    void begin();

    // Custom MQTT server
    void begin(
        const char* host,
        uint16_t port
    );

    void loop();

    void setDebug(
        bool enabled
    );

    DNSLabWifi Wifi;
    DNSLabMQTT MQTT;

private:

    String _mqttHost;

    uint16_t _mqttPort;

    bool _started;

    bool _debug;

    void configureMQTT();
};

#endif
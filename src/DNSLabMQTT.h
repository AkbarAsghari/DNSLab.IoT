#ifndef DNSLAB_MQTT_H
#define DNSLAB_MQTT_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include <functional>

class DNSLabMQTT {
public:

    using MessageCallback = std::function<
        void(const char* topic, const char* message)
    >;

    DNSLabMQTT();

    void begin(
        const char* host,
        uint16_t port = 1883
    );

    void loop();

    bool connected();

    bool connect();

    void disconnect();

    bool publish(
        const char* topic,
        const char* message
    );

    bool publish(
        const char* topic,
        const String& message
    );

    bool subscribe(
        const char* topic
    );

    bool unsubscribe(
        const char* topic
    );

    void onMessage(
        MessageCallback callback
    );

    void setCredentials(
        const char* username,
        const char* password
    );

    void setClientId(
        const char* clientId
    );

private:

    WiFiClient _wifiClient;
    PubSubClient _mqttClient;

    const char* _host;
    uint16_t _port;

    const char* _username;
    const char* _password;

    const char* _clientId;

    MessageCallback _messageCallback;

    unsigned long _lastConnectAttempt;
    unsigned long _reconnectInterval;

    static DNSLabMQTT* _instance;

    static void mqttCallback(
        char* topic,
        byte* payload,
        unsigned int length
    );

    void handleMessage(
        char* topic,
        byte* payload,
        unsigned int length
    );
};

#endif
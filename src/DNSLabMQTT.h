#ifndef DNSLAB_MQTT_H
#define DNSLAB_MQTT_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include <functional>
#include <vector>

class DNSLabMQTT
{
public:

    using MessageCallback =
        std::function<
            void(
                const char* topic,
                const char* message
            )
        >;

    DNSLabMQTT();

    void begin();

    void loop();

    bool connected();

    bool connect();

    void disconnect();

    // --------------------------------
    // Publish
    // --------------------------------

    bool publish(
        const char* topic,
        const char* message
    );

    bool publish(
        const char* topic,
        const String& message
    );

    // --------------------------------
    // Subscribe
    // --------------------------------

    bool subscribe(
        const char* topic
    );

    bool unsubscribe(
        const char* topic
    );

    // --------------------------------
    // Callback
    // --------------------------------

    void onMessage(
        MessageCallback callback
    );

    // --------------------------------
    // Debug
    // --------------------------------

    void setDebug(
        bool enabled
    );

    bool debugEnabled();

    // --------------------------------
    // Identity
    // --------------------------------

    void setTenantId(
        const char* tenantId
    );

    void setDeviceId(
        const char* deviceId
    );

private:

    // --------------------------------
    // MQTT
    // --------------------------------

    WiFiClient _wifiClient;

    PubSubClient _mqttClient;

    const char* _host;

    uint16_t _port;

    // --------------------------------
    // Identity
    // --------------------------------

    const char* _tenantId;

    const char* _deviceId;

    String _clientId;

    // --------------------------------
    // Callback
    // --------------------------------

    MessageCallback _messageCallback;

    // --------------------------------
    // Debug
    // --------------------------------

    bool _debug;

    // --------------------------------
    // State
    // --------------------------------

    bool _mqttStarted;

    bool _subscriptionsRestored;

    unsigned long _lastConnectAttempt;

    unsigned long _reconnectInterval;

    // --------------------------------
    // Subscriptions
    // --------------------------------

    std::vector<String> _subscriptions;

    // --------------------------------
    // Singleton callback
    // --------------------------------

    static DNSLabMQTT* _instance;

    // --------------------------------
    // MQTT callback
    // --------------------------------

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

    // --------------------------------
    // Topic
    // --------------------------------

    String buildTopic(
        const char* topic
    );

    // --------------------------------
    // Subscription manager
    // --------------------------------

    void restoreSubscriptions();

    bool subscriptionExists(
        const char* topic
    );

    // --------------------------------
    // Client ID
    // --------------------------------

    void generateClientId();

    // --------------------------------
    // Debug
    // --------------------------------

    void debug(
        const char* message
    );
};

#endif
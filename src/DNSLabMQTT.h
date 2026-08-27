#ifndef DNSLAB_MQTT_H
#define DNSLAB_MQTT_H

#include <Arduino.h>

#include <vector>
#include <functional>

#if defined(ESP32)

#include <WiFi.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>

#else

#error "DNSLab.IoT supports ESP32 and ESP8266 only."

#endif

#include <PubSubClient.h>


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


    // ==================================================
    // SERVER
    // ==================================================

    void setServer(
        const char* host,
        uint16_t port
    );


    // ==================================================
    // BEGIN
    // ==================================================

    void begin(
        const char* tenantId,
        const char* deviceId
    );


    // ==================================================
    // MQTT
    // ==================================================

    void loop();

    bool connect();

    bool connected();

    void disconnect();


    // ==================================================
    // BUFFER SIZE
    // ==================================================

    void setBufferSize(
        uint16_t size
    );


    // ==================================================
    // PUBLISH
    // ==================================================

    bool publish(
        const char* topic,
        const char* message
    );

    bool publish(
        const char* topic,
        const String& message
    );


    // ==================================================
    // SUBSCRIBE
    // ==================================================

    bool subscribe(
        const char* topic
    );

    bool unsubscribe(
        const char* topic
    );


    // ==================================================
    // CALLBACK
    // ==================================================

    void onMessage(
        MessageCallback callback
    );


    // ==================================================
    // DEBUG
    // ==================================================

    void setDebug(
        bool enabled
    );

    bool debugEnabled();


private:

    #if defined(ESP32) || defined(ESP8266)

    WiFiClient _wifiClient;

    #endif

    PubSubClient _mqttClient;


    String _host;

    uint16_t _port;


    String _tenantId;

    String _deviceId;

    String _clientId;


    bool _started;

    bool _subscriptionsRestored;

    bool _debug;


    unsigned long _lastConnectAttempt;

    unsigned long _reconnectInterval;


    std::vector<String> _subscriptions;


    MessageCallback _messageCallback;


    static DNSLabMQTT* _instance;


    // ==================================================
    // INTERNAL
    // ==================================================

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


    String buildTopic(
        const char* topic
    );


    void restoreSubscriptions();


    bool subscriptionExists(
        const char* topic
    );


    void generateClientId();


    void debug(
        const char* message
    );
};

#endif
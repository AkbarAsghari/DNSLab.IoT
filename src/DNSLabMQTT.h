#ifndef DNSLAB_MQTT_H
#define DNSLAB_MQTT_H

#include <Arduino.h>

#include <vector>
#include <functional>


// ==================================================
// PLATFORM
// ==================================================

#if defined(ESP32)

#include <WiFi.h>
#include <WiFiClientSecure.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#else

#error "DNSLab.IoT supports ESP32 and ESP8266 only."

#endif


// ==================================================
// MQTT
// ==================================================

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
        const char* projectId,
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

    // ==================================================
    // SECURE WIFI CLIENT
    // ==================================================

    #if defined(ESP32) || defined(ESP8266)

    WiFiClientSecure _wifiClient;

    #endif


    // ==================================================
    // MQTT CLIENT
    // ==================================================

    PubSubClient _mqttClient;


    // ==================================================
    // CONNECTION
    // ==================================================

    String _host;

    uint16_t _port;


    // ==================================================
    // DEVICE
    // ==================================================

    String _projectId;

    String _deviceId;

    String _clientId;


    // ==================================================
    // STATE
    // ==================================================

    bool _started;

    bool _subscriptionsRestored;

    bool _debug;


    // ==================================================
    // RECONNECT
    // ==================================================

    unsigned long _lastConnectAttempt;

    unsigned long _reconnectInterval;


    // ==================================================
    // SUBSCRIPTIONS
    // ==================================================

    std::vector<String> _subscriptions;


    // ==================================================
    // CALLBACK
    // ==================================================

    MessageCallback _messageCallback;


    // ==================================================
    // SINGLETON INSTANCE
    // ==================================================

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
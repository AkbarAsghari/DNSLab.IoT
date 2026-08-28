#include "DNSLabMQTT.h"
#include "DNSLabCA.h"

DNSLabMQTT*
DNSLabMQTT::_instance = nullptr;


// ==================================================
// CONSTRUCTOR
// ==================================================

DNSLabMQTT::DNSLabMQTT()
#if defined(ESP32) || defined(ESP8266)
    : _wifiClient(),
      _mqttClient(_wifiClient),
#else
    : _mqttClient(),
#endif
      _host("mqtt.dnslab.ir"),
      _port(8883),
      _projectId(),
      _deviceId(),
      _clientId(),
      _started(false),
      _subscriptionsRestored(false),
      _debug(true),
      _lastConnectAttempt(0),
      _reconnectInterval(5000),
      _subscriptions(),
      _messageCallback(nullptr)
{
    _instance = this;

#if defined(ESP32) || defined(ESP8266)

    _wifiClient.setCACert(
        DNSLAB_ROOT_CA
    );

#endif

    _mqttClient.setCallback(
        DNSLabMQTT::mqttCallback
    );
}


// ==================================================
// SERVER
// ==================================================

void DNSLabMQTT::setServer(
    const char* host,
    uint16_t port
)
{
    if (!host)
    {
        return;
    }

    _host = host;
    _port = port;

    _mqttClient.setServer(
        _host.c_str(),
        _port
    );
}


// ==================================================
// BEGIN
// ==================================================

void DNSLabMQTT::begin(
    const char* projectId,
    const char* deviceId
)
{
    if (!projectId || !deviceId)
    {
        debug(
            "[DNSLab] Invalid Project or Device ID"
        );

        return;
    }

    _projectId = projectId;
    _deviceId = deviceId;

    generateClientId();

    _mqttClient.setServer(
        _host.c_str(),
        _port
    );

    _started = true;

    debug(
        "[DNSLab] MQTT initialized"
    );

    if (_debug)
    {
        Serial.print(
            "[DNSLab] Project: "
        );

        Serial.println(
            _projectId
        );

        Serial.print(
            "[DNSLab] Device: "
        );

        Serial.println(
            _deviceId
        );

        Serial.print(
            "[DNSLab] Client ID: "
        );

        Serial.println(
            _clientId
        );

        Serial.print(
            "[DNSLab] MQTT Username: "
        );

        Serial.println(
            _projectId
        );
    }
}


// ==================================================
// LOOP
// ==================================================

void DNSLabMQTT::loop()
{
    if (!_started)
    {
        return;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        _subscriptionsRestored = false;

        return;
    }


    // -----------------------------
    // Reconnect
    // -----------------------------

    if (!_mqttClient.connected())
    {
        _subscriptionsRestored = false;

        unsigned long now =
            millis();

        if (
            now - _lastConnectAttempt >=
            _reconnectInterval
        )
        {
            connect();
        }

        return;
    }


    // -----------------------------
    // MQTT loop
    // -----------------------------

    _mqttClient.loop();
}


// ==================================================
// CONNECT
// ==================================================

bool DNSLabMQTT::connect()
{
    if (!_started)
    {
        return false;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        return false;
    }

    if (_mqttClient.connected())
    {
        return true;
    }

    _lastConnectAttempt =
        millis();

    debug(
        "[DNSLab] Connecting to MQTT..."
    );


    // ==================================================
    // MQTT CREDENTIALS
    //
    // Username = Project ID
    // Password = Empty
    // ==================================================

    bool result =
        _mqttClient.connect(
            _clientId.c_str(),
            _projectId.c_str(),
            ""
        );


    if (result)
    {
        debug(
            "[DNSLab] MQTT connected"
        );

        if (_debug)
        {
            Serial.print(
                "[DNSLab] MQTT Username: "
            );

            Serial.println(
                _projectId
            );
        }

        restoreSubscriptions();
    }
    else
    {
        if (_debug)
        {
            Serial.print(
                "[DNSLab] MQTT connection failed. State: "
            );

            Serial.println(
                _mqttClient.state()
            );
        }
    }

    return result;
}


// ==================================================
// CONNECTED
// ==================================================

bool DNSLabMQTT::connected()
{
    return _mqttClient.connected();
}


// ==================================================
// DISCONNECT
// ==================================================

void DNSLabMQTT::disconnect()
{
    _mqttClient.disconnect();

    _subscriptionsRestored =
        false;

    debug(
        "[DNSLab] MQTT disconnected"
    );
}


// ==================================================
// BUFFER SIZE
// ==================================================

void DNSLabMQTT::setBufferSize(
    uint16_t size
)
{
    if (size < 128)
    {
        size = 128;
    }

    bool result =
        _mqttClient.setBufferSize(
            size
        );

    if (_debug)
    {
        Serial.print(
            "[DNSLab] MQTT buffer size: "
        );

        Serial.print(
            size
        );

        Serial.print(
            " bytes -> "
        );

        Serial.println(
            result
                ? "OK"
                : "FAILED"
        );
    }
}


// ==================================================
// CLIENT ID
// ==================================================

void DNSLabMQTT::generateClientId()
{

#if defined(ESP32)

    uint64_t chipId =
        ESP.getEfuseMac();

    uint32_t low =
        (uint32_t)(
            chipId &
            0xFFFFFFFF
        );

    char buffer[64];

    snprintf(
        buffer,
        sizeof(buffer),
        "DNSLab-%08X",
        low
    );

    _clientId =
        buffer;


#elif defined(ESP8266)

    char buffer[64];

    snprintf(
        buffer,
        sizeof(buffer),
        "DNSLab-%08X",
        ESP.getChipId()
    );

    _clientId =
        buffer;


#else

    _clientId =
        "DNSLab-Client";

#endif

}


// ==================================================
// BUILD TOPIC
// ==================================================

String DNSLabMQTT::buildTopic(
    const char* topic
)
{
    if (!topic)
    {
        return String();
    }

    if (
        _projectId.length() == 0 ||
        _deviceId.length() == 0
    )
    {
        return String(topic);
    }


    String finalTopic;

    finalTopic +=
        "project/";

    finalTopic +=
        _projectId;

    finalTopic +=
        "/device/";

    finalTopic +=
        _deviceId;

    finalTopic +=
        "/";

    finalTopic +=
        topic;


    return finalTopic;
}


// ==================================================
// PUBLISH
// ==================================================

bool DNSLabMQTT::publish(
    const char* topic,
    const char* message
)
{
    if (!topic || !message)
    {
        return false;
    }

    if (!_mqttClient.connected())
    {
        return false;
    }


    String finalTopic =
        buildTopic(topic);


    if (_debug)
    {
        Serial.print(
            "[DNSLab] Publish: "
        );

        Serial.println(
            finalTopic
        );
    }


    return _mqttClient.publish(
        finalTopic.c_str(),
        message
    );
}


// ==================================================
// PUBLISH STRING
// ==================================================

bool DNSLabMQTT::publish(
    const char* topic,
    const String& message
)
{
    return publish(
        topic,
        message.c_str()
    );
}


// ==================================================
// SUBSCRIBE
// ==================================================

bool DNSLabMQTT::subscribe(
    const char* topic
)
{
    if (!topic)
    {
        return false;
    }


    // -----------------------------
    // Save subscription
    // -----------------------------

    if (!subscriptionExists(topic))
    {
        _subscriptions.push_back(
            String(topic)
        );

        if (_debug)
        {
            Serial.print(
                "[DNSLab] Subscription registered: "
            );

            Serial.println(
                topic
            );
        }
    }


    // -----------------------------
    // MQTT not connected
    // -----------------------------

    if (!_mqttClient.connected())
    {
        return true;
    }


    // -----------------------------
    // Subscribe now
    // -----------------------------

    String finalTopic =
        buildTopic(topic);


    bool result =
        _mqttClient.subscribe(
            finalTopic.c_str()
        );


    if (_debug)
    {
        Serial.print(
            "[DNSLab] Subscribe: "
        );

        Serial.print(
            finalTopic
        );

        Serial.print(
            " -> "
        );

        Serial.println(
            result
                ? "OK"
                : "FAILED"
        );
    }


    return result;
}


// ==================================================
// UNSUBSCRIBE
// ==================================================

bool DNSLabMQTT::unsubscribe(
    const char* topic
)
{
    if (!topic)
    {
        return false;
    }


    // -----------------------------
    // Remove local subscription
    // -----------------------------

    for (
        auto it =
            _subscriptions.begin();

        it !=
            _subscriptions.end();

        ++it
    )
    {
        if (*it == topic)
        {
            _subscriptions.erase(
                it
            );

            break;
        }
    }


    // -----------------------------
    // MQTT
    // -----------------------------

    if (!_mqttClient.connected())
    {
        return true;
    }


    String finalTopic =
        buildTopic(topic);


    return _mqttClient.unsubscribe(
        finalTopic.c_str()
    );
}


// ==================================================
// SUBSCRIPTION EXISTS
// ==================================================

bool DNSLabMQTT::subscriptionExists(
    const char* topic
)
{
    for (
        const String& item :
        _subscriptions
    )
    {
        if (item == topic)
        {
            return true;
        }
    }

    return false;
}


// ==================================================
// RESTORE SUBSCRIPTIONS
// ==================================================

void DNSLabMQTT::restoreSubscriptions()
{
    if (!_mqttClient.connected())
    {
        return;
    }


    bool allSuccessful =
        true;


    for (
        const String& topic :
        _subscriptions
    )
    {
        String finalTopic =
            buildTopic(
                topic.c_str()
            );


        bool result =
            _mqttClient.subscribe(
                finalTopic.c_str()
            );


        if (_debug)
        {
            Serial.print(
                "[DNSLab] Subscribed: "
            );

            Serial.print(
                finalTopic
            );

            Serial.print(
                " -> "
            );

            Serial.println(
                result
                    ? "OK"
                    : "FAILED"
            );
        }


        if (!result)
        {
            allSuccessful =
                false;
        }
    }


    _subscriptionsRestored =
        allSuccessful;
}


// ==================================================
// CALLBACK
// ==================================================

void DNSLabMQTT::onMessage(
    MessageCallback callback
)
{
    _messageCallback =
        callback;
}


// ==================================================
// MQTT CALLBACK
// ==================================================

void DNSLabMQTT::mqttCallback(
    char* topic,
    byte* payload,
    unsigned int length
)
{
    if (_instance)
    {
        _instance->handleMessage(
            topic,
            payload,
            length
        );
    }
}


// ==================================================
// HANDLE MESSAGE
// ==================================================

void DNSLabMQTT::handleMessage(
    char* topic,
    byte* payload,
    unsigned int length
)
{
    if (!_messageCallback)
    {
        return;
    }


    // ==================================================
    // Find Local Subscription
    // ==================================================

    String fullTopic = topic;

    String localTopic;


    for (
        const String& subscription :
        _subscriptions
    )
    {
        String subscribedTopic =
            buildTopic(
                subscription.c_str()
            );


        if (fullTopic == subscribedTopic)
        {
            localTopic = subscription;

            break;
        }
    }


    // ==================================================
    // If Topic Was Not Found
    // ==================================================

    if (localTopic.length() == 0)
    {
        return;
    }


    // ==================================================
    // Build Message
    // ==================================================

    String message;

    message.reserve(
        length + 1
    );


    for (
        unsigned int i = 0;
        i < length;
        i++
    )
    {
        message +=
            (char)payload[i];
    }


    // ==================================================
    // Callback
    // ==================================================

    _messageCallback(
        localTopic.c_str(),
        message.c_str()
    );
}


// ==================================================
// DEBUG
// ==================================================

void DNSLabMQTT::setDebug(
    bool enabled
)
{
    _debug =
        enabled;
}


bool DNSLabMQTT::debugEnabled()
{
    return _debug;
}


void DNSLabMQTT::debug(
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
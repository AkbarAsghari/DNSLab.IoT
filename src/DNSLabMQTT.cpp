#include "DNSLabMQTT.h"

#if defined(ESP32)

DNSLabMQTT* DNSLabMQTT::_instance = nullptr;

DNSLabMQTT::DNSLabMQTT()
    : _wifiClient(),
      _mqttClient(_wifiClient),
      _host("dnslab.link"),
      _port(1883),
      _tenantId(nullptr),
      _deviceId(nullptr),
      _clientId(),
      _messageCallback(nullptr),
      _debug(true),
      _mqttStarted(false),
      _subscriptionsRestored(false),
      _lastConnectAttempt(0),
      _reconnectInterval(5000),
      _subscriptions()
{
    _instance = this;

    _mqttClient.setCallback(
        DNSLabMQTT::mqttCallback
    );
}


// ==================================================
// BEGIN
// ==================================================

void DNSLabMQTT::begin()
{
    _mqttClient.setServer(
        _host,
        _port
    );

    generateClientId();

    _mqttStarted = true;

    debug(
        "[DNSLab] MQTT initialized"
    );
}


// ==================================================
// LOOP
// ==================================================

void DNSLabMQTT::loop()
{
    if (!_mqttStarted)
    {
        return;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    // --------------------------------
    // MQTT disconnected
    // --------------------------------

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

    // --------------------------------
    // MQTT connected
    // --------------------------------

    _mqttClient.loop();
}


// ==================================================
// CONNECTED
// ==================================================

bool DNSLabMQTT::connected()
{
    return _mqttClient.connected();
}


// ==================================================
// CONNECT
// ==================================================

bool DNSLabMQTT::connect()
{
    if (!_mqttStarted)
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

    bool result =
        _mqttClient.connect(
            _clientId.c_str()
        );

    if (result)
    {
        debug(
            "[DNSLab] MQTT connected"
        );

        // --------------------------------
        // Restore subscriptions
        // --------------------------------

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
// TENANT ID
// ==================================================

void DNSLabMQTT::setTenantId(
    const char* tenantId
)
{
    _tenantId = tenantId;

    if (_debug)
    {
        Serial.print(
            "[DNSLab] Tenant: "
        );

        Serial.println(
            _tenantId
        );
    }
}


// ==================================================
// DEVICE ID
// ==================================================

void DNSLabMQTT::setDeviceId(
    const char* deviceId
)
{
    _deviceId = deviceId;

    generateClientId();

    if (_debug)
    {
        Serial.print(
            "[DNSLab] Device: "
        );

        Serial.println(
            _deviceId
        );
    }
}


// ==================================================
// CLIENT ID
// ==================================================

void DNSLabMQTT::generateClientId()
{
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

    _clientId = buffer;
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

    String finalTopic;

    // --------------------------------
    // SaaS namespace
    // --------------------------------

    if (
        _tenantId &&
        _deviceId
    )
    {
        finalTopic += "tenant/";

        finalTopic += _tenantId;

        finalTopic += "/device/";

        finalTopic += _deviceId;

        finalTopic += "/";

        finalTopic += topic;

        return finalTopic;
    }

    // --------------------------------
    // Fallback
    // --------------------------------

    return String(topic);
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

    // --------------------------------
    // Save subscription
    // --------------------------------

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

            Serial.println(topic);
        }
    }

    // --------------------------------
    // MQTT not connected yet
    //
    // Do not fail.
    // It will be restored automatically.
    // --------------------------------

    if (!_mqttClient.connected())
    {
        return true;
    }

    // --------------------------------
    // Subscribe now
    // --------------------------------

    String finalTopic =
        buildTopic(topic);

    bool result =
        _mqttClient.subscribe(
            finalTopic.c_str()
        );

    if (_debug)
    {
        if (result)
        {
            Serial.print(
                "[DNSLab] Subscribed: "
            );

            Serial.println(
                finalTopic
            );
        }
        else
        {
            Serial.print(
                "[DNSLab] Subscribe failed: "
            );

            Serial.println(
                finalTopic
            );
        }
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

    String finalTopic =
        buildTopic(topic);

    // --------------------------------
    // Remove from local list
    // --------------------------------

    for (
        auto it = _subscriptions.begin();
        it != _subscriptions.end();
        ++it
    )
    {
        if (*it == topic)
        {
            _subscriptions.erase(it);

            break;
        }
    }

    // --------------------------------
    // MQTT unsubscribe
    // --------------------------------

    if (!_mqttClient.connected())
    {
        return true;
    }

    bool result =
        _mqttClient.unsubscribe(
            finalTopic.c_str()
        );

    if (_debug)
    {
        Serial.print(
            "[DNSLab] Unsubscribed: "
        );

        Serial.println(
            finalTopic
        );
    }

    return result;
}


// ==================================================
// SUBSCRIPTION EXISTS
// ==================================================

bool DNSLabMQTT::subscriptionExists(
    const char* topic
)
{
    for (
        const String& subscription :
        _subscriptions
    )
    {
        if (subscription == topic)
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

    if (_subscriptionsRestored)
    {
        return;
    }

    debug(
        "[DNSLab] Restoring subscriptions..."
    );

    bool allSuccessful = true;

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
            if (result)
            {
                Serial.print(
                    "[DNSLab] Subscribed: "
                );

                Serial.println(
                    finalTopic
                );
            }
            else
            {
                Serial.print(
                    "[DNSLab] Subscribe failed: "
                );

                Serial.println(
                    finalTopic
                );

                allSuccessful = false;
            }
        }
    }

    _subscriptionsRestored =
        allSuccessful;

    if (_subscriptionsRestored)
    {
        debug(
            "[DNSLab] All subscriptions restored"
        );
    }
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

    String message;

    message.reserve(length);

    for (
        unsigned int i = 0;
        i < length;
        i++
    )
    {
        message +=
            (char)payload[i];
    }

    _messageCallback(
        topic,
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
    _debug = enabled;
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

    Serial.println(message);
}

#else

#error "DNSLab currently supports ESP32 only."

#endif
#include "DNSLabMQTT.h"

#if defined(ESP32)

#include <WiFi.h>

DNSLabMQTT* DNSLabMQTT::_instance = nullptr;

DNSLabMQTT::DNSLabMQTT()
    : _wifiClient(),
      _mqttClient(_wifiClient),
      _host(nullptr),
      _port(1883),
      _username(nullptr),
      _password(nullptr),
      _clientId(nullptr),
      _messageCallback(nullptr),
      _lastConnectAttempt(0),
      _reconnectInterval(5000) {

    _instance = this;

    _mqttClient.setCallback(
        DNSLabMQTT::mqttCallback
    );
}

void DNSLabMQTT::begin(
    const char* host,
    uint16_t port
) {

    _host = host;
    _port = port;

    _mqttClient.setServer(
        _host,
        _port
    );
}

void DNSLabMQTT::loop() {

    if (WiFi.status() != WL_CONNECTED) {
        return;
    }

    if (!_mqttClient.connected()) {

        unsigned long now = millis();

        if (now - _lastConnectAttempt >= _reconnectInterval) {
            connect();
        }

        return;
    }

    _mqttClient.loop();
}

bool DNSLabMQTT::connected() {

    return _mqttClient.connected();
}

bool DNSLabMQTT::connect() {

    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    if (!_host) {
        return false;
    }

    if (_mqttClient.connected()) {
        return true;
    }

    _lastConnectAttempt = millis();

    bool result = false;

    // -------------------------
    // Username / Password
    // -------------------------

    if (_username && _password) {

        if (_clientId) {

            result = _mqttClient.connect(
                _clientId,
                _username,
                _password
            );

        } else {

            result = _mqttClient.connect(
                "DNSLab-ESP32",
                _username,
                _password
            );
        }

    } else {

        // -------------------------
        // Anonymous Connection
        // -------------------------

        if (_clientId) {

            result = _mqttClient.connect(
                _clientId
            );

        } else {

            result = _mqttClient.connect(
                "DNSLab-ESP32"
            );
        }
    }

    return result;
}

void DNSLabMQTT::disconnect() {

    _mqttClient.disconnect();
}

bool DNSLabMQTT::publish(
    const char* topic,
    const char* message
) {

    if (!_mqttClient.connected()) {
        return false;
    }

    if (!topic || !message) {
        return false;
    }

    return _mqttClient.publish(
        topic,
        message
    );
}

bool DNSLabMQTT::publish(
    const char* topic,
    const String& message
) {

    return publish(
        topic,
        message.c_str()
    );
}

bool DNSLabMQTT::subscribe(
    const char* topic
) {

    if (!_mqttClient.connected()) {
        return false;
    }

    if (!topic) {
        return false;
    }

    return _mqttClient.subscribe(
        topic
    );
}

bool DNSLabMQTT::unsubscribe(
    const char* topic
) {

    if (!_mqttClient.connected()) {
        return false;
    }

    if (!topic) {
        return false;
    }

    return _mqttClient.unsubscribe(
        topic
    );
}

void DNSLabMQTT::onMessage(
    MessageCallback callback
) {

    _messageCallback = callback;
}

void DNSLabMQTT::setCredentials(
    const char* username,
    const char* password
) {

    _username = username;
    _password = password;
}

void DNSLabMQTT::setClientId(
    const char* clientId
) {

    _clientId = clientId;
}

void DNSLabMQTT::mqttCallback(
    char* topic,
    byte* payload,
    unsigned int length
) {

    if (_instance) {

        _instance->handleMessage(
            topic,
            payload,
            length
        );
    }
}

void DNSLabMQTT::handleMessage(
    char* topic,
    byte* payload,
    unsigned int length
) {

    if (!_messageCallback) {
        return;
    }

    String message;

    message.reserve(length);

    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    _messageCallback(
        topic,
        message.c_str()
    );
}

#else

#error "DNSLab v0.1.0 currently supports ESP32 only."

#endif
#include <DNSLab.h>

DNSLab dns;

// -------------------------
// WiFi
// -------------------------

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// -------------------------
// DNSLab MQTT
// -------------------------

const char* MQTT_HOST = "dnslab.link";
const uint16_t MQTT_PORT = 1883;

// -------------------------
// Topics
// -------------------------

const char* SUBSCRIBE_TOPIC = "Notification";
const char* PUBLISH_TOPIC = "Notification";

unsigned long lastPublish = 0;

void setup() {

    Serial.begin(115200);

    Serial.println();
    Serial.println("======================");
    Serial.println("DNSLab IoT");
    Serial.println("MQTT Example");
    Serial.println("======================");

    // -------------------------
    // DNSLab
    // -------------------------

    dns.begin(
        WIFI_SSID,
        WIFI_PASSWORD,
        MQTT_HOST,
        MQTT_PORT
    );

    // -------------------------
    // MQTT Callback
    // -------------------------

    dns.mqtt.onMessage(
        [](const char* topic, const char* message) {

            Serial.println();
            Serial.println("----- MQTT Message -----");

            Serial.print("Topic: ");
            Serial.println(topic);

            Serial.print("Message: ");
            Serial.println(message);

            Serial.println("------------------------");
        }
    );
}

void loop() {

    dns.loop();

    // -------------------------
    // Subscribe
    // -------------------------

    static bool subscribed = false;

    if (dns.mqtt.connected() && !subscribed) {

        if (dns.mqtt.subscribe(SUBSCRIBE_TOPIC)) {

            subscribed = true;

            Serial.print("Subscribed to: ");
            Serial.println(SUBSCRIBE_TOPIC);
        }
    }

    // -------------------------
    // Publish every 10 seconds
    // -------------------------

    if (dns.mqtt.connected()) {

        if (millis() - lastPublish >= 10000) {

            lastPublish = millis();

            if (dns.mqtt.publish(
                    PUBLISH_TOPIC,
                    "Hello from DNSLab ESP32!"
                )) {

                Serial.println(
                    "MQTT message published."
                );
            }
        }
    } else {

        // Allow Subscribe again
        // after MQTT reconnect.

        subscribed = false;
    }
}
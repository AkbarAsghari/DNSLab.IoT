#include <DNSLab.h>

DNSLab dnslab;


// ==================================================
// WiFi
// ==================================================

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";


// ==================================================
// MQTT
// ==================================================

const char* PROJECT_ID = "Project1";
const char* DEVICE_ID = "Device456";


// ==================================================
// Setup
// ==================================================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println("DNSLab IoT - MQTT Example");
    Serial.println("==============================");


    // ==================================================
    // DNSLab
    //
    // Default MQTT Server:
    // mqtt.dnslab.ir:8883
    // ==================================================

    dnslab.begin();


    // ==================================================
    // WiFi
    // ==================================================

    dnslab.Wifi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );


    // ==================================================
    // MQTT
    // ==================================================

    dnslab.MQTT.begin(
        PROJECT_ID,
        DEVICE_ID
    );


    // ==================================================
    // MQTT Buffer
    // ==================================================

    dnslab.MQTT.setBufferSize(
        2048
    );
}


// ==================================================
// Loop
// ==================================================

void loop()
{
    dnslab.loop();


    // ==================================================
    // Connection Status
    // ==================================================

    static unsigned long lastPrint = 0;

    if (
        millis() - lastPrint >= 3000
    )
    {
        lastPrint = millis();


        // -----------------------------
        // WiFi
        // -----------------------------

        Serial.print(
            "[DNSLab] WiFi: "
        );

        if (
            dnslab.Wifi.connected()
        )
        {
            Serial.println(
                "Connected"
            );
        }
        else
        {
            Serial.println(
                "Disconnected"
            );
        }


        // -----------------------------
        // MQTT
        // -----------------------------

        Serial.print(
            "[DNSLab] MQTT: "
        );

        if (
            dnslab.MQTT.connected()
        )
        {
            Serial.println(
                "Connected"
            );
        }
        else
        {
            Serial.println(
                "Disconnected"
            );
        }


        Serial.println();
    }
}
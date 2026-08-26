#include <DNSLab.h>

DNSLab dns;

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

void setup() {

    Serial.begin(115200);

    Serial.println();
    Serial.println("DNSLab Basic Example");

    dns.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );
}

void loop() {

    dns.loop();

    static unsigned long lastPrint = 0;

    if (millis() - lastPrint >= 3000) {

        lastPrint = millis();

        Serial.print("WiFi: ");

        if (dns.wifiConnected()) {
            Serial.println("Connected");
        } else {
            Serial.println("Disconnected");
        }
    }
}
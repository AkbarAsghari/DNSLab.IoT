#include <DNSLab.h>

DNSLab dnslab;

void setup()
{
    Serial.begin(115200);

    dnslab.begin();

    dnslab.Wifi.begin(
        "YOUR_WIFI_NAME",
        "YOUR_WIFI_PASSWORD"
    );

    dnslab.MQTT.begin(
        "Project1",
        "Device456"
    );
}

void loop()
{
    dnslab.loop();
}
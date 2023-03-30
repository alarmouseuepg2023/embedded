#include <ESP32Pinout.h>
#include <WiFiConnection.h>

#define DEVICE_ESPTOUCHv2_PASSWORD "2893701982730182"

#define PIN_LED_WIFI_FEEDBACK D21

bool wifi_configurated = false;
WiFiConnection wifi_connection = WiFiConnection(DEVICE_ESPTOUCHv2_PASSWORD);

void setup()
{
  Serial.begin(115200);

  pinMode(PIN_LED_WIFI_FEEDBACK, OUTPUT);
}

void loop()
{
  if (!wifi_configurated)
    wifi_configurated = wifi_connection.waitSmartConfig();

  if (wifi_configurated && !wifi_connection.connected())
    wifi_connection.reconnect();

  digitalWrite(PIN_LED_WIFI_FEEDBACK, wifi_connection.connected());

  delay(50);
}

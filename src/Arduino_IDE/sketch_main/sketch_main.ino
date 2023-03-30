#include <ESP32Pinout.h>
#include <WiFiConnection.h>

#define PIN_LED_WIFI_FEEDBACK D21

WiFiCredentials wifi_credentials = { false, "", "" };
WiFiConnection wifi_connection = WiFiConnection(&wifi_credentials);

void setup()
{
  Serial.begin(115200);

  pinMode(PIN_LED_WIFI_FEEDBACK, OUTPUT);
}

void loop()
{
  if (wifi_credentials.configured && !wifi_connection.connected())
    wifi_connection.reconnect();

  digitalWrite(PIN_LED_WIFI_FEEDBACK, wifi_connection.connected());
  delay(50);
}

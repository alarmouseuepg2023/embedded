#include <ESP32Pinout.h>
#include <WiFiConnection.h>

#define PIN_LED_WIFI_FEEDBACK D21

char wifi_ssid[32] = "";
char wifi_password[32] = "";

WiFiConnection wiFiConnection = WiFiConnection(wifi_ssid, wifi_password);

void setup()
{
  Serial.begin(115200);

  pinMode(PIN_LED_WIFI_FEEDBACK, OUTPUT);
}

void loop()
{
  if (!wiFiConnection.connected()) wiFiConnection.reconnect();

  digitalWrite(PIN_LED_WIFI_FEEDBACK, wiFiConnection.connected());

  delay(50);
}

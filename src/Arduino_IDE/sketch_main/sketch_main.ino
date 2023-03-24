/*
  PLACA: ESP32-WROOM-DA Module
*/
#include <WiFi.h>

#define WIFI_SSID "StatusNet 20 303"
#define WIFI_PASSWORD "98706993"


BluetoothSerial SerialBT;

void setup() {
 Serial.begin(115200);
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
 while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");

}

void loop() {
  delay(2000);
          
}

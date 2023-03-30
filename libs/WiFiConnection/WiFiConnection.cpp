#include "WiFiConnection.h"


WiFiConnection::WiFiConnection(WiFiCredentials* wifiCredentials) {
	this->wifiCredentials = wifiCredentials;
}

void WiFiConnection::connect() {
  WiFi.begin(this->wifiCredentials->ssid, this->wifiCredentials->password);
}

bool WiFiConnection::connected() {
	return WiFi.status() == WL_CONNECTED;
}

IPAddress WiFiConnection::getLocalIP() {
	return WiFi.localIP();
}

String WiFiConnection::getMacAddress() {
	return WiFi.macAddress();
}

void WiFiConnection::printStatus() {
	static unsigned long lmillis = millis();
	
	if ((millis() - lmillis) >= PRINT_STATUS_IN_MS) {
		Serial.println("=====================================");
		Serial.print("Status da conexão wi-fi: ");
		Serial.println(this->connected()? "CONECTADO" : "DESCONECTADO");
		Serial.println("=====================================");
		
		lmillis = millis();
	}
}
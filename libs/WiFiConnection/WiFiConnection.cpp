#include "WiFiConnection.h"


WiFiConnection::WiFiConnection(char* espTouchPassword) {
	this->espTouchPassword = espTouchPassword;
}

void WiFiConnection::connect() {
  WiFi.begin(this->ssid, this->password);
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

bool WiFiConnection::waitSmartConfig() {
	static bool start_config = false;

	if (!start_config) {
		start_config = true;
		WiFi.mode(WIFI_AP_STA);
		WiFi.beginSmartConfig(SC_TYPE_ESPTOUCH_V2, this->espTouchPassword);

		free(this->ssid);
		free(this->password);
	}

	if (WiFi.smartConfigDone()) {
		start_config = false;

		String _ssid = WiFi.SSID();
		this->ssid = (char*) malloc(sizeof(char) * _ssid.length());
		strcpy(this->ssid, _ssid.c_str());

		String _password = WiFi.psk();
		this->password = (char*) malloc(sizeof(char) * _password.length());
		strcpy(this->password, _password.c_str());

		return true;
	}

	return false;
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
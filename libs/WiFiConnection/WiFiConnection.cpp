#include "WiFiConnection.h"


WiFiConnection::WiFiConnection(char* espTouchPassword, void (*cb)(WiFiEvent_t)) {
	this->espTouchPassword = espTouchPassword;
	this->smartConfigStatus = SmartConfigStatus::WAITING;
	WiFi.onEvent(cb);
}

void WiFiConnection::setup() {
	this->preferences.begin("alarmouse");
	this->preferences.clear();

  if (this->hasWifiCredentialsSaved()) {
		String _ssid = this->preferences.getString("ssid");
		this->ssid = (char*) malloc(sizeof(char) * _ssid.length());
		strcpy(this->ssid, _ssid.c_str());
		
		String _password = this->preferences.getString("password");
		this->password = (char*) malloc(sizeof(char) * _password.length());
		strcpy(this->password, _password.c_str());
  }
}

void WiFiConnection::connect() {
  WiFi.begin(this->ssid, this->password);
}

bool WiFiConnection::hasWifiCredentialsSaved() {
  return this->preferences.getUInt("configurated") == 1;
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

void WiFiConnection::resetSmartConfig() {
	this->preferences.putUInt("configurated", 0);
	this->preferences.remove("ssid");
	this->preferences.remove("password");
	this->preferences.end();
	ESP.restart();
}

bool WiFiConnection::waitSmartConfig() {
	if (this->smartConfigStatus == SmartConfigStatus::WAITING) {
		this->smartConfigStatus = SmartConfigStatus::STARTED;

		WiFi.mode(WIFI_AP_STA);
		WiFi.beginSmartConfig(SC_TYPE_ESPTOUCH_V2, this->espTouchPassword);

		free(this->ssid);
		free(this->password);
	}

	if (WiFi.smartConfigDone()) {
		this->smartConfigStatus = SmartConfigStatus::FINISHED;

		String _ssid = WiFi.SSID();
		this->ssid = (char*) malloc(sizeof(char) * _ssid.length());
		strcpy(this->ssid, _ssid.c_str());

		String _password = WiFi.psk();
		this->password = (char*) malloc(sizeof(char) * _password.length());
		strcpy(this->password, _password.c_str());

    this->preferences.putUInt("configurated", 1);
		this->preferences.putString("ssid", _ssid);
    this->preferences.putString("password", _password);

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
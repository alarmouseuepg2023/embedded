#include "WiFiConnection.h"


WiFiConnection::WiFiConnection(char* espTouchPassword, void (*cb)(WiFiEvent_t)) {
	this->espTouchPassword = espTouchPassword;
	this->smartConfigStatus = SmartConfigStatus::WAITING;
	WiFi.onEvent(cb);
}

void WiFiConnection::updateWifiCredentialsSaved(int config, String _ssid, String _password) {
	this->preferences.putUInt(PREF_CONFIGURATED, config);

	if (config == WIFI_PREF_SAVED) {
		this->preferences.putString(PREF_SSID, _ssid);
		this->preferences.putString(PREF_PASSWORD, _password);
	}
	else {
		this->preferences.remove(PREF_SSID);
		this->preferences.remove(PREF_PASSWORD);
	}
}

void WiFiConnection::copyCredentials(String _ssid, String _password) {
	if (this->ssid != NULL) free(this->ssid);
	if (this->password != NULL) free(this->password);

	this->ssid = (char*) malloc(sizeof(char) * _ssid.length());
	strcpy(this->ssid, _ssid.c_str());
	
	this->password = (char*) malloc(sizeof(char) * _password.length());
	strcpy(this->password, _password.c_str());
}

void WiFiConnection::setup() {
	this->preferences.begin(PREF_NAMESPACE);

  if (this->hasWifiCredentialsSaved()) {
		this->smartConfigStatus = SmartConfigStatus::FINISHED;

		this->copyCredentials(
			this->preferences.getString(PREF_SSID),
			this->preferences.getString(PREF_PASSWORD)
		);
  }
}

void WiFiConnection::connect() {
	WiFi.mode(WIFI_STA);
  WiFi.begin(this->ssid, this->password);
}

bool WiFiConnection::hasWifiCredentialsSaved() {
  return this->preferences.getUInt(PREF_CONFIGURATED) == WIFI_PREF_SAVED;
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
	this->updateWifiCredentialsSaved(WIFI_PREF_UNSAVED);
	this->preferences.end();
	ESP.restart();
}

bool WiFiConnection::waitSmartConfig() {
	if (this->smartConfigStatus == SmartConfigStatus::WAITING) {
		this->smartConfigStatus = SmartConfigStatus::STARTED;

		WiFi.mode(WIFI_AP_STA);
		WiFi.beginSmartConfig(SC_TYPE_ESPTOUCH_V2, this->espTouchPassword);
	}

	if (WiFi.smartConfigDone()) {
		String _ssid = WiFi.SSID();
		String _password = WiFi.psk();

		this->smartConfigStatus = SmartConfigStatus::FINISHED;
		this->copyCredentials(_ssid, _password);
		this->updateWifiCredentialsSaved(
			WIFI_PREF_SAVED, _ssid, _password
		);

		return true;
	}

	return false;
}
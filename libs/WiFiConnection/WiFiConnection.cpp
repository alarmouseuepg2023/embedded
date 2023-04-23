#include "WiFiConnection.h"


WiFiConnection::WiFiConnection(char* espTouchPassword, void (*cb)(WiFiEvent_t)) {
	this->espTouchPassword = espTouchPassword;
	this->smartConfigStatus = SmartConfigStatus::WAITING;
	WiFi.onEvent(cb);
}

void WiFiConnection::updateWifiCredentialsSaved(int config, String ssid, String password) {
	this->preferences.putUInt(PREF_CONFIGURATED, config);

	if (config == WIFI_PREF_SAVED) {
		this->preferences.putString(PREF_SSID, ssid);
		this->preferences.putString(PREF_PASSWORD, password);
	}
	else {
		this->preferences.remove(PREF_SSID);
		this->preferences.remove(PREF_PASSWORD);
	}
}

void WiFiConnection::setup() {
	this->preferences.begin(PREF_NAMESPACE);

  if (this->hasWifiCredentialsSaved()) {
		this->smartConfigStatus = SmartConfigStatus::FINISHED;

		String ssid_saved = this->preferences.getString(PREF_SSID);
		this->ssid = (char*) malloc(sizeof(char) * ssid_saved.length());
		strcpy(this->ssid, ssid_saved.c_str());
		
		String password_saved = this->preferences.getString(PREF_PASSWORD);
		this->password = (char*) malloc(sizeof(char) * password_saved.length());
		strcpy(this->password, password_saved.c_str());
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

		this->updateWifiCredentialsSaved(
			WIFI_PREF_SAVED, _ssid, _password
		);

		return true;
	}

	return false;
}
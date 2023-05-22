#ifndef WI_FI_CONNECTION_H
#define WI_FI_CONNECTION_H

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ConnectionManager.h>

#define USE_FLASH_MEMORY 0
#define PRINT_STATUS_IN_MS 2000
#define PREF_NAMESPACE "alarmouse"
#define PREF_SSID "ssid"
#define PREF_PASSWORD "password"
#define PREF_CONFIGURATED "configurated"
#define WIFI_PREF_SAVED 1
#define WIFI_PREF_UNSAVED 0


enum SmartConfigStatus {
  WAITING = 0,
  STARTED = 1,
  FINISHED = 2
};

class WiFiConnection : public ConnectionManager {

private:
  char* ssid;
  char* password;
  char* espTouchPassword;
  bool hasWifiCredentialsSaved;
  Preferences preferences;
  SmartConfigStatus smartConfigStatus;

  void copyCredentials(String,String);
  void updateWifiCredentialsSaved(unsigned int,String="",String="");

protected:
  void connect();

public:
  WiFiConnection(char*, void (*)(WiFiEvent_t));
  bool connected();
	String getMacAddress();
  String getSsid();
	IPAddress getLocalIP();
  bool waitSmartConfig();
  void resetSmartConfig();
  void setup();
  bool hasWifiCredentials();

};


#endif
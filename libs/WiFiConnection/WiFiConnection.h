#ifndef WI_FI_CONNECTION_H
#define WI_FI_CONNECTION_H

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ConnectionManager.h>

#define PRINT_STATUS_IN_MS 2000
#define PREF_NAMESPACE "alarmouse"
#define PREF_SSID "ssid"
#define PREF_PASSWORD "password"
#define PREF_CONFIGURATED "configurated"


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
  Preferences preferences;
  SmartConfigStatus smartConfigStatus;

protected:
  void connect();

public:
  WiFiConnection(char*, void (*)(WiFiEvent_t));
  bool connected();
	String getMacAddress();
	IPAddress getLocalIP();
  bool waitSmartConfig();
  void resetSmartConfig();
  void setup();
  bool hasWifiCredentialsSaved();

};


#endif
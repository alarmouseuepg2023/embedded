#ifndef WI_FI_CONNECTION_H
#define WI_FI_CONNECTION_H

#include <Arduino.h>
#include <WiFi.h>
#include <ConnectionManager.h>

#define PRINT_STATUS_IN_MS 2000


enum SmartConfigStatus {
  WAITING = 0,
  STARTED = 1,
  FINISHED = 2
}

class WiFiConnection : public ConnectionManager {

private:
  char* ssid;
  char* password;
  char* espTouchPassword;
  SmartConfigStatus smartConfigStatus;

protected:
  void connect();

public:
  WiFiConnection(char*, void (*)(WiFiEvent_t));
  bool connected();
	String getMacAddress();
	IPAddress getLocalIP();
	void printStatus();
  bool waitSmartConfig();
  void resetSmartConfig();

};


#endif
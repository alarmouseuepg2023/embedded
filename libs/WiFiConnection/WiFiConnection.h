#ifndef WI_FI_CONNECTION_H
#define WI_FI_CONNECTION_H

#include <Arduino.h>
#include <WiFi.h>
#include <ConnectionManager.h>

#define PRINT_STATUS_IN_MS 2000


class WiFiConnection : public ConnectionManager {

private:
  char* ssid;
  char* password;
  char* espTouchPassword;

protected:
  void connect();

public:
  WiFiConnection(char*);
  bool connected();
	String getMacAddress();
	IPAddress getLocalIP();
	void printStatus();
  bool waitSmartConfig();

};


#endif
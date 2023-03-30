#ifndef WI_FI_CONNECTION_H
#define WI_FI_CONNECTION_H

#include <Arduino.h>
#include <WiFi.h>
#include <ConnectionManager.h>

#define PRINT_STATUS_IN_MS 2000


typedef struct wifi_credentials
{
  bool configured;
  char ssid[32];
  char password[32];
} WiFiCredentials;


class WiFiConnection : public ConnectionManager {

private:
  WiFiCredentials* wifiCredentials;

protected:
  void connect();

public:
  WiFiConnection(WiFiCredentials*);
  bool connected();
	String getMacAddress();
	IPAddress getLocalIP();
	void printStatus();

};


#endif
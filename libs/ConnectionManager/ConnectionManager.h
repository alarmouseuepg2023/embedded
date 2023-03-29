#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <Arduino.h>

#define RECONNECT_ATTEMPTS 5
#define WAITING_TO_RETRY_RECONNECT_IN_MS 2000
#define RETRY_RECONNECT_IN_MS 120000


class ConnectionManager {

private:
  int reconnection_attempts;
  void resetReconnectionAttempts();
  void retryReconnection();

protected:
  virtual void connect() = 0;

public:
  ConnectionManager();
  void reconnect();
  virtual bool connected() = 0;

};


#endif
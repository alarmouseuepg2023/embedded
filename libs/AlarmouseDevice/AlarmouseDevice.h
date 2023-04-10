#ifndef ALARMOUSE_DEVICE_H
#define ALARMOUSE_DEVICE_H

#include <Arduino.h>


enum DeviceStatus {
  UNCONFIGURED = 0,
  LOCKED = 1,
  UNLOCKED = 2,
  TRIGGERED = 3
};

enum DeviceEvent {
  STATUS_CHANGED = 0,
};

class AlarmouseDevice {
private:
  DeviceStatus status;
  unsigned int sensorPin;
  unsigned int alarmPin;
  void (*onEventCallback)(DeviceEvent);

  bool targetDetected();
  void changeStatus(DeviceStatus);

public:
  AlarmouseDevice(int,int,void (*)(DeviceEvent));
  void statusChangedByExternal(char);
  void loop();
  bool configurated();
  void setIsConfigurated(bool);
  void resetConfig();
};


#endif
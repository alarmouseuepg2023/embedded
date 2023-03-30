#ifndef ALARMOUSE_DEVICE_H
#define ALARMOUSE_DEVICE_H

#include <Arduino.h>


enum DeviceStatus {
  LOCKED = 1,
  UNLOCKED = 2,
  TRIGGERED = 3
};

class AlarmouseDevice {
private:
  DeviceStatus status;
  unsigned int sensorPin;
  unsigned int alarmPin;

  bool targetDetected();
  void changeStatus(DeviceStatus);

public:
  AlarmouseDevice(int,int);
  void statusChangedByExternal(byte);
  void loop();
};


#endif
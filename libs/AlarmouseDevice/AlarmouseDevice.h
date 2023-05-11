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
  unsigned int rfControlPin;
  void (*onEventCallback)(DeviceEvent);

  void changeStatus(DeviceStatus);
  void onRfChangedCallback();
  void onSensorDetectedCallback();

  static AlarmouseDevice* interruptControl;
  static void IRAM_ATTR onRfChangedHandler();
  static void IRAM_ATTR onSensorDetectedHandler();

public:
  AlarmouseDevice(int,int,int,void (*)(DeviceEvent));
  void statusChangedByExternal(char);
  void loop();
  void setup();
  DeviceStatus getStatus();
};


#endif
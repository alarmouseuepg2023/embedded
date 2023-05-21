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
  FAILED_STATUS_CHANGED_ATTEMPT = 1,
};

class AlarmouseDevice {

private:
  DeviceStatus status;
  unsigned int sensorPin;
  unsigned int alarmPin;
  void (*onEventCallback)(DeviceEvent);

  void changeStatus(DeviceStatus);
  void onSensorDetectedCallback();
  void onRfControlPressedCallback();

  static AlarmouseDevice* interruptControl;
  static void IRAM_ATTR onSensorDetectedHandler();

public:
  AlarmouseDevice(int,int,void (*)(DeviceEvent));
  void statusChangedByExternal(char);
  void loop();
  void setup();
  void setIsConfigurated();
  DeviceStatus getStatus();
  
  static void onRfControlPressedHandler();
};


#endif
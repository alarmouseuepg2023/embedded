#include "AlarmouseDevice.h"

bool AlarmouseDevice::targetDetected() {
  return !(digitalRead(this->sensorPin) == HIGH);
}

AlarmouseDevice::AlarmouseDevice(int sensor, int alarm) {
  this->alarmPin = alarm;
  this->sensorPin = sensor;
  this->status = DeviceStatus::UNLOCKED;

  pinMode(this->sensorPin, INPUT);
  pinMode(this->alarmPin, OUTPUT);
}

void AlarmouseDevice::statusChangedByExternal(byte status) {
  if (status == 1) this->status = DeviceStatus::LOCKED;
  if (status == 2) this->status = DeviceStatus::UNLOCKED;
  if (status == 3) this->status = DeviceStatus::TRIGGERED;
}

void AlarmouseDevice::changeStatus(DeviceStatus status) {
  this->status = status;
  // publish mqtt
}

void AlarmouseDevice::loop() {
  if (this->status == DeviceStatus::LOCKED && this->targetDetected())
    this->changeStatus(DeviceStatus::TRIGGERED);

  // temporary
  if (this->status == DeviceStatus::TRIGGERED) 
    tone(this->alarmPin, 262, 1000);
  else
    noTone(this->alarmPin);
}
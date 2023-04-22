#include "AlarmouseDevice.h"

AlarmouseDevice::AlarmouseDevice(int sensor, int alarm, void (*cb)(DeviceEvent event)) {
  this->alarmPin = alarm;
  this->sensorPin = sensor;
  this->status = DeviceStatus::UNCONFIGURED;
  this->onEventCallback = cb;

  pinMode(this->sensorPin, INPUT);
  pinMode(this->alarmPin, OUTPUT);
}

bool AlarmouseDevice::targetDetected() {
  return !(digitalRead(this->sensorPin) == HIGH);
}

DeviceStatus AlarmouseDevice::getStatus() {
  return this->status;
}

void AlarmouseDevice::statusChangedByExternal(char status) {
  int status_converted = atoi((char*)(&status));

  if (status_converted == 1) this->status = DeviceStatus::LOCKED;
  if (status_converted == 2) this->status = DeviceStatus::UNLOCKED;
  if (status_converted == 3) this->status = DeviceStatus::TRIGGERED;
}

void AlarmouseDevice::changeStatus(DeviceStatus status) {
  this->status = status;
  this->onEventCallback(DeviceEvent::STATUS_CHANGED);
}

void AlarmouseDevice::loop() {
  if (this->status == DeviceStatus::LOCKED && this->targetDetected())
    this->changeStatus(DeviceStatus::TRIGGERED);

  // temporary
  digitalWrite(this->alarmPin, this->status == DeviceStatus::TRIGGERED);
}
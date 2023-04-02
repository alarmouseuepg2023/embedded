#include "AlarmouseDevice.h"

bool AlarmouseDevice::targetDetected() {
  return !(digitalRead(this->sensorPin) == HIGH);
}

AlarmouseDevice::AlarmouseDevice(int sensor, int alarm) {
  this->alarmPin = alarm;
  this->sensorPin = sensor;
  this->status = DeviceStatus::UNCONFIGURED;

  pinMode(this->sensorPin, INPUT);
  pinMode(this->alarmPin, OUTPUT);
}

bool AlarmouseDevice::configurated() {
  return this->status != DeviceStatus::UNCONFIGURED;
}

void AlarmouseDevice::setIsConfigurated(bool wifiConfigurated) {
  if (wifiConfigurated) this->status = DeviceStatus::UNLOCKED;
}

void AlarmouseDevice::resetConfig() {
  this->status = DeviceStatus::UNCONFIGURED;
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
  digitalWrite(this->alarmPin, this->status == DeviceStatus::TRIGGERED);
}
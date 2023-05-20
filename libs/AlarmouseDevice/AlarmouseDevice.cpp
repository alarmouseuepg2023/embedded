#include "AlarmouseDevice.h"

AlarmouseDevice* AlarmouseDevice::interruptControl = nullptr;

AlarmouseDevice::AlarmouseDevice(
  int sensor, int alarm, int rfControl, void (*cb)(DeviceEvent event)
) {
  interruptControl = this;
  this->alarmPin = alarm;
  this->sensorPin = sensor;
  this->rfControlPin = rfControl;
  this->status = DeviceStatus::UNCONFIGURED;
  this->onEventCallback = cb;

  pinMode(this->alarmPin, OUTPUT);
}

void AlarmouseDevice::setup() {
  attachInterrupt(digitalPinToInterrupt(this->sensorPin), onSensorDetectedHandler, RISING);
  attachInterrupt(digitalPinToInterrupt(this->rfControlPin), onRfChangedHandler, CHANGE);
}

void AlarmouseDevice::onSensorDetectedHandler() {
  interruptControl->onSensorDetectedCallback();
}

void AlarmouseDevice::onRfChangedHandler() {
  interruptControl->onRfChangedCallback();
}

void AlarmouseDevice::onSensorDetectedCallback() {
  if (this->status != DeviceStatus::LOCKED)
    return;

  this->changeStatus(DeviceStatus::TRIGGERED);
}

void AlarmouseDevice::onRfChangedCallback() {
  // todo
  return;
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

void AlarmouseDevice::setIsConfigurated() {
  if (this->status != DeviceStatus::UNCONFIGURED) 
    return;

  this->status = DeviceStatus::UNLOCKED;
}

void AlarmouseDevice::loop() {
  digitalWrite(
    this->alarmPin, 
    this->status == DeviceStatus::TRIGGERED? HIGH : LOW
  );
}
#include "AlarmouseDevice.h"

AlarmouseDevice* AlarmouseDevice::interruptControl = nullptr;

AlarmouseDevice::AlarmouseDevice(
  int sensor, int alarm, void (*cb)(DeviceEvent event)
) {
  interruptControl = this;
  this->alarmPin = alarm;
  this->sensorPin = sensor;
  this->status = DeviceStatus::UNCONFIGURED;
  this->onEventCallback = cb;

  pinMode(this->alarmPin, OUTPUT);
}

void AlarmouseDevice::setup() {
  attachInterrupt(digitalPinToInterrupt(this->sensorPin), onSensorDetectedHandler, RISING);
}

void AlarmouseDevice::onSensorDetectedHandler() {
  interruptControl->onSensorDetectedCallback();
}

void AlarmouseDevice::onSensorDetectedCallback() {
  if (this->status != DeviceStatus::LOCKED)
    return;

  this->changeStatus(DeviceStatus::TRIGGERED);
}

void AlarmouseDevice::onRfControlPressedHandler() {
  interruptControl->onRfControlPressedCallback();
}

void AlarmouseDevice::onRfControlPressedCallback() {
  //todo
}

DeviceStatus AlarmouseDevice::getStatus() {
  return this->status;
}

void AlarmouseDevice::statusChangedByExternal(char status) {
  int status_converted = atoi((char*)(&status));

  switch(status_converted) {
    case 1: this->status = DeviceStatus::LOCKED; break;
    case 2: this->status = DeviceStatus::UNLOCKED; break;
    case 3: this->status = DeviceStatus::TRIGGERED; break;
    default: this->onEventCallback(DeviceEvent::FAILED_STATUS_CHANGED_ATTEMPT);
  }
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
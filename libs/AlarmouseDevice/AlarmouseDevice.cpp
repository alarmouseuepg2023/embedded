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
  this->lastAlarmPlayed = 0;
  this->lastAlarmTriggered = 0;

  pinMode(this->alarmPin, OUTPUT);
}

void AlarmouseDevice::setup() {
  attachInterrupt(digitalPinToInterrupt(this->sensorPin), onSensorDetectedHandler, RISING);
}

void AlarmouseDevice::onSensorDetectedHandler() {
  interruptControl->onSensorDetectedCallback();
}

void AlarmouseDevice::onSensorDetectedCallback() {
  if (
    this->status != DeviceStatus::LOCKED || 
    this->status == DeviceStatus::TRIGGERED
  )
    return;

  this->changeStatus(DeviceStatus::TRIGGERED);
}

void AlarmouseDevice::onRfControlPressedHandler() {
  interruptControl->onRfControlPressedCallback();
}

void AlarmouseDevice::onRfControlPressedCallback() {
  switch (this->status) {
    case DeviceStatus::UNLOCKED:
      this->changeStatus(DeviceStatus::LOCKED); 
      break;

    case DeviceStatus::UNCONFIGURED:
      break;
    
    case DeviceStatus::LOCKED:
    case DeviceStatus::TRIGGERED:
      this->changeStatus(DeviceStatus::UNLOCKED); 
      break;
  }
}

DeviceStatus AlarmouseDevice::getStatus() {
  return this->status;
}

void AlarmouseDevice::triggerAlarm2UserFeedback() {
  this->lastAlarmPlayed = millis();
}

void AlarmouseDevice::statusChangedByExternal(char status) {
  int status_converted = atoi((char*)(&status));

  switch(status_converted) {
    case 1: this->changeStatus(DeviceStatus::LOCKED);    break;
    case 2: this->changeStatus(DeviceStatus::UNLOCKED);  break;
    case 3: this->changeStatus(DeviceStatus::TRIGGERED); break;
    case 0: case 4:                                      break;
    default: this->onEventCallback(DeviceEvent::FAILED_STATUS_CHANGED_ATTEMPT);
  }
}

void AlarmouseDevice::changeStatus(DeviceStatus status) {
  if (
    this->status == DeviceStatus::LOCKED && status == DeviceStatus::UNLOCKED ||
    this->status == DeviceStatus::UNLOCKED && status == DeviceStatus::LOCKED
  )
    this->lastAlarmPlayed = millis();

  if (status == DeviceStatus::TRIGGERED)
    this->lastAlarmTriggered = millis();

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
    (
      this->status == DeviceStatus::TRIGGERED &&
      millis() <= this->lastAlarmTriggered + MILLIS_TO_PLAY_ALARM_ON_TRIGGERED
    ) 
    ||
    (
      this->lastAlarmPlayed != 0 && 
      millis() <= this->lastAlarmPlayed + MILLIS_TO_PLAY_ALARM_ON_STATUS_CHANGED
    )
  );
}
#include "Debounce.h"

Debounce::Debounce(byte pin, int timePressing, void(*cb)()) {
  this->pin = pin;
  this->timePressing = timePressing;
  this->onPressedCallback = cb;

  this->lastTime = 0;
  this->lastPressedTime = 0;
  this->state = LOW;
  this->lastState = LOW;
  this->pressed = false;

  pinMode(this->pin, INPUT);
}

void Debounce::loop() {
  int value = digitalRead(this->pin);

  if (value != this->lastState)
    this->lastTime = millis();

  if ((millis() - this->lastState) > DELAY_DEBOUNCE && value != this->state) {
    this->state = value;
    
    if (this->state == LOW) {
      this->pressed = true;
      this->lastPressedTime = millis();
    }
    else {
      this->pressed = false;
    }
  }

  if (this->pressed && (millis() - this->lastPressedTime) > this->timePressing) {
    this->pressed = false;
    this->onPressedCallback();
  }

  this->lastState = value;
}
#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <Arduino.h>

#define DELAY_DEBOUNCE 50


class Debounce {

private:
  unsigned long lastTime;
  unsigned long lastPressedTime;
  unsigned int timePressing;
  byte state;
  byte lastState;
  byte pin;
  bool pressed;
  void (*onPressedCallback)();

public:
  Debounce(byte,int,void(*)());
  void loop();

};


#endif
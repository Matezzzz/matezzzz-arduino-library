#ifndef LED_H
#define LED_H

#include "funshield.h"

class LED{
  const uint8_t pin;
public:
  LED(uint8_t p) : pin(p)
  {
    pinMode(p, OUTPUT);
  }
  void turnOn() const{
    digitalWrite(pin, ON);
  }
  void turnOff() const{
    digitalWrite(pin, OFF);
  }
};

const LED LEDs[]{LED(led1_pin), LED(led2_pin), LED(led3_pin), LED(led4_pin)};
constexpr uint8_t LED_count = sizeof(LEDs) / sizeof(uint8_t);


#endif
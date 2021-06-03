#ifndef LED_ANIM_H
#define LED_ANIM_H

#include "led.h"
#include "timing.h"

template<int phase_count>
class Animation{
  Timer timer;
protected:
  uint8_t phase;
public:
  Animation(uint16_t update_period) : timer(update_period)
  {}
  void update(){
    if (timer.update()) phase = (++phase) % phase_count;
  }
};



class LEDSnake : public Animation<4>{
  uint8_t snake_length;
public:
  LEDSnake(uint8_t length = 2, uint16_t update_period = 500) : Animation(update_period), snake_length(length)
  {}
  void update(){
    Animation::update();
    for (int i = 0; i < LED_count; i++){
      if ((i + phase) % LED_count < snake_length){
        LEDs[i].turnOn();
      }else{
        LEDs[i].turnOff();
      }
    }
  }
};



class LEDJump : public Animation<6>{
public:
  LEDJump(uint16_t update_period = 500) : Animation(update_period)
  {}
  void update(){
    Animation::update();
    uint8_t p = phase;
    if (phase > 3) p = 6 - phase;
    for (int i = 0; i < LED_count; i++){
      if (i == p){
        LEDs[i].turnOn();
      }
      else{
        LEDs[i].turnOff();
      }
    }
  }
};



#endif
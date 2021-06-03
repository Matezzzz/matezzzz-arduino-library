#ifndef LED_ANIM_H
#define LED_ANIM_H

#include "led.h"
#include "timing.h"


/**
 * Animation
 *  - base class for all animations. Phase count represents the amount of phases the animation will go through, and it is increased by one each 'update_period' miliseconds. It resets to zero when larger than phase_count
 */
template<int phase_count>
class Animation{
  Timer<> timer;
protected:
  uint8_t phase;
public:
  Animation(uint16_t update_period) : timer(update_period)
  {}
  void init(){
    timer.init();
  }
  void update(){
    if (timer.update()) phase = (++phase) % phase_count;
  }
};


/**
 * LedSnake
 *  - snake going upwards through LEDs - ##__ -> _##_ -> __## -> #__# -> ##__, and repeat
 */
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


/**
 * LedJump
 *  - one jumping LED. #___ -> _#__ -> __#_ -> ___# -> __#_ -> _#__ -> #___, and repeat
 */
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
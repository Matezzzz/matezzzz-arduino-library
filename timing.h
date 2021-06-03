#ifndef TIMING_H
#define TIMING_H

#include "funshield.h"

class Stopwatch{
  uint16_t timer_start;
public:
  Stopwatch() : timer_start(millis())
  {}
  inline uint16_t elapsedTime(){
    return millis() - timer_start;
  }
  inline void reset(){
    timer_start = millis();
  }
};


class Timer : public Stopwatch{
  uint16_t timer_length;
public:
  Timer(uint16_t length = 0) : timer_length(length)
  {}
  bool update(){
    if (elapsed()){
      reset();
      return true;
    }
    return false;
  }
  bool elapsed(){
    return elapsedTime() >= timer_length;
  }
};

#endif
#ifndef TIMING_H
#define TIMING_H

#include "funshield.h"

template<typename time_unit = uint16_t>
class Stopwatch{
  time_unit timer_start;
public:
  Stopwatch() : timer_start(millis())
  {}
  time_unit elapsedTime(){
    return millis() - timer_start;
  }
  void reset(){
    timer_start = millis();
  }
};

template<typename time_unit = uint16_t>
class Timer : public Stopwatch<time_unit>{
  time_unit timer_length;
public:
  Timer(time_unit length = 0) : timer_length(length)
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
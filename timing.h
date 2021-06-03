#ifndef TIMING_H
#define TIMING_H

#include "funshield.h"


//Measures time elapsed from a time point
template<typename time_unit = uint16_t>
class Stopwatch{
  time_unit timer_start;
public:
  Stopwatch() : timer_start()
  {}
  void init(){
    reset();
  }
  time_unit elapsedTime(){
    return millis() - timer_start;
  }
  void reset(){
    timer_start = millis();
  }
};

//Counts down remaining time from a given point
template<typename time_unit = uint16_t>
class Timer : public Stopwatch<time_unit>{
  time_unit timer_length;
public:
  Timer(time_unit length = 0) : timer_length(length)
  {}
  //if elapsed, return true and reset, else return false
  bool update(){
    if (this->elapsed()){
      this->reset();
      return true;
    }
    return false;
  }
  bool elapsed(){
    return this->elapsedTime() >= timer_length;
  }
};

#endif
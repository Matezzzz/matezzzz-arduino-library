#ifndef SEGMENT_DISPLAY_H
#define SEGMENT_DISPLAY_H

#include "funshield.h"


const int DIGIT_NONE = 10;
const int DIGIT_d = 11;
constexpr uint8_t extended_digits[]{ 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff, 0xa1};

constexpr uint8_t segment_display_pins[]{latch_pin, data_pin, clock_pin};



class SegmentDisplay{
  enum PINS{LATCH, DATA, CLOCK};
  uint8_t pins[3];
protected:
  uint8_t state[4];
public:
  SegmentDisplay(const uint8_t p[3] = segment_display_pins) : pins{p[0], p[1], p[2]}
  {
    for (uint8_t pin : pins) pinMode(pin, OUTPUT);
    for (int i = 0; i < 4; i++) state[i] = 0xFF;
  }
  void update(){
    for (uint8_t i = 0; i < 4; i++) writeSegment(i);
  }
  void writeSegment(uint8_t segment){
    digitalWrite(pins[LATCH], LOW);
    shiftOut(pins[DATA], pins[CLOCK], MSBFIRST, state[segment]);
    shiftOut(pins[DATA], pins[CLOCK], MSBFIRST, 1 << segment);
    digitalWrite(pins[LATCH], HIGH);
  }
};




class NumberSegmentDisplay : public SegmentDisplay{
public:
  NumberSegmentDisplay(uint16_t n = 0, const uint8_t p[3] = segment_display_pins) : SegmentDisplay(p)
  {
    setNumber(n);
  }
  void setNumber(uint16_t n){
    setSegments((n < 1000) ? DIGIT_NONE : (n / 1000), (n < 100) ? DIGIT_NONE : (n / 100 % 10), (n < 10) ? DIGIT_NONE : (n / 10 % 10), n % 10);
  }
  void setSegments(uint8_t a, uint8_t b, uint8_t c, uint8_t d){
    setSegNumber(0, a);
    setSegNumber(1, b);
    setSegNumber(2, c);
    setSegNumber(3, d);
  }
  void setSegNumber(uint8_t seg, uint8_t n){
    state[seg] = extended_digits[n];
  }
};

#endif
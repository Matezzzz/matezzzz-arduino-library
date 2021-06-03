#ifndef SEGMENT_DISPLAY_H
#define SEGMENT_DISPLAY_H

#include "funshield.h"


const int DIGIT_NONE = 10;
const int DIGIT_d = 11;
//digits 0-9, then blank space, then letter d
constexpr uint8_t extended_digits[]{ 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff, 0xa1};

constexpr uint8_t segment_display_pins[]{latch_pin, data_pin, clock_pin};



class SegmentDisplay{
  enum PINS{LATCH, DATA, CLOCK};
  uint8_t pins[3];
  uint8_t current_digit = 0;
protected:
  //current state of display - one bit for each segment or/off
  uint8_t seg_state[4];
public:
  SegmentDisplay(const uint8_t p[3] = segment_display_pins) : pins{p[0], p[1], p[2]}
  {
    for (int i = 0; i < 4; i++) seg_state[i] = 0xFF;
  }
  void init(){
    for (uint8_t pin : pins) pinMode(pin, OUTPUT);
  }
  void update(){
    writeSegment(current_digit);
    current_digit = (current_digit + 1) % 4;
  }
  void writeSegment(uint8_t segment){
    digitalWrite(pins[LATCH], LOW);
    shiftOut(pins[DATA], pins[CLOCK], MSBFIRST, seg_state[segment]);
    shiftOut(pins[DATA], pins[CLOCK], MSBFIRST, 1 << segment);
    digitalWrite(pins[LATCH], HIGH);
  }
};



template<bool leading_zeroes=false>
class NumberSegmentDisplay : public SegmentDisplay{
public:
  NumberSegmentDisplay(uint16_t n = 0, const uint8_t p[3] = segment_display_pins) : SegmentDisplay(p)
  {
    setNumber(n);
  }
  void setNumber(uint16_t n){
    setSegments((n < 1000) ? emptyDigit() : (n / 1000), (n < 100) ? emptyDigit() : (n / 100 % 10), (n < 10) ? emptyDigit() : (n / 10 % 10), n % 10);
  }
  void setSegments(uint8_t a, uint8_t b, uint8_t c, uint8_t d){
    setSegNumber(0, a);
    setSegNumber(1, b);
    setSegNumber(2, c);
    setSegNumber(3, d);
  }
  void setSegNumber(uint8_t seg, uint8_t n){
    seg_state[seg] = extended_digits[n];
  }
private:
  constexpr static uint8_t emptyDigit(){
    return leading_zeroes ? 0 : DIGIT_NONE;
  }
};

#endif
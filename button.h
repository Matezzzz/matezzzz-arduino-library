#ifndef BUTTON_H
#define BUTTON_H


#include "funshield.h"
#include "timing.h"


/**
 * Button
 *  - basic button class. isPressed returns true/false based on current state of button
 */
class Button{
  const uint8_t pin;
public:
  Button(uint8_t p) : pin(p)
  {}
  void init(){
    pinMode(pin, INPUT);
  }
  bool isPressed(){
    return !digitalRead(pin);
  }
};


/**
 * TimedButton
 *  - all the time the button is pressed withnin a certain interval since last press will be ignored
 */
class TimedButton : public Button{
  Timer<> cooldown;
public:
  TimedButton(uint8_t pin, uint16_t cd) : Button(pin), cooldown(cd)
  {}
  void init(){
    Button::init();
    cooldown.init();
  }
  bool isPressed(){
    if (Button::isPressed() && cooldown.elapsed()){
      cooldown.reset();
      return true;
    }
    return false;
  }
};



/**
 * EventButton
 *  - has press down and press up events - these happen for one update call after button is pressed/unpressed
 */
class EventButton : public Button{
  uint8_t data;
  static constexpr uint8_t WAS_PRESSED = 0x1;
  static constexpr uint8_t PRESS_DOWN = 0x2;
  static constexpr uint8_t PRESS_UP = 0x4;
public:
  EventButton(uint8_t pin) : Button(pin), data(0)
  {}
  
  void update(){
    bool pressed = isPressed();

    if (pressed){
      if (getBool<WAS_PRESSED>()){
        clearBool<PRESS_DOWN>();
      }else{
        setBool<PRESS_DOWN>();
      }
    }else{
      if (getBool<WAS_PRESSED>()){
        setBool<PRESS_UP>();
      }else{
        clearBool<PRESS_UP>();
      }
    }
    setBool<WAS_PRESSED>(pressed);
  }
  bool pressEvent(){
    return getBool<PRESS_DOWN>();
  }
  bool liftEvent(){
    return getBool<PRESS_UP>();
  }
private:
  template<uint8_t mask>
  inline bool getBool(){
    return (data & mask);
  }
  template<uint8_t mask>
  inline void setBool(){
    data |= mask;
  }
  template<uint8_t mask>
  inline void clearBool(){
    data &= ~mask;
  }
  template<uint8_t mask>
  inline void setBool(bool val){
    if (val){
      setBool<mask>();
    }
    else{
      clearBool<mask>();
    }
  }
};



/**
 * DoublyTimedButton
 *  - Similar to timed button, however, interval changes after first repetition
 *  - behaviour is similar to pc keyboard keys, first key press writes one character, but holding the key for an extended period of time writes many at once
 */
class DoublyTimedButton : public Button{
  enum PressState{
    NOT_PRESSED,
    PRESSED_FIRST,
    PRESSED_LATER
  } state = NOT_PRESSED;
  Timer<> timer_first;
  Timer<> timer_later;
public:
  DoublyTimedButton(uint8_t pin, uint16_t cd_first, uint16_t cd) : Button(pin), timer_first(cd_first), timer_later(cd)
  {}
  void init(){
    Button::init();
    timer_first.init();
    timer_later.init();
  }
  bool isPressed(){
    if (Button::isPressed()){
      if (state == NOT_PRESSED){
        timer_first.reset();
        state = PRESSED_FIRST;
        return true;
      }else if (state == PRESSED_FIRST && timer_first.elapsed()){
        timer_later.reset();
        state = PRESSED_LATER;
        return true;
      }else if (state == PRESSED_LATER && timer_later.elapsed()){
        timer_later.reset();
        return true;
      }
      return false;
    }else{
      state = NOT_PRESSED;
      return false;
    }
  }
};


#endif

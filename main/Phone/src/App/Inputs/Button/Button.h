/*
  Button.h - Library for handling button inputs.
*/
#ifndef Button_h
#define Button_h
#include "Arduino.h"

class Button {
  private:
    uint32_t state_2_start_time;
    uint32_t button_change_time;    
    uint32_t debounce_duration;
    uint32_t long_press_duration;
    uint8_t pin;
    uint8_t flag;
    bool button_pressed;
    uint8_t state;
    void read();
  public:
    Button(uint8_t button_pin);
    uint8_t update_state();
};

#endif
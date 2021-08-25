#include "Arduino.h"
#include "Button.h"

Button::Button(uint8_t button_pin) {  
  flag = 0;  
  state = 0;
  pin = button_pin;
  state_2_start_time = millis();
  button_change_time = millis();
  debounce_duration = 10;
  long_press_duration = 1000;
  button_pressed = 0;
}

void Button::read(){
  uint8_t button_state = digitalRead(pin);  
  button_pressed = !button_state;
}

uint8_t Button::update_state() {
  read();
  flag = 0;
  if (state==0) {
    if (button_pressed) {
      state = 1;
      button_change_time = millis();
    }
  } else if (state==1) {
    if (!button_pressed) {
      state = 0;
      button_change_time = millis();
    } else {
      if (millis()-button_change_time >= debounce_duration) {
        state = 2;
        state_2_start_time = millis();
      }
    }
  } else if (state==2) {
    if (!button_pressed) {
      state = 4;
      button_change_time = millis();
    } else {
      if (millis()-state_2_start_time >= long_press_duration) {
        state = 3;
      }
    }
  } else if (state==3) {
    if (!button_pressed) {
      state = 4;
      button_change_time = millis();
    }
  } else if (state==4) {        
    if (!button_pressed) {
      if (millis()-button_change_time >= debounce_duration) {
        state = 0;
        if (millis()-state_2_start_time >= long_press_duration) {
          flag = 2;
        } else {
          flag = 1;
        }
      }
    } else {
      if (millis()-state_2_start_time >= long_press_duration) {
        state = 3;
      } else {
        state = 2;
      }
      button_change_time = millis();
    }
  }
  return flag;
}
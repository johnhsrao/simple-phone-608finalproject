/*
  Keyboard.h - Library for a keyboard interface. Allows users to type messages.
*/

#ifndef Keyboard_h
#define Keyboard_h
#include "Arduino.h"

// keyboards
static char *keyboards[3][28] = {
    {"a", "b", "c", "d", "e", "f", "g", 
    "h", "i", "j", "k", "l", "m", "n", 
    "o", "p", "q", "r", "s", "t", "u", 
    "v", "w", "x", "y", "z"},
    {"A", "B", "C", "D", "E", "F", "G", 
    "H", "I", "J", "K", "L", "M", "N", 
    "O", "P", "Q", "R", "S", "T", "U", 
    "V", "W", "X", "Y", "Z"},
    {"1", "2", "3", "4", "5", "6", "7",
    "8", "9", "0", "-", "/", ":", ";",
    "(", ")", "$", "&", "@", ".", ",", 
    "?", "!", "%", "^", "*", "+", "="}
};

static char *case_buttons[2] = {"UC", "LC"};

static char *switch_buttons[3] = {"123", "123", "ABC"};

static char *buttons[3] = {"space", "delete", "enter"};

class Keyboard {
  private:
    char current_message[300];
    uint8_t horizontal_select_state;
    uint8_t vertical_select_state;
    uint8_t offset;
    uint8_t current_keyboard;
  public:
    Keyboard();
    bool handle_user_input(uint8_t button_input_one, uint8_t button_input_two, uint8_t button_input_three);
    char* get_current_message();
    void draw_screen();
    void reset_message();
};

#endif
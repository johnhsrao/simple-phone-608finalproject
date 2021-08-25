/*
  HomeApp.h - Library for the HomeApp child class of App.
              Displays the available apps for the user to access.
*/
#ifndef HomeApp_h
#define HomeApp_h
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Arduino.h"
#include "Phone.h"
#include "App/App.h"

class HomeApp: public App {
    private:
        uint8_t horizontal_select_state;
        uint8_t vertical_select_state;
        uint8_t current_app;
    public:
        HomeApp();
        uint8_t handle_user_input();
        void draw_screen();
        void reset_screen();
};

#endif
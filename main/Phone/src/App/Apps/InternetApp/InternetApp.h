/*
  InternetApp.h - Library for the InternetApp child class of App.
                  Allows users to search Wikipedia for articles.
*/
#ifndef InternetApp_h
#define InternetApp_h
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Arduino.h"
#include "Phone.h"
#include "App/App.h"

class InternetApp: public App {
    public:
        InternetApp();
        uint8_t handle_user_input();
        void draw_screen();
};

#endif
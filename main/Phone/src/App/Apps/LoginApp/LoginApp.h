/*
  LoginApp.h - Library for the LoginApp child class of App.
               Users login into their phone account and connect
               to their WiFi network.
*/
#ifndef LoginApp_h
#define LoginApp_h
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Arduino.h"
#include "Phone.h"
#include "App/App.h"

class LoginApp: public App {
    private:
        // state variables
        uint8_t screen_state; 
        // connect to internet, select login or new user, login, new user, ask to connect to saved network, ask to login as saved user, ask to save network, ask to save user
        uint8_t vertical_select_state;
        uint8_t horizontal_select_state;
        bool state_change;
        // keyboard
        char text_box_one[19];
        char text_box_two[19];
        bool show_keyboard;
        bool incorrect_input;
        // login http request
        bool loading;
        void login();
        // connect to WiFi
        bool connect_to_internet();
        // flash memory functions and variables
        void store_string_eeprom(char* write_string, uint8_t start_addr);
        void retrieve_string_eeprom(char* read_string, uint8_t start_addr);
        bool retrieve_network;
        bool retrieve_user;
        char network[19];
        char network_password[19];
        char user[19];
        char user_password[19];
    public:
        LoginApp();
        uint8_t handle_user_input();
        void reset_screen();
        void draw_screen();
};

#endif
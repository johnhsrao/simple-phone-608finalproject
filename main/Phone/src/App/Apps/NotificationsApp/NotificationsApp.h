/*
    NotificationsApp.h - Library for the NotificationsApp child class of App.
                         Displays notifications of new unread messages and email.
*/
#ifndef NotificationsApp_h
#define NotificationsApp_h
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Arduino.h"
#include "Phone.h"
#include "App/App.h"

class NotificationsApp: public App {
    private:
        const uint16_t GETTING_PERIOD = 10000; // periodicity of checking for notifications
        const uint16_t SHOW_PERIOD = 4000; // periodicity of showing notifications
        uint32_t timer;
        uint32_t show_timer;
        bool loading;
        bool state_change;
        uint32_t message_num;
        uint32_t mail_num;
        char message_from[19];
        char mail_from[50];
        void deserialize_notifications_json();
    public:
        NotificationsApp();
        bool check_notifications();
        void draw_screen();
        uint8_t handle_user_input();
};

#endif
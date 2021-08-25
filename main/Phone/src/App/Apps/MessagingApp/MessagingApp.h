/*
  MessagingApp.h - Library for the MessagingApp child class of App.
                   Allows users to send and receive text messages with other ESP32s.
*/
#ifndef MessagingApp_h
#define MessagingApp_h
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include <ArduinoJson.h>
#include "Arduino.h"
#include "Phone.h"
#include "App/App.h"

class MessagingApp: public App {
    //messaging states
    static const uint8_t SELECT_MENU = 0;
    static const uint8_t SEND_MESSAGE = 1;
    static const uint8_t CHECK_MESSAGE = 2;
    static const uint8_t DISPLAY_MESSAGE = 3;
    static const uint8_t LOADING_SCREEN = 4;
    static const uint8_t SELECT_UNREAD = 0;
    static const uint8_t SELECT_SEND = 1;
    static const uint8_t SELECT_HISTORY = 2;
    static const uint8_t SCREEN_WIDTH = 128; 
    static const uint8_t CHAR_WIDTH = 6;
    static const uint8_t CHAR_HEIGHT = 8;

    uint8_t text_state;
    uint8_t select_state;
    uint8_t prev_selected;
    uint8_t prev_state;
    bool otherSelected;
    bool messageSelected;
    bool firstTime;
    char other[100];
    char message[1000];
    char prev_keyboard_input[500];
    int unread_num;
    int prev_unread_num;
    uint32_t notif_period;
    int timer;
    uint8_t msg_index;
    bool msg_display_updated;
    bool more_msg;
    bool loading;
    bool sending;
    bool checkingNum;
    bool DISPLAYING;
    StaticJsonDocument<1000> parsed_doc;

    public:
        MessagingApp();
        uint8_t handle_user_input();
        void draw_screen();
        void reset_screen();
    private:
        void checkUnreadMessages();
        void checkMessages(char* other, int number);
        void sendMessage(char* other, char* message);
        uint8_t drawMessages(char* d, char* msg, uint8_t yPos);
        void message_parsing(StaticJsonDocument<1000> &doc, int next_msg);
        void checkUnreadNum();
        void draw_selected(uint8_t selection, uint8_t prev);
        uint8_t new_yPos(uint8_t yPos, uint8_t x_start, char* msg);
        void draw_loading();
};

#endif
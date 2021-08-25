/*
  MailApp.h - Library for the MailApp child class of App.
              Allows users to send and receive emails.
*/
#ifndef MailApp_h
#define MailApp_h
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Arduino.h"
#include "Phone.h"
#include "App/App.h"
#include <ArduinoJson.h>

class MailApp: public App {
	static const uint8_t LOGIN_MENU = 0;
    static const uint8_t USER_LOGIN = 1;
    static const uint8_t OPTION_MENU = 2;
    static const uint8_t SEND_MAIL = 3;
    static const uint8_t DISPLAY_MAIL = 4;
    static const uint8_t LOADING_SCREEN = 5;
    static const uint8_t SELECT_UNREAD = 1;
    static const uint8_t SELECT_SEND = 2;
    static const uint8_t SCREEN_WIDTH = 128; 
    static const uint8_t CHAR_WIDTH = 6;
    static const uint8_t CHAR_HEIGHT = 8;

    StaticJsonDocument<500> account_json;
    StaticJsonDocument<1000> email_json;
    //class variables storing email, message, etc.
    char user_email[50];
    char email_temp[50];
    char other[50];
    char message[1000];
    char prev_keyboard_input[3000];
    // state variables
    uint8_t mail_state;
    uint8_t select_state;
    uint8_t num_saved_account;
    uint8_t prev_selected;
    uint8_t prev_state;
    
    int unread_num;
    uint32_t notif_period;
    int timer;

    bool emailEntered;
    bool subjectEntered;
    bool login_failed;
    bool messageEntered;
    bool firstTime;
    bool more_unread;
    bool more_to_view;
    bool loading;
    bool checkingNum;
    bool sending;
    bool DISPLAYING;

    uint8_t msg_index;
    bool msg_display_updated;

    public:
        MailApp();
        void reset_screen();
        uint8_t handle_user_input();
        void draw_screen();
    private:
        void sendMail(char* recipient, char* subject, char* message);
        void checkUnreadMail();
        void checkUnreadNum();
        void getSavedAccounts();
        void loginEmail(char* account, char* password);
        void setUserEmail(uint8_t account_index);
        void mail_parsing(StaticJsonDocument<1000> &doc, int next_msg);
        void storeSavedAccounts();
        void draw_loading();
};

#endif
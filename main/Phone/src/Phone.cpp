#include "Arduino.h"
#include <memory>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include <EEPROM.h> // library for storing data in flash memory
#include "Phone.h"
#include "App/App.h"
// apps
#include "App/Apps/LoginApp/LoginApp.h"
#include "App/Apps/NotificationsApp/NotificationsApp.h"
#include "App/Apps/HomeApp/HomeApp.h"
#include "App/Apps/MessagingApp/MessagingApp.h"
#include "App/Apps/MailApp/MailApp.h"
#include "App/Apps/ClockApp/ClockApp.h"
#include "App/Apps/WeatherApp/WeatherApp.h"
#include "App/Apps/GameApp/GameApp.h"
#include "App/Apps/MusicApp/MusicApp.h"
#include "App/Apps/InternetApp/InternetApp.h"

Phone::Phone() {
    state = LOGIN; // default state is the home app
    setup(); // sets up the lcd screen, buttons, eeprom, and serial
    // apps
    login_app = std::unique_ptr<App>(new LoginApp());
    login_app->reset_screen();
    notifications_app = std::unique_ptr<NotificationsApp>(new NotificationsApp());
    home_app = std::unique_ptr<App>(nullptr);
    messaging_app = std::unique_ptr<App>(nullptr);
    mail_app = std::unique_ptr<App>(nullptr);
    clock_app = std::unique_ptr<App>(nullptr);
    weather_app = std::unique_ptr<App>(nullptr);
    game_app = std::unique_ptr<App>(nullptr);
    music_app = std::unique_ptr<App>(nullptr);
    internet_app = std::unique_ptr<App>(nullptr);
}

void Phone::setup() {
    // screen setup
    tft.init();  // init lcd screen
    tft.setRotation(2); // adjust rotation
    tft.fillScreen(TFT_WHITE); // fill background

    // serial setup
    Serial.begin(115200); // initialize serial to communicate with the serial monitor

    // eeprom setup
    if (!EEPROM.begin(76)) {
        Serial.println("failed to init EEPROM");
    } else {
        Serial.println("successfully init EEPROM");
    }

    // button setup
    pinMode(19, INPUT_PULLUP); // set IO pin 3, 5, and 19 as inputs which default to a 3.3V signal when unconnected and 0V when the switch is pushed
    pinMode(5, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
}

void Phone::loop() {
    uint8_t new_state;
    switch (state) {
        case LOGIN:
            login_app->get_user_input(); // get the button input values
            new_state = login_app->handle_user_input(); // handle the button input and return if the state has changed
            break;
        case HOME:
            home_app->get_user_input();
            new_state = home_app->handle_user_input();
            break;
        case MESSAGING:
            messaging_app->get_user_input();
            new_state = messaging_app->handle_user_input();
            break;
        case MAIL:
            mail_app->get_user_input();
            new_state = mail_app->handle_user_input();
            break;
        case CLOCK:
            clock_app->get_user_input();
            new_state = clock_app->handle_user_input();
            break;
        case WEATHER:
            weather_app->get_user_input();
            new_state = weather_app->handle_user_input();
            break;
        case GAME:
            game_app->get_user_input();
            new_state = game_app->handle_user_input();
            break;
        case MUSIC:
            music_app->get_user_input();
            new_state = music_app->handle_user_input();
            break;
        case INTERNET:
            internet_app->get_user_input();
            new_state = internet_app->handle_user_input();
            break;
        case SHOW_NOTIFICATION:
            notifications_app->get_user_input();
            new_state = notifications_app->handle_user_input();
            break;
        default:
            home_app->get_user_input();
            new_state = home_app->handle_user_input();
            break;
    }

    if (state != LOGIN) {
        if (notifications_app->check_notifications()) {
            previous_state = state;
            state = SHOW_NOTIFICATION;
        }
    }
    
    if (state != SHOW_NOTIFICATION) {
        if (new_state != NO_CHANGE && state != new_state) {
            state = new_state;
            switch (state) {
                case HOME:
                    if (home_app != nullptr) {
                        home_app->reset_screen();
                    }
                    break;
                case MESSAGING:
                    if (messaging_app != nullptr) {
                        messaging_app->reset_screen();
                    }
                    break;
                case MAIL:
                    if (mail_app != nullptr) {
                        mail_app->reset_screen();
                    }
                    break;
                case CLOCK:
                    if (clock_app != nullptr) {
                        clock_app->reset_screen();
                    }
                    break;
                case WEATHER:
                    if (weather_app != nullptr) {
                        weather_app->reset_screen();
                    }
                    break;
                case GAME:
                    if (game_app != nullptr) {
                        game_app->reset_screen();
                    }
                    break;
                case MUSIC:
                    if (music_app != nullptr) {
                        music_app->reset_screen();
                    }
                    break;
                case INTERNET:
                    if (internet_app != nullptr) {
                        internet_app->reset_screen();
                    }
                    break;
                default:
                    if (home_app != nullptr) {
                        home_app->reset_screen();
                    }
                    break;
            }
        }
    } else {
        if (new_state == REMOVE_NOTIFICATION) {
            state = previous_state;
            switch (state) {
                case HOME:
                    home_app->reset_screen();
                    break;
                case MESSAGING:
                    messaging_app->reset_screen();
                    break;
                case MAIL:
                    mail_app->reset_screen();
                    break;
                case CLOCK:
                    clock_app->reset_screen();
                    break;
                case WEATHER:
                    weather_app->reset_screen();
                    break;
                case GAME:
                    game_app->reset_screen();
                    break;
                case MUSIC:
                    music_app->reset_screen();
                    break;
                case INTERNET:
                    internet_app->reset_screen();
                    break;
                default:
                    home_app->reset_screen();
                    break;
            }
        }
    }

    switch (state) {
        case LOGIN:
            login_app->draw_screen(); // draw the screen based off of the app and its state machine
            break;
        case HOME:
            if (home_app == nullptr) {
                home_app = std::unique_ptr<App>(new HomeApp());
                home_app->reset_screen();
            }
            home_app->draw_screen();
            break;
        case MESSAGING:
            if (messaging_app == nullptr) {
                messaging_app = std::unique_ptr<App>(new MessagingApp());
                messaging_app->reset_screen();
            }
            messaging_app->draw_screen();
            break;
        case MAIL:
            if (mail_app == nullptr) {
                mail_app = std::unique_ptr<App>(new MailApp());
                mail_app->reset_screen();
            }
            mail_app->draw_screen();
            break;
        case CLOCK:
            if (clock_app == nullptr) {
                clock_app = std::unique_ptr<App>(new ClockApp());
                clock_app->reset_screen();
            }
            clock_app->draw_screen();
            break;
        case WEATHER:
            if (weather_app == nullptr) {
                weather_app = std::unique_ptr<App>(new WeatherApp());
                weather_app->reset_screen();
            }
            weather_app->draw_screen();
            break;
        case GAME:
            if (game_app == nullptr) {
                game_app = std::unique_ptr<App>(new GameApp());
                game_app->reset_screen();
            }
            game_app->draw_screen();
            break;
        case MUSIC:
            if (music_app == nullptr) {
                music_app = std::unique_ptr<App>(new MusicApp());
                music_app->reset_screen();
            }
            music_app->draw_screen();
            break;
        case INTERNET:
            if (internet_app == nullptr) {
                internet_app = std::unique_ptr<App>(new InternetApp());
                internet_app->reset_screen();
            }
            internet_app->draw_screen();
            break;
        case SHOW_NOTIFICATION:
            notifications_app->draw_screen();
            break;
        default:
            if (home_app == nullptr) {
                home_app = std::unique_ptr<App>(new HomeApp());
                home_app->reset_screen();
            }
            home_app->draw_screen();
            break;
    }
}
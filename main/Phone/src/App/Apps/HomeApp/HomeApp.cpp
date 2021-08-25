#include "Arduino.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Phone.h"
#include "App/App.h"
#include "HomeApp.h"
// app icons
#include "App/Apps/HomeApp/Images/clock.h"
#include "App/Apps/HomeApp/Images/game.h"
#include "App/Apps/HomeApp/Images/internet.h"
#include "App/Apps/HomeApp/Images/mail.h"
#include "App/Apps/HomeApp/Images/messaging.h"
#include "App/Apps/HomeApp/Images/music.h"
#include "App/Apps/HomeApp/Images/weather.h"

HomeApp::HomeApp() {  
    horizontal_select_state = 0;
    vertical_select_state = 0;
    current_app = 8;
}

uint8_t HomeApp::handle_user_input() {
  if (button_input_two == 1 || button_input_two == 2) {
    // vertical scroll
    if (vertical_select_state == 0) {
      vertical_select_state++;
    } else if (vertical_select_state == 1) {
      vertical_select_state++;
      horizontal_select_state = 0;
    } else {
      vertical_select_state = 0;
    }
  }
  if (button_input_one == 1 || button_input_one == 2) {
    // horizontal scroll
    if (vertical_select_state != 2 && horizontal_select_state < 2) {
      horizontal_select_state++;
    } else {
      horizontal_select_state = 0;
    }
  }
  current_app = 1 + horizontal_select_state + vertical_select_state*3;
  if (button_input_three == 1 || button_input_three == 2) {
    // enter an app
    return current_app; // switch to the app entered
  } else {
    return NO_CHANGE; // stay on the home app
  }
}

void HomeApp::draw_screen() {
    // first row
    if (current_app != MESSAGING) {
        tft.pushImage(8, 16, messaging_icon_width, messaging_icon_height, messaging_icon);
    } else {
        tft.pushImage(8, 16, messaging_icon_width, messaging_icon_height, messaging_icon_inverted);
    }
    if (current_app != MAIL) {
        tft.pushImage(48, 16, mail_icon_width, mail_icon_height, mail_icon);
    } else {
        tft.pushImage(48, 16, mail_icon_width, mail_icon_height, mail_icon_inverted);
    }
    if (current_app != CLOCK) {
        tft.pushImage(88, 16, clock_icon_width, clock_icon_height, clock_icon);
    } else {
        tft.pushImage(88, 16, clock_icon_width, clock_icon_height, clock_icon_inverted);
    }
    // second row
    if (current_app != WEATHER) {
        tft.pushImage(8, 64, weather_icon_width, weather_icon_height, weather_icon);
    } else {
        tft.pushImage(8, 64, weather_icon_width, weather_icon_height, weather_icon_inverted);
    }
    if (current_app != GAME) {
        tft.pushImage(48, 64, game_icon_width, game_icon_height, game_icon);
    } else {
        tft.pushImage(48, 64, game_icon_width, game_icon_height, game_icon_inverted);
    }
    if (current_app != MUSIC) {
        tft.pushImage(88, 64, music_icon_width, music_icon_height, music_icon);
    } else {
        tft.pushImage(88, 64, music_icon_width, music_icon_height, music_icon_inverted);
    }
    // third row
    if (current_app != INTERNET) {
        tft.pushImage(8, 112, internet_icon_width, internet_icon_height, internet_icon);
    } else {
        tft.pushImage(8, 112, internet_icon_width, internet_icon_height, internet_icon_inverted);
    }
}

void HomeApp::reset_screen() {
  tft.fillScreen(TFT_HOME_SCREEN);
}
#include "Arduino.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Phone.h"
#include "App/App.h"
#include "App/Inputs/Keyboard/Keyboard.h"
#include "InternetApp.h"

InternetApp::InternetApp() {  
    // background color
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // set color of font to white foreground, black background
    keyboard.reset_message();
}

uint8_t InternetApp::handle_user_input() {
    // your code here
    keyboard.handle_user_input(button_input_one, button_input_two, button_input_three);
    if (button_input_three == 2) {
        return HOME; // switch to the home app
    } else {
        return NO_CHANGE; // stay on the internet app
    }
}

void InternetApp::draw_screen() {
    tft.drawString("Internet app", 10, 10);
    keyboard.draw_screen();
}
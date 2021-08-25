/*
  WeatherApp.h - Library for the WeatherApp child class of App.
                 Allows users to see the current weather from anywhere in the world.
*/
#ifndef WeatherApp_h
#define WeatherApp_h
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Arduino.h"
#include "Phone.h"
#include "App/App.h"
#include <ArduinoJson.h>

class WeatherApp: public App {
	private:
		uint8_t SELECT_MENU;
		uint8_t SELECT_PLACE;
        uint8_t FAVORITES;
		uint8_t DISPLAY_INFO;
        uint8_t ERROR;
        uint8_t LOADING_SCREEN;
		uint8_t weather_state;
        uint8_t prev_state;
		char input[100];
        char city[100];
        char humidity[100];
        char weather[100];
        char temperature[100];
        char fave1[100];
        char fave2[100];
        char fave3[100];
        char fave4[100];
        char fave5[100];
        int change;
        int location_state;
        int prev_loc_state;
        char pointers_char[300];
        bool loading;
        bool gotWeather;
        bool parsed;
        bool local;
    	StaticJsonDocument<600> responsedoc;
    public:
        WeatherApp();
        uint8_t handle_user_input();
        void draw_screen();
        void reset_screen();
    private:
        void getWeather(char* input);
        void parseWeather(char* input, bool local);
        void printWeather();
        void parseResponse(char* response);
        void draw_loading();
        void getLocalWeather(double lat, double lon);
};

#endif
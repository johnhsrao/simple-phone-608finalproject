/*
  ClockApp.h - Library for the ClockApp child class of App.
               Allows users to view the current date and time from anywhere in the world.
               Set alarms and timers.
*/
#ifndef ClockApp_h
#define ClockApp_h
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Arduino.h"
#include "Phone.h"
#include "App/App.h"

class ClockApp: public App {
	private:
		const uint16_t GETTING_PERIOD = 60000; // periodicity of getting the current time
		uint8_t state; // home screen, favorites screen, search screen, help screen, or display screen
		uint8_t vertical_select_state; // scrolling vertically
		uint8_t horizontal_select_state; // scrolling horizontally
		bool digital; // display digital or display analog
		char search_text_box[19];
		bool show_keyboard;
		bool state_change;
		bool incorrect_input;
		bool loading; // http request sending
		void draw_digital(uint8_t digital_starting_y);
        void draw_analog();
		void draw_clock_face();
		void draw_hour_hand(uint8_t hour_type, int color);
		void draw_minute_second_hand(uint8_t minute_second_type, int color);
		// current time functions and variables
		void update_time();
		void update_timezone(int timezone);
		void deserialize_time_json();
		void deserialize_timezones_json();
		bool get_time_zone;
		bool get_local_time;
		// timezones
		int local_timezone;
		int favorite_timezones[5];
		char favorite_cities[5][19];
		int favorites_size;
		int search_timezone;
		uint32_t timer;  // used for storing millis() readings between calls
		// utc time readings
		int8_t last_hour;
		int8_t hour;
		int8_t utc_hour;
		uint8_t last_minute;
		uint8_t minute;
		uint8_t last_second;
		uint8_t second;
		uint8_t current_second;
		uint16_t millisecond;
    public:
        ClockApp();
        uint8_t handle_user_input();
        void draw_screen();
		void reset_screen();
};

#endif
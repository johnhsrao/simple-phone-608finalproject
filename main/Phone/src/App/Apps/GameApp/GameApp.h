/*
  GameApp.h - Library for the GameApp child class of App.
              Allows users to play a video game.
*/
#ifndef GameApp_h
#define GameApp_h
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Arduino.h"
#include "Phone.h"
#include "App/App.h"

class GameApp: public App {
	private: 
		int loc_y;
	    int loc_x;
	    int obs1_y;
	    int obs1_x;
		int old_locx;
		int old_locy;
	    int obs1_height;
	    int obs1_width;
	    int obs2_y;
	    int obs2_x;
	    int obs2_height;
	    int obs2_width;
	    int choose1;
	    int choose2;
	    int score;
	    bool done1;
	    bool done2;
	    char game_over[100];
	    bool over;
	    char scorestring[100];
		uint8_t timer;
		uint8_t SELECT_MENU;
		uint8_t LEADERBOARD;
		uint8_t PLAY_GAME;
		uint8_t game_state;
    	uint8_t prev_state;
		char first_place[100];
		char second_place[100];
		char third_place[100];
		const char delimiter[5] = ",";
		char *pch;
		char pointers_char[3];
		int counter;
		int loop_time;
		int playing_state;
		int menu_state;
		int prev_playing_state;
		int top_scores[3];
		char top_users[3][100];
		uint8_t diff_timer;
		bool first;
    public:
        GameApp();
        uint8_t handle_user_input();
        void draw_screen();
		void reset_screen();
    private: 
        void reset();
};

#endif
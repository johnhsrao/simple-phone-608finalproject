/*
  MusicApp.h - Library for the MusicApp child class of App.
               Allows users to listen to music.
*/
#ifndef MusicApp_h
#define MusicApp_h
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Arduino.h"
#include "Phone.h"
#include "App/App.h"
#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

class MusicApp: public App {
    uint8_t music_state;
    uint8_t prev_state;
    uint8_t MENU;
    uint8_t SELECT;
    uint8_t SONG;
    int song_count;
    char songs[10][20];
    int current_song;
    
    
    bool song_state;
    void setup_Speaker();
    public:
        MusicApp();
        uint8_t handle_user_input();
        void draw_screen();
};

#endif
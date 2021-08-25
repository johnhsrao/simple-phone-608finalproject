#include "Arduino.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Phone.h"
#include "App/App.h"
#include "MusicApp.h"
#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini myDFPlayer;

MusicApp::MusicApp() {  
    // background color
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // set color of font to white foreground, black background
    MENU = 0;
    SELECT = 1;
    SONG = 2;
    song_count = 0;
    music_state = 0;
    prev_state = music_state;
    current_song = 0;
    song_state = true;
    setup_Speaker();

}

uint8_t MusicApp::handle_user_input() {
    
    if (music_state == MENU){
        if(button_input_two > 0 || button_input_one > 0 || button_input_three == 1){
            song_count = myDFPlayer.readFileCounts();
            
            if (song_count > 10){
                song_count = 10;
            }
            for (int i = 0; i< song_count; i++){
                char printsong[10];
                sprintf(printsong, "TRACK %d", i);
                sprintf(songs[i], printsong);
            }
            music_state = SELECT;
            prev_state = MENU;
      }
    }
    else if (music_state == SELECT){
        if (button_input_one > 0){//for going back to the main menu
            music_state = MENU;
            prev_state = SELECT;
        }

        if (button_input_two > 0){
            current_song++;
            current_song %= song_count;
            
        }

        if (button_input_three == 1){
            myDFPlayer.play(current_song + 1);
            music_state = SONG;
            prev_state = SELECT;

        }
    }
    else if (music_state == SONG){
    	if(button_input_one > 0){ //pause and play button
    		if(song_state == true){
    			myDFPlayer.pause();
    			song_state = false;
    		}else{
    			myDFPlayer.start();
    			song_state = true; 
    		}
    	}
    	if(button_input_two == 1){
    		myDFPlayer.next(); //skip button
    		current_song ++;
    		current_song %= song_count;
        tft.drawString("          ", 50, 110);
        song_state = true;
    	}
    	if(button_input_two == 2){
    		myDFPlayer.previous(); // go back button
    		current_song --;
    		current_song %= song_count;
        tft.drawString("          ", 50, 110);
        song_state = true;
    	}
        if(button_input_three == 1){
            music_state = MENU;
            prev_state = SONG;
        }
    }
    if (button_input_three == 2) {
        myDFPlayer.pause();
        current_song = 0;
        music_state = MENU;
        return HOME; // switch to the home app
    } else {
        return NO_CHANGE; // stay on the music app
    }

}

void MusicApp::setup_Speaker(){
  mySoftwareSerial.begin(9600, SERIAL_8N1, 32, 33);  // speed, type, RX, TX
  Serial.begin(115200);
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  delay(1000);
  while (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    
    Serial.println(myDFPlayer.readType(),HEX);
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    //while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  
  //----Set volume----
  myDFPlayer.volume(8);  //Set volume value (0~30).
  myDFPlayer.volumeUp(); //Volume Up
  myDFPlayer.volumeDown(); //Volume Down

  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  int delayms=100;
  //----Read imformation----
  Serial.println(F("readState--------------------"));
  Serial.println(myDFPlayer.readState()); //read mp3 state
  Serial.println(F("readVolume--------------------"));
  Serial.println(myDFPlayer.readVolume()); //read current volume
  //Serial.println(F("readEQ--------------------"));
  //Serial.println(myDFPlayer.readEQ()); //read EQ setting
  Serial.println(F("readFileCounts--------------------"));
  Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
  Serial.println(F("readCurrentFileNumber--------------------"));
  Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
  Serial.println(F("readFileCountsInFolder--------------------"));
  Serial.println(myDFPlayer.readFileCountsInFolder(3)); //read fill counts in folder SD:/03
  Serial.println(F("--------------------"));

}

void MusicApp::draw_screen() {
    if(music_state == MENU){
      if(prev_state != MENU){
        tft.fillScreen(TFT_BLACK);
        prev_state = MENU;
      }
      
      tft.drawString("Welcome to the Music", 5, 30);
      tft.drawString("App", 60, 40);
      tft.drawString("Press any button to", 5, 80);
      tft.drawString("select a song", 5, 90);
    }
    else if(music_state == SELECT){
     
      if(prev_state != SELECT){
        tft.fillScreen(TFT_BLACK);
        prev_state = SELECT;
      }
      

      tft.drawString("Select Track:", 5, 5);
      int y = 25;
      for (int i = 0; i< song_count; i++){
        
        char printsong[20];
        
        
        sprintf(printsong, "TRACK %d       ", i + 1);
        Serial.println(printsong);
        if(i == current_song){
          
          sprintf(printsong, "TRACK %d   <--", i + 1);
        }
        tft.drawString(printsong, 5, y);
        y += 10;
      }
      

    }
    else if(music_state == SONG){
      if(prev_state != SONG){
        tft.fillScreen(TFT_BLACK);
        prev_state = SONG;
      }
      char songprint[50];
      char track[10];
      sprintf(songprint, "Currently Playing:");
      sprintf(track, "TRACK %d   ", current_song + 1);


      tft.drawString(songprint, 5, 15);
      tft.drawString(track, 5, 25);
      tft.drawString("Press Button 1 to", 5, 50);
      tft.drawString("Pause and Resume",5, 60);
      tft.drawString("Press Button 2 to", 5, 80);
      tft.drawString("Skip and Reverse", 5, 90);
      if(song_state == false){
        tft.drawString("Paused", 50, 110);
      }else{
        tft.drawString("          ", 50, 110);
      }
    }

    }

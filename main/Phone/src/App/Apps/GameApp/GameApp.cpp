#include "Arduino.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Phone.h"
#include "App/App.h"
#include "GameApp.h"
#include "math.h"
#include "stdlib.h"
#include <ArduinoJson.h>

#include "App/Apps/GameApp/game_images/bird.h"
#include "App/Apps/GameApp/game_images/volley_image.h"
#include "App/Apps/GameApp/game_images/volleyball_image.h"

GameApp::GameApp() {  
    // background color
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // set color of font to black foreground, white background
    loc_y = 82;
    loc_x = 30;
    obs1_x = 127;
    obs2_x = 191;
	old_locx = loc_x;
	old_locy = loc_y;
    choose1 = random(2);
    choose2 = random(2);
	game_state = 0;
    prev_state = 0;
    score = 0;
    done1 = false;
    done2 = false;
	timer = 0;
	SELECT_MENU = 0;
    LEADERBOARD = 1; 
    PLAY_GAME = 2;
	prev_state = 4;
	game_state = SELECT_MENU;
    game_over[100] = {NULL};
    scorestring[100] = {NULL};
	first_place[100] = {NULL};
	second_place[100] = {NULL};
	third_place[100] = {NULL};
	counter = 0;
	loop_time = 50;
	playing_state = 0;
	menu_state = 0;
	first = false;
	
    if (choose1 == 0) {
    	obs1_y = random(82) + 82;
    	obs1_height = volley_height;
    	obs1_width = volley_width;
    } else {
    	obs1_y = random(82);
    	obs1_height = bird_height;
    	obs1_width = bird_width;
    }
    if (choose2 == 0) {
    	obs2_y = random(82) + 82;
    	obs2_height = volley_height;
    	obs2_width = volley_width;
    } else {
    	obs2_y = random(82);
    	obs2_height = bird_height;
    	obs2_width = bird_width;
    }
}

uint8_t GameApp::handle_user_input() {
	if (game_state == SELECT_MENU){
		if (button_input_two > 0){
			game_state = LEADERBOARD;
			prev_state = SELECT_MENU;
		} else if (button_input_one > 0){
			game_state = PLAY_GAME;
			prev_state = SELECT_MENU;
			playing_state = 0;
			menu_state = 0;
		}
	} else if (game_state == LEADERBOARD){
		
		if (button_input_two > 0){
			game_state = SELECT_MENU;
			prev_state = LEADERBOARD;
			first = false;
		}
	} else if (game_state == PLAY_GAME){
		if (playing_state == 0) {
			if (menu_state >= 4) {
				if (button_input_two > 0) {
					menu_state = 0;
				}
			} else {
				if (button_input_two > 0) {
					menu_state += 1;
				}
			}
			if (button_input_three == 1){
				playing_state = 1;
				loop_time = 50 - (10*menu_state);
			}
		} else {

			if (button_input_two == 1) {
				if (loc_y >= 20) {
					loc_y -= 20;
				} else {
					loc_y = 0;
				}
			}
			obs1_x -= 1;
			obs2_x -= 1;
			if (obs1_x <= -obs1_width) {
				choose1 = random(2);
				if (choose1 <= 0) {
					obs1_y = random(82) + 82;
					obs1_height = volley_height;
					obs1_width = volley_width;
				} else {
					obs1_y = random(82);
					obs1_height = bird_height;
					obs1_width = bird_width;
				}
				obs1_x = 127;
			}
			if (obs2_x <= -obs2_width) {
				choose2 = random(2);
				if (choose2 <= 0) {
					obs2_y = random(82) + 82;
					obs2_height = volley_height;
					obs2_width = volley_width;
				} else {
					obs2_y = random(82);
					obs2_height = bird_height;
					obs2_width = bird_width;
				}
				obs2_x = 127;
			}
			loc_y += 1;
			if (button_input_three == 1) {
				playing_state = 0;
				prev_playing_state = 1;
				menu_state = 0;
				over = false;
				loc_y = 82;
				loc_x = 64;
				obs1_y;
				obs1_x = 127;
				obs1_height;
				obs1_width;
				obs2_y;
				obs2_x = 191;
				obs2_height;
				obs2_width;
				choose1 = random(2);
				choose2 = random(2);
				memset(game_over, 0, strlen(game_over));
				if (choose1 <= 0.5) {
					obs1_y = random(82) + 82;
					obs1_height = volley_height;
					obs1_width = volley_width;
				} else {
					obs1_y = random(82);
					obs1_height = bird_height;
					obs1_width = bird_width;
				}
				if (choose2 <= 0.5) {
					obs2_y = random(82) + 82;
					obs2_height = volley_height;
					obs2_width = volley_width;
				} else {
					obs2_y = random(82);
					obs2_height = bird_height;
					obs2_width = bird_width;
				}
				score = 0;
				tft.fillScreen(TFT_WHITE);
			}
			if (button_input_one > 0){
				game_state = SELECT_MENU;
				prev_state = PLAY_GAME;
				playing_state = 0;
				prev_playing_state = 1;
				menu_state = 0;
				tft.fillScreen(TFT_WHITE);
			}
		}
	}
    if (button_input_three == 2) {
        return HOME; // switch to the home app
    } else {
        return NO_CHANGE; // stay on the game app
    }
}

void GameApp::draw_screen() {
	if (game_state == SELECT_MENU){
		if (prev_state != SELECT_MENU){
			tft.fillScreen(TFT_BLACK);
			prev_state = SELECT_MENU;
		}
		tft.drawString("Game App", 30, 5);
        tft.drawString("Press button 2 to", 10, 20);
        tft.drawString("see the leaderboard", 10, 30);
        tft.drawString("Press button 1 to", 10, 50);
        tft.drawString("play a game", 10, 60);

	} else if (game_state == LEADERBOARD){
		if (prev_state != LEADERBOARD){
			tft.fillScreen(TFT_BLACK);
			tft.drawString("LEADERBOARD", 5, 10);
			prev_state = LEADERBOARD;
			
			http_request("GET", "", "http://608dev-2.net/sandbox/sc/team28/final_project/python/GamesApp/score_database.py", "608dev-2.net");

		}
		if (request_sending == false and first == false) {
			StaticJsonDocument<1000> doc;
			DeserializationError error = deserializeJson(doc, response);

			// Test if parsing succeeds.
			if (error) {
				Serial.print(("deserializeJson() failed: "));
				Serial.println(error.f_str());
				return;
			}

			tft.setCursor(5, 20);
			char user1[30] = {NULL};
			strcpy(user1, doc["user1"]);
			char user2[30] = {NULL};
			strcpy(user2, doc["user2"]);
			char user3[30] = {NULL};
			strcpy(user3, doc["user3"]);
			int score1 = (int) doc["score1"];
			int score2 = (int) doc["score2"];
			int score3 = (int) doc["score3"];
			tft.printf("%s: %d\n", user1, score1);
			tft.setCursor(5, 30);
			tft.printf("%s: %d\n", user2, score2);
			tft.setCursor(5, 40);
			tft.printf("%s: %d\n", user3, score3);
			// uint8_t i = 0;
			// while (i < 3) {
			// 	sprintf(top_users[i], doc["users"][i]);
			// 	top_scores[i] = doc["scores"][i];
			// 	i++;
			// 	tft.printf("%s: %d\n", top_users[i], top_scores[i]);
			// }
			first = true;
		}
		
		
	} else if (game_state == PLAY_GAME){
		if (prev_state != PLAY_GAME){
			over = false;
			loc_y = 82;
			loc_x = 64;
			obs1_y;
			obs1_x = 127;
			obs1_height;
			obs1_width;
			obs2_y;
			obs2_x = 191;
			obs2_height;
			obs2_width;
			choose1 = random(2);
			choose2 = random(2);
			memset(game_over, 0, strlen(game_over));
			if (choose1 <= 0.5) {
				obs1_y = random(82) + 82;
				obs1_height = volley_height;
				obs1_width = volley_width;
			} else {
				obs1_y = random(82);
				obs1_height = bird_height;
				obs1_width = bird_width;
			}
			if (choose2 <= 0.5) {
				obs2_y = random(82) + 82;
				obs2_height = volley_height;
				obs2_width = volley_width;
			} else {
				obs2_y = random(82);
				obs2_height = bird_height;
				obs2_width = bird_width;
			}
			score = 0;
			tft.fillScreen(TFT_WHITE);
			prev_state = PLAY_GAME;
		}

		if (playing_state == 0) {
			tft.setTextColor(TFT_BLACK, TFT_WHITE);
			tft.drawString("Level 1", 5, 10);
			tft.drawString("Level 2", 5, 20);
			tft.drawString("Level 3", 5, 30);
			tft.drawString("Level 4", 5, 40);
			tft.drawString("Level 5", 5, 50);
			tft.drawCircle(100, menu_state*10 + 8, 2, TFT_BLACK);
			if (menu_state > 0) {
				tft.drawCircle(100, menu_state*10 - 2, 2, TFT_WHITE);
			} else {
				tft.drawCircle(100, 48, 2, TFT_WHITE);
			}
			
			prev_playing_state = 0;

		} else {
			if (prev_playing_state == 0){
				tft.fillScreen(TFT_WHITE);
				prev_playing_state = 1;
			}
			if (!over && millis() - timer > loop_time) {
				delay(loop_time);
				tft.fillRect(obs2_x + obs2_width, obs2_y, 1, obs2_height, TFT_WHITE); 
				tft.fillRect(obs1_x + obs1_width, obs1_y, 1, obs1_height, TFT_WHITE);
				
				if (old_locy - loc_y > 0){
					tft.fillRect(loc_x, loc_y + volleyball_height, volleyball_width, 20, TFT_WHITE);
				} else {
					tft.fillRect(old_locx, old_locy, volleyball_width, 1, TFT_WHITE);
				}
				tft.pushImage(loc_x, loc_y, volleyball_width, volleyball_height, volleyball);
				

				if (choose1 <= 0.5) {
					tft.pushImage(obs1_x, obs1_y, volley_width, volley_height, volleynet);
					
				} else {
					tft.pushImage(obs1_x, obs1_y, bird_width, bird_height, bird);
				}
				if (choose2 <= 0.5) {
					tft.pushImage(obs2_x, obs2_y, volley_width, volley_height, volleynet);
				} else {
					tft.pushImage(obs2_x, obs2_y, bird_width, bird_height, bird);
				}
				timer = millis();
				
				if (loc_x < obs1_x) { // ball before obstacle 1
					if (loc_x + volleyball_width - 6 > obs1_x) {
						if (choose1 == 0) {
							if (loc_y + volleyball_height - 6 > obs1_y) {
								over = true;
								reset();
							}
						} else {
							if (loc_y > obs1_y) {
								if (obs1_y + obs1_height - 6 > loc_y) {
									over = true;
									reset();
								}
							} else {
								if (loc_y + volleyball_height - 6 > obs1_y) {
									over = true;
									reset();
								}
							}
						}
						if (!done1) {
							score += 1;
						}
						done1 = true;
					} else {
						done1 = false;
					}
				} else { // ball at or after obstacle 1
					if (loc_x < obs1_x + obs1_width) { // ball at obstacle
						if (choose1 == 0) {
							if (loc_y + volleyball_height - 6 > obs1_y) {
								over = true;
								reset();
							}
						} else {
							if (loc_y > obs1_y) {
								if (obs1_y + obs1_height - 6 > loc_y) {
									over = true;
									reset();
								}
							} else {
								if (loc_y + volleyball_height - 6 > obs1_y) {
									over = true;
									reset();
								}
							}
						}
						if (!done1) {
							score += 1;
						}
						done1 = true;
					} else {
						done1 = false;
					}
				}

				if (loc_x < obs2_x) { // ball before obstacle 1
					if (loc_x + volleyball_width - 6 > obs2_x) {
						if (choose2 == 0) {
							if (loc_y + volleyball_height - 6 > obs2_y) {
								over = true;
								reset();
							}
						} else {
							if (loc_y > obs2_y) {
								if (obs2_y + obs2_height - 6 > loc_y) {
									over = true;
									reset();
								}
							} else {
								if (loc_y + volleyball_height - 6 > obs2_y) {
									over = true;
									reset();
								}
							}
						}
						if (!done2) {
							score += 1;
						}
						done2 = true;
					} else {
						done2 = false;
					} 
				} else { // ball at or after obstacle 1
					if (loc_x < obs2_x + obs2_width) { // ball at obstacle
						if (choose2 == 0) {
							if (loc_y + volleyball_height - 6 > obs2_y) {
								over = true;
								reset();
							}
						} else {
							if (loc_y > obs2_y) {
								if (obs2_y + obs2_height - 6 > loc_y) {
									over = true;
									reset();
								}
							} else {
								if (loc_y + volleyball_height - 6 > obs2_y) {
									over = true;
									reset();
								}
							}
						}
						if (!done2) {
							score += 1;
						}
						done2 = true;
					} else {
						done2 = false;
					}
				}

				if (loc_y >= 164) {
					over = true;
					reset();
				}
			}
			if (over) {
				reset();
			}
			old_locx = loc_x;
			old_locy = loc_y; 
		}
	}
}

void GameApp::reset_screen() {
	if (game_state == PLAY_GAME) {
		tft.fillScreen(TFT_WHITE);
	} else {
		tft.fillScreen(TFT_BLACK);
	}
}

void GameApp::reset() {
	sprintf(game_over, "GAME OVER\nSCORE: %d", score);
	
    if (playing_state == 1 and request_sending == false) {
    	tft.fillScreen(TFT_BLACK);
		tft.setTextSize(1);
	    tft.setTextColor(TFT_WHITE, TFT_BLACK); 
	    tft.drawString(game_over, 5, 75);
    	memset(scorestring, 0, strlen(scorestring));
    	sprintf(scorestring, "score=%d&user=%s&level=%d", score, username, loop_time);
    	http_request("POST", scorestring, "http://608dev-2.net/sandbox/sc/team28/final_project/python/GamesApp/score_database.py", "608dev-2.net", false, false);
    	playing_state = 2;
    }
    if (playing_state == 2){
    	if (request_sending == false){
		    tft.setCursor(5, 85);
		    tft.println(response);
		}
    }
    loc_y = 82;
    loc_x = 64;
	old_locy = loc_y;
	old_locx = loc_x;
    obs1_y;
    obs1_x = 127;
    obs1_height;
    obs1_width;
    obs2_y;
    obs2_x = 191;
    obs2_height;
    obs2_width;
    choose1 = random(2);
    choose2 = random(2);
    memset(game_over, 0, strlen(game_over));
    if (choose1 <= 0.5) {
    	obs1_y = random(82) + 82;
    	obs1_height = volley_height;
    	obs1_width = volley_width;
    } else {
    	obs1_y = random(82);
    	obs1_height = bird_height;
    	obs1_width = bird_width;
    }
    if (choose2 <= 0.5) {
    	obs2_y = random(82) + 82;
    	obs2_height = volley_height;
    	obs2_width = volley_width;
    } else {
    	obs2_y = random(82);
    	obs2_height = bird_height;
    	obs2_width = bird_width;
    }
}
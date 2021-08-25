#include "Arduino.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include <string.h>
#include "Phone.h"
#include <ArduinoJson.h> // Library to deserialize json
#include "App/App.h"
#include "ClockApp.h"

ClockApp::ClockApp() {  
    digital = true;
	state = 0; // home screen
	vertical_select_state = 0;
    horizontal_select_state = 0;
    state_change = true;
    show_keyboard = false;
	incorrect_input = false;
    loading = true;
	get_time_zone = true; // update_time will send a request to get utc and time zone
	get_local_time = true; // uses the lat and lon
	favorites_size = 0;
	sprintf(search_text_box, "");
}

uint8_t ClockApp::handle_user_input() {
	if (!show_keyboard) {
		if (state == 0) {
			// home screen
			if (button_input_two == 1 || button_input_two == 2) {
				// vertical scroll
				if (vertical_select_state < 2) {
					vertical_select_state++;
				} else {
					vertical_select_state = 0;
				}
				state_change = true;
			}
			if (button_input_three == 1) {
				// click the favorites, search, or help buttons
				if (vertical_select_state == 0) {
					// switch to favorites
					state = 1;
					vertical_select_state = 0;
					if (favorites_size == 0) {
						char destination[105];
						sprintf(destination, "http://608dev-2.net/sandbox/sc/team28/final_project/python/ClockApp/favorites.py?user=%s", username);
						http_request(
							"GET",
							"",
							destination,
							"608dev-2.net"
						);
						loading = true;
					}
				} else if (vertical_select_state == 1) {
					// switch to search
					state = 2;
					vertical_select_state = 0;
					keyboard.reset_message();
				} else {
					// switch to help
					state = 3;
				}
				state_change = true;
				reset_screen();
			}
		} else if (state == 1) {
			// favorites screen
			if (button_input_two == 1 || button_input_two == 2) {
				// vertical scroll
				if (vertical_select_state < favorites_size) {
					vertical_select_state++;
				} else {
					vertical_select_state = 0;
				}
				state_change = true;
			}
			if (vertical_select_state == favorites_size) {
				if (button_input_one == 1 || button_input_one == 2) {
					// horizontal scroll
					if (horizontal_select_state < 2) {
						horizontal_select_state++;
					} else {
						horizontal_select_state = 0;
					}
					state_change = true;
				}
			}
			if (button_input_three == 1) {
				if (vertical_select_state < favorites_size) {
					// view favorite city
					state = 4;
					update_timezone(favorite_timezones[vertical_select_state]);
				} else {
					if (horizontal_select_state == 0) {
						// go back to home screen
						state = 0;
						incorrect_input = false;
						show_keyboard = false;
						sprintf(search_text_box, "");
						vertical_select_state = 0;
						horizontal_select_state = 0;
						update_timezone(local_timezone);
					} else if (horizontal_select_state == 1) {
						// delete favorite city
					} else {
						// add favorite city
					}
				}
				state_change = true;
				reset_screen();
			}
		} else if (state == 2) {
			// search screen
			if (button_input_two == 1 || button_input_two == 2) {
				// vertical scroll
				if (vertical_select_state < 1) {
					vertical_select_state++;
				} else {
					vertical_select_state = 0;
				}
				state_change = true;
			}
			if (vertical_select_state == 1) {
				if (button_input_one == 1 || button_input_one == 2) {
					// horizontal scroll
					if (horizontal_select_state < 1) {
						horizontal_select_state++;
					} else {
						horizontal_select_state = 0;
					}
					state_change = true;
				}
			}
			if (button_input_three == 1) {
				if (vertical_select_state == 0) {
					// enter text into box
					show_keyboard = true;
				} else {
					// submit info
					if (horizontal_select_state == 0) {
						// go back to home screen
						state = 0;
						get_local_time = true; // uses the lat and lon
						incorrect_input = false;
						sprintf(search_text_box, "");
						vertical_select_state = 0;
						horizontal_select_state = 0;
					} else {
						get_local_time = false; // uses user text input
					}
					get_time_zone = true; // update_time will send a request to get utc and time zone
					show_keyboard = false;
					loading = true;
					reset_screen();
				}
				state_change = true;
			}
		} else if (state == 3) {
			// help screen
			if (button_input_two == 1 || button_input_two == 2) {
				// go back to home screen
				state = 0;
				state_change = true;
				reset_screen();
			}
		} else {
			// display screen
			if (button_input_two == 1 || button_input_two == 2) {
				// go back to home screen
				state = 0;
				sprintf(search_text_box, "");
				vertical_select_state = 0;
				horizontal_select_state = 0;
				digital = true;
				incorrect_input = false;
				show_keyboard = false;
				state_change = true;
				update_timezone(local_timezone);
				reset_screen();
			}
			if (button_input_one == 1 || button_input_one == 2) {
				// switch between analog and digital
				digital = !digital;
				state_change = true;
				reset_screen();
			}
		}
	} else {
		if (button_input_one != 0 || button_input_two != 0 || button_input_three != 0) {
			state_change = true;
		}
		bool keyboard_enter = keyboard.handle_user_input(button_input_one, button_input_two, button_input_three);
		strncpy(search_text_box, keyboard.get_current_message(), 18);
		if (keyboard_enter) {
			keyboard.reset_message();
			show_keyboard = false;
			reset_screen();
		}
	}

	// update time if necessary
	update_time();

	if (loading && !request_sending) {
		state_change = true;
		loading = false;
		if (state == 0 || state == 2) {
			deserialize_time_json();
			if (state == 2) {
				if (!incorrect_input) {
					state = 4;
				}
			}
			timer = millis() - millisecond;
		} else {
			deserialize_timezones_json();
		}
		reset_screen();
	}

    if (button_input_three == 2) {
        return HOME; // switch to the home app
    } else {
        return NO_CHANGE; // stay on the clock app
    }
}

void ClockApp::reset_screen() {
	tft.fillScreen(TFT_HOME_SCREEN);
	state_change = true;
}

void ClockApp::draw_screen() {
	if (state_change) {
		if (loading) {
			if (state == 0) {
				// getting local time
				loading_animation.set_dimensions(64, 38, 12);
			} else {
				// getting favorites OR time for city searched
				loading_animation.set_dimensions(64, 80, 12);
			}
		}

		if (state == 0) {
			// select favorites, search, or help
			tft.setTextSize(1);
			tft.setTextColor(TFT_WHITE, TFT_HOME_SCREEN); // set color of font to white foreground, blue background
			tft.drawString("local time:", 31, 8);

			tft.setTextSize(2);
			char *options_array[3] = {"favorites", "search", "help"};
			for (int i = 0; i < 3; i++) {
				if (i == vertical_select_state) {
					tft.fillRect(8, 60+34*i, 112, 24, TFT_WHITE);
					tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
					tft.drawString(options_array[i], 64-strlen(options_array[i])*6, 64+34*i);
				} else {
					tft.fillRect(8, 60+34*i, 112, 24, TFT_BLACK);
					tft.setTextColor(TFT_WHITE, TFT_BLACK); // set color of font to white foreground, black background
					tft.drawString(options_array[i], 64-strlen(options_array[i])*6, 64+34*i);
				}
			}
		} else if (state == 1) {
			if (!loading) {
				// favorite cities and their current times
				tft.setTextSize(1);
				for (int i = 0; i < favorites_size; i++) {
					if (i == vertical_select_state) {
						tft.fillRect(8, 12+24*i, 112, 12, TFT_WHITE);
						tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
						tft.drawString(favorite_cities[i], 60-strlen(favorite_cities[i])*3, 14+24*i);
					} else {
						tft.fillRect(8, 12+24*i, 112, 12, TFT_BLACK);
						tft.setTextColor(TFT_WHITE, TFT_BLACK); // set color of font to white foreground, black background
						tft.drawString(favorite_cities[i], 60-strlen(favorite_cities[i])*3, 14+24*i);
					}
				}

				tft.fillRect(2, 132, 40, 12, TFT_WHITE);
				tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
				tft.drawString("back", 2+8, 132+2);

				tft.fillRect(44, 132, 40, 12, TFT_WHITE);
				tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
				tft.drawString("delete", 44+2, 132+2);

				tft.fillRect(86, 132, 40, 12, TFT_WHITE);
				tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
				tft.drawString("add", 86+11, 132+2);

				if (vertical_select_state == favorites_size) {
					if (horizontal_select_state == 0) {
						tft.drawRect(1, 131, 42, 14, TFT_BLACK);
						tft.drawRect(43, 131, 42, 14, TFT_HOME_SCREEN);
						tft.drawRect(85, 131, 42, 14, TFT_HOME_SCREEN);
					} else if (horizontal_select_state == 1) {
						tft.drawRect(1, 131, 42, 14, TFT_HOME_SCREEN);
						tft.drawRect(43, 131, 42, 14, TFT_BLACK);
						tft.drawRect(85, 131, 42, 14, TFT_HOME_SCREEN);
					} else {
						tft.drawRect(1, 131, 42, 14, TFT_HOME_SCREEN);
						tft.drawRect(43, 131, 42, 14, TFT_HOME_SCREEN);
						tft.drawRect(85, 131, 42, 14, TFT_BLACK);
					}
				} else {
					tft.drawRect(1, 131, 42, 14, TFT_HOME_SCREEN);
					tft.drawRect(43, 131, 42, 14, TFT_HOME_SCREEN);
					tft.drawRect(85, 131, 42, 14, TFT_HOME_SCREEN);
				}
			}
		} else if (state == 2) {
			if (!loading) {
				// lookup a city
				tft.setTextSize(1);
				tft.setTextColor(TFT_WHITE, TFT_HOME_SCREEN); // set color of font to white foreground, blue background
				tft.drawString("city:", 8, 21);
				tft.setTextColor(TFT_RED, TFT_HOME_SCREEN); // set color of font to red foreground, blue background
				if (incorrect_input) {
					tft.drawString("incorrect", 68, 21);
				}

				tft.fillRect(8, 36, 112, 12, TFT_WHITE);
				tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
				tft.drawString(search_text_box, 10, 36+2);

				tft.fillRect(16, 69, 40, 12, TFT_WHITE);
				tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
				tft.drawString("back", 16+8, 69+2);

				tft.fillRect(72, 69, 40, 12, TFT_WHITE);
				tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
				tft.drawString("submit", 72+2, 69+2);

				if (vertical_select_state == 0) {
					tft.drawRect(7, 35, 114, 14, TFT_BLACK);
					tft.drawRect(15, 68, 42, 14, TFT_HOME_SCREEN);
					tft.drawRect(71, 68, 42, 14, TFT_HOME_SCREEN);
				} else {
					tft.drawRect(7, 35, 114, 14, TFT_HOME_SCREEN);
					if (horizontal_select_state == 0) {
						tft.drawRect(15, 68, 42, 14, TFT_BLACK);
						tft.drawRect(71, 68, 42, 14, TFT_HOME_SCREEN);
					} else {
						tft.drawRect(71, 68, 42, 14, TFT_BLACK);
						tft.drawRect(15, 68, 42, 14, TFT_HOME_SCREEN);
					}
				}

				if (show_keyboard) {
					keyboard.draw_screen();
				}
			}
		} else if (state == 3) {
			// help info
			tft.setTextSize(1);
			tft.setTextColor(TFT_RED, TFT_HOME_SCREEN);
			tft.drawString("Home, Favs, Search", 7, 15);
			tft.setTextColor(TFT_WHITE, TFT_HOME_SCREEN);
			tft.drawString("button 5: y scroll", 7, 25);
			tft.drawString("button 19: x scroll", 7, 35);
			tft.drawString("button 3: enter", 7, 45);

			tft.setTextColor(TFT_RED, TFT_HOME_SCREEN);
			tft.drawString("Help", 7, 70);
			tft.setTextColor(TFT_WHITE, TFT_HOME_SCREEN);
			tft.drawString("button 5: home", 7, 80);

			tft.setTextColor(TFT_RED, TFT_HOME_SCREEN);
			tft.drawString("Clock", 7, 105);
			tft.setTextColor(TFT_WHITE, TFT_HOME_SCREEN);
			tft.drawString("button 5: home", 7, 115);
			tft.drawString("button 19: analog", 7, 125);
			tft.drawString("to digital", 7, 135);
		} else {
			// initial render of the clock screen
			if (!digital) {
				draw_clock_face();
			}
		}
		state_change = false;
	}

	if (state == 0) {
		// constantly redraw the clock
		if (!loading) {
			// display local time
			draw_digital(26);
		}
	}

	if (state == 4) {
		// constantly redraw the clock
		if (digital) {
			draw_digital(65);
		} else {
			draw_analog();
		}
	}

	if (loading) {
		if (state == 0) {
			// getting local time
			loading_animation.draw_screen();
		} else if (state == 1) {
			// getting favorites
			loading_animation.draw_screen();
		} else {
			// getting time for city searched
			loading_animation.draw_screen();
		}
	}
}

void ClockApp::draw_digital(uint8_t digital_starting_y) {
	tft.setTextSize(3);
  	tft.setTextColor(TFT_RED, TFT_HOME_SCREEN); // set color of font to red foreground, blue background
	
	uint8_t digital_starting_x;
	char time_of_day[3];
	char digital_hour_minute[6];
	char digital_second[3];

	if (hour == 0) {
		if (minute < 10) {
			sprintf(digital_hour_minute, "12:0%d", minute);
		} else {
			sprintf(digital_hour_minute, "12:%d", minute);
		}
		sprintf(time_of_day, "AM");
	} else if (hour < 12) {
		if (minute < 10) {
			sprintf(digital_hour_minute, "%d:0%d", hour, minute);
		} else {
			sprintf(digital_hour_minute, "%d:%d", hour, minute);
		}
		sprintf(time_of_day, "AM");
	} else {
		if (minute < 10) {
			sprintf(digital_hour_minute, "%d:0%d", hour-12, minute);
		} else {
			sprintf(digital_hour_minute, "%d:%d", hour-12, minute);
		}
		sprintf(time_of_day, "PM");
	}

	if (hour == 0 || hour == 10 || hour == 11 || hour == 12 || hour == 22 || hour == 23) {
		digital_starting_x = 7;
	} else {
		digital_starting_x = 25;
	}
	if (current_second < 10) {
		sprintf(digital_second, "0%d", current_second);
	} else {
		sprintf(digital_second, "%d", current_second);
	}

	tft.drawString(digital_hour_minute, digital_starting_x, digital_starting_y);
	tft.setTextSize(1);
	tft.drawString(time_of_day, 108, digital_starting_y);
	tft.drawString(digital_second, 108, digital_starting_y+14);
}

void ClockApp::draw_analog() {
	// erase old hour, minute, second hands as needed
	if (hour != last_hour) {
		draw_hour_hand(last_hour, TFT_WHITE);
	}
	if (minute != last_minute) {
		draw_minute_second_hand(last_minute, TFT_WHITE);
	}
	if (current_second != last_second) {
		draw_minute_second_hand(last_second, TFT_WHITE);
	}

	// draw new hour, minute, second hands
	draw_hour_hand(hour, TFT_BLACK);
	draw_minute_second_hand(minute, TFT_BLACK);
	draw_minute_second_hand(current_second, TFT_RED);
}

void ClockApp::draw_clock_face() {
	// draw clock background
	tft.fillCircle(64, 80, 50, TFT_WHITE);

	// draw small dot at the center of the clock
	tft.fillCircle(64, 80, 2, TFT_BLACK);

	// set text size/color
	tft.setTextSize(2);
	tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background

	// draw numbers on the clock including 12, 3, 6, 9
	tft.drawString("12", 52, 36);
	tft.drawString("3", 96, 72);
	tft.drawString("6", 58, 113);
	tft.drawString("9", 20, 72);

	// draw two small lines between each number on the clock
	for (uint8_t i = 0; i < 12; i++) {
		if (i != 0 && i != 3 && i != 6 && i != 9) {
			tft.drawLine(64 + 45*cos(i*PI/6), 
						80 - 45*sin(i*PI/6), 
						64 + 50*cos(i*PI/6), 
						80 - 50*sin(i*PI/6), 
						TFT_BLACK);
		}
	}
}

void ClockApp::draw_hour_hand(uint8_t hour_type, int color) {
	uint8_t hour_radians;
	if (hour_type >= 12) {
		hour_radians = 11 - ((hour_type - 12) - 4);
	} else {
	if (hour_type <= 3) {
		hour_radians = 3 - hour_type;
	} else {
		hour_radians = 11 - (hour_type - 4);
	}
	}
	tft.drawLine(64, 
				80, 
				64 + 20*cos(hour_radians*PI/6), 
				80 - 20*sin(hour_radians*PI/6), 
				color);
}

void ClockApp::draw_minute_second_hand(uint8_t minute_second_type, int color) {
	uint8_t minute_second_radians;
	if (minute_second_type <= 15) {
		minute_second_radians = 15 - minute_second_type;
	} else {
		minute_second_radians = 59 - (minute_second_type - 16);
	}
	tft.drawLine(64, 
				80, 
				64 + 28*cos(minute_second_radians*PI/30), 
				80 - 28*sin(minute_second_radians*PI/30), 
				color);
}

void ClockApp::update_time() {
	// get new current time
	if (millis() - timer >= GETTING_PERIOD || get_time_zone) {
		char destination[125];
		if (get_time_zone) {
			// get current utc time and timezone
			if (get_local_time) {
				sprintf(destination, "http://608dev-2.net/sandbox/sc/team28/final_project/python/ClockApp/get_current_time.py?lat=%f&lon=%f", lat, lon);
			} else {
				sprintf(destination, "http://608dev-2.net/sandbox/sc/team28/final_project/python/ClockApp/get_current_time.py?city=%s", search_text_box);
			}
			get_time_zone = false;
		} else {
			// get current utc time
			sprintf(destination, "http://608dev-2.net/sandbox/sc/team28/final_project/python/ClockApp/get_current_time.py");
		}
		http_request(
			"GET",
			"",
			destination,
			"608dev-2.net"
		);
	}

	// update second and if necessary, minute
	last_hour = hour;
	last_minute = minute;
	last_second = current_second;
	if ((millis()-timer)/1000 + second >= 60) {
		minute += 1;
		second = 0;
		current_second = second;
		timer = millis();
	} else {
		current_second = (millis()-timer)/1000 + second;
	}

	// update minute if necessary
	if (minute >= 60) {
		hour += 1;
		minute = 0;
	}

	// update hour if necessary
	if (hour >= 24) {
		hour = 0;
	}
}

void ClockApp::deserialize_timezones_json() {
	// Deserialize the JSON document
	StaticJsonDocument<1000> doc;
	DeserializationError error = deserializeJson(doc, response);

	// Test if parsing succeeds.
	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}

	favorites_size = doc["favorite_timezones"].size();
	uint8_t i = 0;
	while (i < favorites_size) {
		sprintf(favorite_cities[i], doc["favorite_cities"][i]);
		favorite_timezones[i] = doc["favorite_timezones"][i];
		i++;
	}
}

void ClockApp::deserialize_time_json() {
	// Deserialize the JSON document
	StaticJsonDocument<500> doc;
	DeserializationError error = deserializeJson(doc, response);

	// Test if parsing succeeds.
	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}

	// set the timezone
	if (doc.containsKey("timezone")) {
		if (state == 0) {
			local_timezone = doc["timezone"];
		} else if (state == 2) {
			search_timezone = doc["timezone"];
		}
	}

	// set if the input was correct or not
	if (doc.containsKey("timezone_found")) {
		incorrect_input = !doc["timezone_found"];
	}

	// set the time
	int8_t offset;
	if (state == 0) {
		offset = local_timezone/3600;
	} else {
		offset = search_timezone/3600;
	}
	utc_hour = doc["utc_hour"];
	hour = utc_hour + offset;
	if (hour > 23) {
		hour -= 24;
	} else if (hour < 0) {
		hour += 24;
	}
	minute = doc["utc_minute"];
	second = doc["utc_second"];
	millisecond = doc["utc_millisecond"];
}

void ClockApp::update_timezone(int timezone) {
	int8_t offset = timezone/3600;
	hour = utc_hour + offset;
	if (hour > 23) {
		hour -= 24;
	} else if (hour < 0) {
		hour += 24;
	}
}
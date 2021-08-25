#include "Arduino.h"
#include <WiFi.h> // Connect to WiFi Network
#include <string.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include <EEPROM.h> // library for storing data in flash memory
#include "Phone.h"
#include "App/App.h"
#include "LoginApp.h"

LoginApp::LoginApp() {
    // state variables
    screen_state = 0;  
    vertical_select_state = 0;
    horizontal_select_state = 0;
    state_change = true;
    // keyboard
    show_keyboard = false;
    keyboard.reset_message();
    incorrect_input = false;
    sprintf(text_box_one, "");
    sprintf(text_box_two, "");
    // http request
    loading = false;
    // flash memory
    retrieve_network = true;
    retrieve_user = true;
    sprintf(network, "");
    sprintf(network_password, "");
    sprintf(user, "");
    sprintf(user_password, "");
}

bool LoginApp::connect_to_internet() {
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        // connect to the Internet
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
            uint8_t* cc = WiFi.BSSID(i);
            for (int k = 0; k < 6; k++) {
                Serial.print(*cc, HEX);
                if (k != 5) Serial.print(":");
                cc++;
            }
            Serial.println("");
        }
    }
    delay(100); // wait a bit (100 ms)

    // if using regular connection use line below:
    WiFi.begin(network, network_password);
    // if using channel/mac specification for crowded bands use the following:
    // WiFi.begin(network, password, channel, bssid);
    uint8_t count = 0; //count used for Wifi check times
    Serial.print("Attempting to connect to ");
    Serial.println(text_box_one);
    while (WiFi.status() != WL_CONNECTED && count<6) {
        delay(500);
        Serial.print(".");
        count++;
    }
    delay(2000);
    if (WiFi.isConnected()) { // if we connected then print our IP, Mac, and SSID we're on
        Serial.println("CONNECTED!");
        Serial.printf("%d:%d:%d:%d (%s) (%s)\n",WiFi.localIP()[3],WiFi.localIP()[2],
                                                WiFi.localIP()[1],WiFi.localIP()[0], 
                                                WiFi.macAddress().c_str() ,WiFi.SSID().c_str());
        delay(500);
        return true;
    } else { // if we failed to connect just Try again.
        Serial.println("Failed to Connect :/");
        Serial.println(WiFi.status());
        return false;
    }
}

void LoginApp::login() {
    // send http post request with credentials
    char body[55];
    sprintf(body, "username=%s&passcode=%s", user, user_password);
    char destination[85];
    if (screen_state == 2 || screen_state == 5) {
        sprintf(destination, "http://608dev-2.net/sandbox/sc/team28/final_project/python/Login/login.py");
    } else {
        sprintf(destination, "http://608dev-2.net/sandbox/sc/team28/final_project/python/Login/create_user.py");
    }
    http_request("POST", body, destination, "608dev-2.net");
}

uint8_t LoginApp::handle_user_input() {
    if (retrieve_network) {
        Serial.println("network:");
        retrieve_string_eeprom(network, 0);
        if (strlen(network) == 0) {
            Serial.println("network not stored");
        } else {
            Serial.println(network);
            screen_state = 4; // switch to ask to connect to saved network
        }
        Serial.println("network password:");
        retrieve_string_eeprom(network_password, 19);
        if (strlen(network_password) == 0) {
            Serial.println("network password not stored");
        } else {
            Serial.println(network_password);
        }
        retrieve_network = false;
    }
    if (retrieve_user) {
        Serial.println("user:");
        retrieve_string_eeprom(user, 38);
        if (strlen(user) == 0) {
            Serial.println("user not stored");
        } else {
            Serial.println(user);
        }
        Serial.println("user password:");
        retrieve_string_eeprom(user_password, 57);
        if (strlen(user_password) == 0) {
            Serial.println("user password not stored");
        } else {
            Serial.println(user_password);
        }
        retrieve_user = false;
    }

    if (!loading) {
        if (!show_keyboard) {
            if (screen_state == 1) {
                if (button_input_two == 1 || button_input_two == 2) {
                    // vertical scroll
                    if (vertical_select_state < 1) {
                        vertical_select_state++;
                    } else {
                        vertical_select_state = 0;
                    }
                    state_change = true;
                }
                if (button_input_three == 1) {
                    // click the login or new user button
                    if (vertical_select_state == 0) {
                        if (strlen(user) == 0) {
                            screen_state = 2; // switch to user login
                        } else {
                            screen_state = 5; // switch to ask to login as saved user
                        }
                    } else {
                        // switch to new user
                        screen_state = 3;
                    }
                    state_change = true;
                    vertical_select_state = 0;
                    reset_screen();
                }
            } else if (screen_state == 4 || screen_state == 5) {
                // connect to saved network or login to saved user
                if (button_input_one == 1 || button_input_one == 2) {
                    // horizontal scroll
                    if (horizontal_select_state < 1) {
                        horizontal_select_state++;
                    } else {
                        horizontal_select_state = 0;
                    }
                    state_change = true;
                }
                if (button_input_three == 1) {
                    // enter button
                    if (horizontal_select_state == 0) {
                        if (screen_state == 4) {
                            // do not connect to saved network
                            screen_state = 0; // switch to connect to internet
                        } else {
                            // do not connect to saved user
                            screen_state = 2; // switch to login
                        }
                    } else {
                        if (screen_state == 4) {
                            // connect to saved network
                            if (connect_to_internet()) {
                                screen_state = 1;
                                get_current_location();
                                loading = true;
                            } else {
                                incorrect_input = true;
                            }
                        } else {
                            // login as saved user
                            login();
                            loading = true;
                        }
                    }
                    horizontal_select_state = 0;
                    state_change = true;
                    reset_screen();
                }
            } else if (screen_state == 6 || screen_state == 7) {
                // save network or user
                if (button_input_one == 1 || button_input_one == 2) {
                    // horizontal scroll
                    if (horizontal_select_state < 1) {
                        horizontal_select_state++;
                    } else {
                        horizontal_select_state = 0;
                    }
                    state_change = true;
                }
                if (button_input_three == 1) {
                    // enter button
                    if (screen_state == 6) {
                        if (horizontal_select_state == 1) {
                            // save network
                            store_string_eeprom(network, 0);
                            store_string_eeprom(network_password, 19);
                        }
                        screen_state = 1; // switch to select login or new user
                        horizontal_select_state = 0;
                        state_change = true;
                        reset_screen();
                    } else {
                        if (horizontal_select_state == 1) {
                            // save user
                            store_string_eeprom(user, 38);
                            store_string_eeprom(user_password, 57);
                        }
                        return HOME;
                    }
                }
            } else {
                if (button_input_two == 1 || button_input_two == 2) {
                    // vertical scroll
                    if (vertical_select_state < 2) {
                        vertical_select_state++;
                    } else {
                        vertical_select_state = 0;
                    }
                    state_change = true;
                }
                if ((screen_state == 2 || screen_state == 3) && vertical_select_state == 2) {
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
                    if (vertical_select_state == 0 || vertical_select_state == 1) {
                        // enter text into box
                        show_keyboard = true;
                    } else {
                        // submit info
                        if (screen_state == 2 || screen_state == 3) {
                            if (horizontal_select_state == 0) {
                                // go back to select login or new user
                                screen_state = 1;
                                sprintf(text_box_one, "");
                                sprintf(text_box_two, "");
                                vertical_select_state = 0;
                                horizontal_select_state = 0;
                                incorrect_input = false;
                            } else {
                                sprintf(user, text_box_one);
                                sprintf(user_password, text_box_two);
                                login();
                                loading = true;
                            }
                            reset_screen();
                        } else {
                            // connect to the internet
                            sprintf(network, text_box_one);
                            sprintf(network_password, text_box_two);
                            if (connect_to_internet()) {
                                screen_state = 6;
                                sprintf(text_box_one, "");
                                sprintf(text_box_two, "");
                                vertical_select_state = 0;
                                incorrect_input = false;
                                get_current_location();
                                loading = true;
                                reset_screen();
                            } else {
                                incorrect_input = true;
                            }
                        }
                    }
                    state_change = true;
                }
            }
        } else {
            if (button_input_one != 0 || button_input_two != 0 || button_input_three != 0) {
                state_change = true;
            }
            bool keyboard_enter = keyboard.handle_user_input(button_input_one, button_input_two, button_input_three);
            if (vertical_select_state == 0) {
                strncpy(text_box_one, keyboard.get_current_message(), 18);
            } else {
                strncpy(text_box_two, keyboard.get_current_message(), 18);
            }
            if (keyboard_enter) {
                keyboard.reset_message();
                show_keyboard = false;
                reset_screen();
            }
        }
    } else {
        if (!request_sending) {
            state_change = true;
            loading = false;
            if (screen_state == 2 || screen_state == 5) {
                if (strncmp(response, "True", 4) == 0) {
                    // login or creation of new user was successful
                    // store username
                    strcpy(username, user);
                    if (screen_state == 5) {
                        return HOME; // switch to home screen
                    } else {
                        screen_state = 7; // switch to ask to save user
                    }
                } else {
                    // login or creation of new user was NOT successful
                    incorrect_input = true;
                }
            } else {
                deserialize_location_json();
            }
            reset_screen();
        }
    }
    return NO_CHANGE;
}

void LoginApp::reset_screen() {
    tft.fillScreen(TFT_HOME_SCREEN);
}

void LoginApp::draw_screen() {
    if (state_change) {
        if (loading) {
            loading_animation.set_dimensions(64, 80, 12);
        } else {
            if (screen_state == 1) {
                // select login or new user
                tft.setTextSize(2);
                if (vertical_select_state == 0) {
                    tft.fillRect(12, 44, 104, 24, TFT_WHITE);
                    tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
                    tft.drawString("login", 12+22, 44+4);

                    tft.fillRect(12, 92, 104, 24, TFT_BLACK);
                    tft.setTextColor(TFT_WHITE, TFT_BLACK); // set color of font to white foreground, black background
                    tft.drawString("new user", 12+4, 92+4);
                } else {
                    tft.fillRect(12, 44, 104, 24, TFT_BLACK);
                    tft.setTextColor(TFT_WHITE, TFT_BLACK); // set color of font to white foreground, black background
                    tft.drawString("login", 12+22, 44+4);

                    tft.fillRect(12, 92, 104, 24, TFT_WHITE);
                    tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
                    tft.drawString("new user", 12+4, 92+4);
                }
            } else if (screen_state >= 4) {
                // connect to saved network or login to saved user OR save network or user
                tft.setTextSize(1);
                tft.setTextColor(TFT_WHITE, TFT_HOME_SCREEN);
                if (screen_state == 4) {
                    tft.drawString("want to connect to", 10, 43);
                    tft.drawString(network, 64-strlen(network)*3, 53);
                } else if (screen_state == 5) {
                    tft.drawString("want to login as", 16, 43);
                    tft.drawString(user, 64-strlen(user)*3, 53);
                } else if (screen_state == 6) {
                    tft.drawString("want to save", 28, 43);
                    tft.drawString(network, 64-strlen(network)*3, 53);
                } else {
                    tft.drawString("want to save", 28, 43);
                    tft.drawString(user, 64-strlen(user)*3, 53);
                }

                tft.fillRect(16, 104, 40, 12, TFT_WHITE);
                tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
                tft.drawString("no", 16+14, 104+2);

                tft.fillRect(72, 104, 40, 12, TFT_WHITE);
                tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
                tft.drawString("yes", 72+11, 104+2);

                if (horizontal_select_state == 0) {
                    tft.drawRect(15, 103, 42, 14, TFT_BLACK);
                    tft.drawRect(71, 103, 42, 14, TFT_HOME_SCREEN);
                } else {
                    tft.drawRect(15, 103, 42, 14, TFT_HOME_SCREEN);
                    tft.drawRect(71, 103, 42, 14, TFT_BLACK);
                }
            } else {
                tft.setTextSize(1);
                tft.setTextColor(TFT_WHITE, TFT_HOME_SCREEN); // set color of font to white foreground, blue background
                if (screen_state == 2 || screen_state == 3) {
                    // enter username and password
                    tft.drawString("username:", 8, 9);
                    tft.setTextColor(TFT_RED, TFT_HOME_SCREEN); // set color of font to red foreground, blue background
                    if (incorrect_input) {
                        tft.drawString("incorrect", 68, 9);
                    }
                } else {
                    // select WiFi network and enter password
                    tft.drawString("network:", 8, 9);
                    tft.setTextColor(TFT_RED, TFT_HOME_SCREEN); // set color of font to red foreground, blue background
                    if (incorrect_input) {
                        tft.drawString("incorrect", 68, 9);
                    }
                }
                tft.fillRect(8, 24, 112, 12, TFT_WHITE);
                tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
                tft.drawString(text_box_one, 10, 26);

                tft.setTextColor(TFT_WHITE, TFT_HOME_SCREEN); // set color of font to white foreground, blue background
                tft.drawString("password:", 8, 45);
                tft.setTextColor(TFT_RED, TFT_HOME_SCREEN); // set color of font to red foreground, blue background
                if (incorrect_input) {
                    tft.drawString("incorrect", 68, 45);
                }
                tft.fillRect(8, 60, 112, 12, TFT_WHITE);
                tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
                tft.drawString(text_box_two, 10, 62);

                if (screen_state == 2 || screen_state == 3) {
                    tft.fillRect(16, 81, 40, 12, TFT_WHITE);
                    tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
                    tft.drawString("back", 16+8, 81+2);

                    tft.fillRect(72, 81, 40, 12, TFT_WHITE);
                    tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
                    tft.drawString("submit", 72+2, 81+2);
                } else {
                    tft.fillRect(42, 81, 44, 12, TFT_WHITE);
                    tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
                    tft.drawString("submit", 42+4, 81+2);
                }
                if (vertical_select_state == 0) {
                    tft.drawRect(7, 23, 114, 14, TFT_BLACK);
                } else if (vertical_select_state == 1) {
                    tft.drawRect(7, 59, 114, 14, TFT_BLACK);
                } else {
                    if (screen_state == 2 || screen_state == 3) {
                        if (horizontal_select_state == 0) {
                            tft.drawRect(15, 80, 42, 14, TFT_BLACK);
                        } else {
                            tft.drawRect(71, 80, 42, 14, TFT_BLACK);
                        }
                    } else {
                        tft.drawRect(41, 80, 46, 14, TFT_BLACK);
                    }
                }
                if (show_keyboard) {
                    keyboard.draw_screen();
                }
            }
        }
        state_change = false;
    } else {
        if (loading) {
            loading_animation.draw_screen();
        }
    }
}

void LoginApp::store_string_eeprom(char* write_string, uint8_t start_addr) {
    // writing byte-by-byte to EEPROM
    for (int i = 0; i < 19; i++) {
        EEPROM.write(start_addr + i, write_string[i]);
    }
    EEPROM.commit();
}

void LoginApp::retrieve_string_eeprom(char* read_string, uint8_t start_addr) {
    // reading byte-by-byte from EEPROM
    for (int i = 0; i < 19; i++) {
        byte readValue = EEPROM.read(start_addr + i);

        if (readValue == 0xff || readValue == 0) {
            // 0xff is the default value of an empty EEPROM cell
            // 0 is the int value for the null character
            read_string[i] = 0;
            break;
        }

        char readValueChar = char(readValue);
        read_string[i] = readValueChar;
    }
}
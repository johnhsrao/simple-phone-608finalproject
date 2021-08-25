#include "Arduino.h"
#include "Phone.h"
#include "Keyboard.h"

Keyboard::Keyboard() {
    horizontal_select_state = 0;
    vertical_select_state = 0;
    current_keyboard = 0;
    offset = 0;
}

bool Keyboard::handle_user_input(uint8_t button_input_one, uint8_t button_input_two, uint8_t button_input_three) {
    if (button_input_two == 1 || button_input_two == 2) {
        // vertical scroll
        if (vertical_select_state < 2) {
            vertical_select_state++;
        } else if (vertical_select_state == 2) {
            vertical_select_state++;
            if (horizontal_select_state >= 6 && current_keyboard != 2) {
                horizontal_select_state--;
            }
        } else {
            vertical_select_state = 0;
            if (horizontal_select_state == 6 && current_keyboard != 2) {
                horizontal_select_state = 7;
            }
        }
    }
    if (button_input_one == 1 || button_input_one == 2) {
        // horizontal scroll
        if (vertical_select_state == 3 && current_keyboard != 2) {
            if (horizontal_select_state < 6) {
                horizontal_select_state++;
            } else {
                horizontal_select_state = 0;
            }
        } else {
            if (horizontal_select_state < 7) {
                horizontal_select_state++;
            } else {
                horizontal_select_state = 0;
            }
        }
    }
    if (button_input_three == 1) {
        uint8_t horizontal_limit;
        if (vertical_select_state == 3 && current_keyboard != 2) {
            horizontal_limit = 5;
        } else {
            horizontal_limit = 7;
        }

        if (horizontal_select_state < horizontal_limit) {
            // enter a character into the message
            char *current_character = keyboards[current_keyboard][vertical_select_state*7 + horizontal_select_state];
            offset += sprintf(current_message + offset, "%s", current_character);
        } else {
            // click a button
            if (vertical_select_state == 0) {
                // switch between letters and numbers/symbols
                if (current_keyboard != 2) {
                    current_keyboard = 2;
                } else {
                    current_keyboard = 0;
                }
            } else if (vertical_select_state == 1) {
                // spacebar
                offset += sprintf(current_message + offset, " ");
            } else if (vertical_select_state == 2) {
                // delete a character
                if (offset > 0) {
                    sprintf(current_message + offset - 1, "\0");
                    offset--;
                }
            } else {
                if (horizontal_select_state == 5) {
                    // switch between upper and lower case
                    if (current_keyboard != 2) {
                        current_keyboard = !current_keyboard;
                    }
                } else {
                    // enter the message
                    return true;
                }
            }
        }
    }
    return false;
}

char* Keyboard::get_current_message() {
    return current_message;
}

void Keyboard::reset_message() {
    horizontal_select_state = 0;
    vertical_select_state = 0;
    current_keyboard = 0;
    offset = 0;
    sprintf(current_message, "");
}

void Keyboard::draw_screen() {
    tft.setTextSize(1);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 7; j++) {
            if (i == vertical_select_state && j == horizontal_select_state) {
                tft.fillRect(2+j*12, 102+i*14, 10, 12, TFT_WHITE);
                tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
                tft.drawString(keyboards[current_keyboard][i*7+j], 2+j*12+2, 102+i*14+2);
            } else {
                tft.fillRect(2+j*12, 102+i*14, 10, 12, TFT_KEY_GRAY);
                tft.setTextColor(TFT_WHITE, TFT_KEY_GRAY); // set color of font to white foreground, gray background
                tft.drawString(keyboards[current_keyboard][i*7+j], 2+j*12+2, 102+i*14+2);
            }
            if (i == 3 && j == 4 && current_keyboard != 2) {
                break;
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        if (i == 3) {
            if (current_keyboard != 2) {
                // draw the upper/lower case button
                if (vertical_select_state == i && horizontal_select_state == 5) {
                    tft.fillRect(62, 102+i*14, 22, 12, TFT_WHITE);
                    tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
                } else {
                    tft.fillRect(62, 102+i*14, 22, 12, TFT_KEY_GRAY);
                    tft.setTextColor(TFT_WHITE, TFT_KEY_GRAY); // set color of font to white foreground, gray background
                }
                tft.drawString(case_buttons[current_keyboard], 67, 102+i*14+2);
            } else {
                // removes the middle part of the case button that was leftover
                tft.fillRect(72, 102+i*14, 2, 12, TFT_BLACK);
            }
        }
        // draw the switch, space, delete, or enter buttons
        uint8_t horizontal_limit;
        if (i == 3 && current_keyboard != 2) {
            horizontal_limit = 6;
        } else {
            horizontal_limit = 7;
        }
        if (vertical_select_state == i && horizontal_select_state == horizontal_limit) {
            tft.fillRect(86, 102+i*14, 40, 12, TFT_WHITE);
            tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
        } else {
            tft.fillRect(86, 102+i*14, 40, 12, TFT_KEY_GRAY);
            tft.setTextColor(TFT_WHITE, TFT_KEY_GRAY); // set color of font to white foreground, gray background
        }
        if (i == 0) {
            tft.drawString(switch_buttons[current_keyboard], 88+9, 102+i*14+2);
        } else {
            if (i == 2) {
                tft.drawString(buttons[i-1], 88+1, 102+i*14+2);
            } else {
                tft.drawString(buttons[i-1], 88+4, 102+i*14+1);
            }
        }
    }
}
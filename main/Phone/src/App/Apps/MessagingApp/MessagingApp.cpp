#include "Arduino.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include <ArduinoJson.h>
#include "Phone.h"
#include "App/App.h"
#include "MessagingApp.h"


MessagingApp::MessagingApp() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    checkUnreadNum();
    timer = millis();
    msg_index = 1;
    text_state = 0;
    select_state = 1;
    prev_selected = 3;
    prev_state = 5;
    otherSelected = false;
    messageSelected = false;
    memset(other, 0, sizeof(other));
    memset(message, 0, sizeof(message));
    memset(prev_keyboard_input, 0, sizeof(prev_keyboard_input));
    firstTime = true;
    unread_num = 0;
    prev_unread_num = 0;
    notif_period = 3000;
    msg_display_updated = true;
    more_msg = false;
    parsed_doc;
    loading = false;
    sending = false;
    DISPLAYING = false;
}

void MessagingApp::reset_screen(){
    tft.fillScreen(TFT_BLACK);
    if(prev_state == DISPLAY_MESSAGE && DISPLAYING == true){
        Serial.println("reset screen");
        msg_display_updated = false;
    }
    else if(prev_state = DISPLAY_MESSAGE){
        prev_state = SEND_MESSAGE;
    }
    else{
        prev_state = 5;
    }
}

void MessagingApp::sendMessage(char* other, char* message){
    char body[300];
    sprintf(body, "action=send&sender=%s&receiver=%s&message=%s", username, other, message);
    http_request("POST",
        body,
        "http://608dev-2.net/sandbox/sc/team28/final_project/python/MessagingApp/messaging_server.py",
        "608dev-2.net");
    sending = true;
}

void MessagingApp::checkUnreadMessages(){
    char body[300];
    sprintf(body, "action=check&other=UN_READ&me=%s&num=10&size=900", username);
    http_request("POST",
        body,
        "http://608dev-2.net/sandbox/sc/team28/final_project/python/MessagingApp/messaging_server.py",
        "608dev-2.net");
    loading = true;
}

void MessagingApp::checkMessages(char* other, int number){
    char body[300];
    sprintf(body, "action=check&other=%s&me=%s&num=%d&size=900", other, username, number);
    http_request("POST",
        body,
        "http://608dev-2.net/sandbox/sc/team28/final_project/python/MessagingApp/messaging_server.py",
        "608dev-2.net");
    loading = true;
}

void MessagingApp::checkUnreadNum(){
    if(!request_sending){
    char query[200];
    sprintf(query, "receiver=%s", username);
    http_request("GET",
        query,
        "http://608dev-2.net/sandbox/sc/team28/final_project/python/MessagingApp/messaging_server.py",
        "608dev-2.net");
    checkingNum = true;
    }
}

uint8_t MessagingApp::drawMessages(char* d, char* msg, uint8_t yPos){
    char sender[10];
    sprintf(sender, "%c", msg[0]);
    sprintf(sender + strlen(sender), "%c", msg[1]);
    if(strcmp(sender, "Me")==0){
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
    }
    tft.setCursor(5, yPos);
    tft.println(msg);
    if(strcmp(d, " ")!=0){
        tft.setTextColor(0x7BEF, TFT_BLACK);
        tft.setCursor(5, yPos - 10);
        tft.println(d);
    }
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    int curr_y = tft.getCursorY();
    return curr_y;
}

uint8_t MessagingApp::new_yPos(uint8_t y, uint8_t x_start, char* msg){
    uint16_t msg_length = strlen(msg);
    uint8_t max_num = (SCREEN_WIDTH - x_start) / CHAR_WIDTH;
    uint8_t num_lines = msg_length / max_num;
    uint8_t num_pix = (num_lines+1)*(CHAR_HEIGHT+2)+10;
    uint8_t new_y = y - num_pix;
    return new_y;
}

void MessagingApp::message_parsing(StaticJsonDocument<1000> &doc, int next_msg){
    tft.fillScreen(TFT_BLACK);
    uint8_t yPos = 150;
    uint16_t body_length = doc["length"];
    Serial.println(body_length);
    if(body_length > 0){
        for(int i = next_msg; i <= body_length; i++){
            char key[2];
            memset(key, 0, sizeof(key));
            itoa(i, key, 10);
            char date[20];
            sprintf(date, doc[key]["date"]);
            char message[1000];
            sprintf(message, doc[key]["message"]);
            yPos = new_yPos(yPos,5,message);
            if(yPos < 48){
                tft.setTextColor(TFT_BLUE);
                tft.setCursor(5, 5);
                tft.println("RIGHT to Scroll Up");
                tft.setCursor(5, 15);
                tft.println("DOWN to Scroll Down");
                tft.setTextColor(TFT_WHITE);
                more_msg = true;
                break;
            }
            uint8_t last_y = drawMessages(date, message, yPos);
            yPos = last_y;
        }
        if(more_msg != true){
            tft.setTextColor(TFT_BLUE);
            tft.setCursor(1, 1);
            char end_m[200];
            sprintf(end_m, doc["end"]);
            tft.println(end_m);
            tft.println("Long Press RIGHT to go back");
            tft.setTextColor(TFT_WHITE);
        }
    }
    else{
        char date[5];
        sprintf(date, " ");
        char message[200];
        sprintf(message, doc["end"]);
        drawMessages(date, message, yPos);
    }
}

void MessagingApp::draw_selected(uint8_t selection, uint8_t prev){
    if(selection == SELECT_SEND){
        tft.fillRect(8, 55, 114, 28, TFT_GREEN);
    }
    else if(selection == SELECT_HISTORY){
        tft.fillRect(8, 85, 114, 28, TFT_GREEN);
    }
    else if(selection == SELECT_UNREAD){
        tft.fillRect(8, 25, 114, 28, TFT_GREEN);
    }
    if(prev == SELECT_SEND){
        tft.fillRect(8, 55, 114, 28, TFT_BLACK);
    }
    else if(prev == SELECT_HISTORY){
        tft.fillRect(8, 85, 114, 28, TFT_BLACK);
    }
    else if(prev == SELECT_UNREAD){
        tft.fillRect(8, 25, 114, 28, TFT_BLACK);
    }
}

uint8_t MessagingApp::handle_user_input() {
    if(button_input_one == 2){
        checkUnreadNum();
        memset(message, 0, sizeof(message));
        memset(other, 0, sizeof(other));
        otherSelected = false;
        messageSelected = false;
        firstTime = true;
        prev_selected = 3;
        select_state = 1;
        msg_index = 1;
        prev_state = text_state;
        text_state = SELECT_MENU;
    }
    else if (text_state == SELECT_MENU){// main menu 
        //updating unread_num only if response is back
        if(checkingNum && !request_sending){
            checkingNum = false;
            prev_unread_num = unread_num;
            unread_num = atoi(response);
        }
        //select option using DOWN button (button 2)
        if (button_input_two == 1){
            select_state ++;
            if(select_state > SELECT_HISTORY){
                if(unread_num > 0){
                    select_state = SELECT_UNREAD;
                }
                else{
                    select_state = SELECT_SEND;
                }
            }
        }
        else if(button_input_three == 1){
            if(select_state == SELECT_UNREAD){
                checkUnreadMessages();
                text_state = DISPLAY_MESSAGE;
                prev_state = SELECT_MENU;
            }
            else if(select_state == SELECT_SEND){
                text_state = SEND_MESSAGE;
                prev_state = SELECT_MENU;
                otherSelected = false;
                messageSelected = false;
            }
            else if(select_state == SELECT_HISTORY){
                text_state = CHECK_MESSAGE;
                prev_state = SELECT_MENU;
            }
        }
        if(millis()-timer >= notif_period){
            checkUnreadNum();
            timer = millis();
        }
    }

    else if(text_state== SEND_MESSAGE){ //Sending Messages
            if(keyboard.handle_user_input(button_input_one, button_input_two, button_input_three)){
                if (otherSelected == false){
                    sprintf(other, "%s", keyboard.get_current_message());
                    keyboard.reset_message();
                    otherSelected = true;
                }else{
                    sprintf(message, "%s", keyboard.get_current_message()); 
                    keyboard.reset_message();
                    sendMessage(other, message);
                    otherSelected = false;
                    text_state = DISPLAY_MESSAGE;
                    prev_state = SEND_MESSAGE;
                    firstTime = true;
                }
            }
    }

    else if(text_state==CHECK_MESSAGE){ //Selecting which messages to see
            if(keyboard.handle_user_input(button_input_one, button_input_two, button_input_three)){
                if(otherSelected == false){
                    sprintf(other, "%s", keyboard.get_current_message());
                    keyboard.reset_message();
                    otherSelected = true;
                }else{
                    sprintf(message, "%s", keyboard.get_current_message());
                    keyboard.reset_message();
                    checkMessages(other, atoi(message));
                    otherSelected = false;
                    text_state = DISPLAY_MESSAGE;
                    prev_state = CHECK_MESSAGE;
                    firstTime = true;
                }
        }
    }
    else if(text_state==DISPLAY_MESSAGE){
            if(sending && !request_sending){
                sending = false;
                prev_state = SEND_MESSAGE;
            }
            else if(loading && !request_sending){
                loading = false;
                prev_state = CHECK_MESSAGE;
                Serial.println("crash check 2");
            }
            if(button_input_one > 0 && more_msg == true){
                msg_index ++;
                msg_display_updated = false;
                more_msg = false;
            }
            else if(button_input_two > 0 && msg_index > 1){
                msg_index --;
                msg_display_updated = false;
            }
    }
    if (button_input_three == 2) {
        return HOME; // switch to the home app
        prev_state = 5;
    } else {
        return NO_CHANGE; // stay on the messaging app
    } //Handling user inputs for texting select menu
}

void MessagingApp::draw_loading(){
    if(prev_state != LOADING_SCREEN){
        tft.fillScreen(TFT_BLACK);
        loading_animation.set_dimensions(64, 80, 12);
        prev_state = LOADING_SCREEN;
    }
    loading_animation.draw_screen();
}

void MessagingApp::draw_screen() {
        //Serial.println(text_state);
    	if(text_state==SELECT_MENU){
            if(prev_state != SELECT_MENU){
                tft.fillScreen(TFT_BLACK);
                prev_state = SELECT_MENU;
                prev_selected = 3;
                char notification[30];
                tft.setTextColor(TFT_RED, TFT_BLACK);
                sprintf(notification, "%d new messages", unread_num);
                tft.drawString(notification, 20, 10);
                tft.setTextColor(TFT_WHITE);
                if(unread_num > 0){
                    tft.drawRect(8, 25, 114, 28, TFT_BLUE);
                    tft.setTextColor(TFT_WHITE);
                    tft.drawString("View All Unread", 20, 35);
                }
                else{
                    tft.drawRect(8, 25, 114, 28, 0x7BEF);
                    tft.setTextColor(0x7BEF);
                    tft.drawString("View All Unread", 20, 35);
                    tft.setTextColor(TFT_WHITE);
                }
                tft.drawRect(8, 55, 114, 28, TFT_BLUE);
                tft.drawRect(8, 85, 114, 28, TFT_BLUE);
                tft.drawString("Send Message", 32, 65);
                tft.drawString("Check History", 30, 95);
            }
            else if(unread_num != prev_unread_num){
                tft.setTextColor(TFT_RED, TFT_BLACK);
                char new_draw_num[30];
                sprintf(new_draw_num, "%d new messages", unread_num);
                tft.drawString(new_draw_num, 20, 10);
                tft.setTextColor(TFT_WHITE);
                if(unread_num > 0){
                    tft.drawRect(8, 25, 114, 28, TFT_BLUE);
                    tft.setTextColor(TFT_WHITE);
                    tft.drawString("View All Unread", 20, 35);
                }
                else{
                    tft.drawRect(8, 25, 114, 28, 0x7BEF);
                    tft.setTextColor(0x7BEF);
                    tft.drawString("View All Unread", 20, 35);
                    tft.setTextColor(TFT_WHITE);
                }
            }
            if(prev_selected != select_state){
                draw_selected(select_state, prev_selected);
                prev_selected = select_state;
                if(unread_num > 0){
                    tft.drawRect(8, 25, 114, 28, TFT_BLUE);
                    tft.setTextColor(TFT_WHITE);
                    tft.drawString("View All Unread", 20, 35);
                }
                else{
                    tft.drawRect(8, 25, 114, 28, 0x7BEF);
                    tft.setTextColor(0x7BEF);
                    tft.drawString("View All Unread", 20, 35);
                    tft.setTextColor(TFT_WHITE);
                }
                tft.drawRect(8, 55, 114, 28, TFT_BLUE);
                tft.drawRect(8, 85, 114, 28, TFT_BLUE);
                tft.setTextColor(TFT_WHITE);
                tft.drawString("Send Message", 32, 65);
                tft.drawString("Check History", 30, 95); 
            }   
    	}
    	else if(text_state==SEND_MESSAGE){
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            if(prev_state != SEND_MESSAGE){
                 tft.fillScreen(TFT_BLACK);
                 prev_state = SEND_MESSAGE;
                 if(otherSelected != true && firstTime == true){
                    tft.drawRect(5, 5, 115, 25, TFT_GREEN);
                    tft.drawString("Recepient:", 10, 8);
                    tft.drawRect(5, 35, 115, 60, TFT_WHITE);
                }
            }
    		keyboard.draw_screen();
            if (otherSelected != true && firstTime == true){
                if(strlen(keyboard.get_current_message()) < strlen(prev_keyboard_input)){
                    tft.fillRect(5, 5, 115, 25, TFT_BLACK);
                    tft.drawRect(5, 5, 115, 25, TFT_GREEN);
                    tft.drawString("Recepient:", 10, 8);
                }
                tft.drawString(keyboard.get_current_message(), 10, 17);
                sprintf(prev_keyboard_input, "%s", keyboard.get_current_message());
            }
    		if(otherSelected == true && firstTime == true){
                tft.fillScreen(TFT_BLACK);
                firstTime = false;
                char draw_s[200];
                tft.drawRect(5, 5, 115, 25, TFT_WHITE);
                tft.drawString("Recepient:", 10, 8);
                sprintf(draw_s, "%s", other);
                tft.setTextColor(0x7BEF, TFT_BLACK);
                tft.drawString(draw_s, 10, 18);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawRect(5, 35, 115, 60, TFT_GREEN);
                tft.drawString("Message:", 10, 37);
            }
            if(otherSelected == true){
                if(strlen(keyboard.get_current_message()) < strlen(prev_keyboard_input)){
                    tft.fillRect(5, 35, 115, 60, TFT_BLACK);
                    tft.drawRect(5, 35, 115, 60, TFT_GREEN);
                    tft.drawString("Message:", 10, 37);
                }
                tft.drawString(keyboard.get_current_message(), 10, 54);
                sprintf(prev_keyboard_input, "%s", keyboard.get_current_message());
            }
        }
    	else if(text_state==CHECK_MESSAGE){
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            if(prev_state != CHECK_MESSAGE){
                tft.fillScreen(TFT_BLACK);
                prev_state = CHECK_MESSAGE;
                if(otherSelected != true && firstTime == true){
                    tft.drawRect(5, 5, 115, 25, TFT_GREEN);
                    tft.drawString("Check:", 10, 8);
                    tft.drawRect(5, 35, 115, 60, TFT_WHITE);
                }
            }
            
    		keyboard.draw_screen();
            if (otherSelected != true && firstTime == true){
                if(strlen(keyboard.get_current_message()) < strlen(prev_keyboard_input)){
                    tft.fillRect(5, 5, 115, 25, TFT_BLACK);
                    tft.drawRect(5, 5, 115, 25, TFT_GREEN);
                    tft.drawString("Check:", 10, 8);
                }
                tft.drawString(keyboard.get_current_message(), 50, 8);
                sprintf(prev_keyboard_input, "%s", keyboard.get_current_message());
            }
            if(otherSelected == true && firstTime == true){
                tft.fillScreen(TFT_BLACK);
                firstTime = false;
                char draw_s[200];
                tft.drawRect(5, 5, 115, 25, TFT_WHITE);
                tft.drawString("Check:", 10, 8);
                sprintf(draw_s, "%s", other);
                tft.setTextColor(0x7BEF, TFT_BLACK);
                tft.drawString(draw_s, 10, 18);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawRect(5, 35, 115, 60, TFT_GREEN);
                tft.drawString("How Many?", 10, 37);
            }
            if(otherSelected == true){
                if(strlen(keyboard.get_current_message()) < strlen(prev_keyboard_input)){
                    tft.fillRect(5, 35, 115, 60, TFT_BLACK);
                    tft.drawRect(5, 35, 115, 60, TFT_GREEN);
                    tft.drawString("How Many?", 10, 37);
                }
                tft.drawString(keyboard.get_current_message(), 10, 50);
                sprintf(prev_keyboard_input, "%s", keyboard.get_current_message());
            }
    	}
    	else if(text_state==DISPLAY_MESSAGE){
            if(loading || sending){
                draw_loading();
            }
            else{
                if(prev_state != DISPLAY_MESSAGE){
                    tft.fillScreen(TFT_BLACK);
                }
        		if((prev_state == CHECK_MESSAGE) or (prev_state == SELECT_MENU)){
                    Serial.println("updating json");
        			deserializeJson(parsed_doc, response);
                    DISPLAYING = true;
                    message_parsing(parsed_doc, 1);
        		}
                else if(prev_state == SEND_MESSAGE){
                    tft.setCursor(3, 5);
                    tft.println(response);
                    tft.setCursor(3, tft.getCursorY()+5);
                    tft.setTextColor(TFT_BLUE);
                    tft.println("Long Press RIGHT to go back");
                    tft.setTextColor(TFT_WHITE, TFT_BLACK);
                }
                else if(msg_display_updated != true && DISPLAYING == true){
                    Serial.println("crash check 1");
                    message_parsing(parsed_doc, msg_index);
                    msg_display_updated = true;
                }
                prev_state = DISPLAY_MESSAGE;
        	}
        }
    }
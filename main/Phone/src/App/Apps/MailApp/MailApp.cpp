#include "Arduino.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Phone.h"
#include "App/App.h"
#include "MailApp.h"
#include <ArduinoJson.h>

MailApp::MailApp() {  
    // background color
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // set color of font to white foreground, black background
    account_json;
    email_json;
    getSavedAccounts();
    mail_state = 0;
    memset(user_email, 0, sizeof(user_email));
    memset(email_temp, 0, sizeof(email_temp));
    memset(other, 0, sizeof(other));
    memset(message, 0, sizeof(message));
    memset(prev_keyboard_input, 0, sizeof(prev_keyboard_input));
    emailEntered = false;
    subjectEntered = false;
    messageEntered = false;
    login_failed = false;
    more_unread = false;
    select_state = 0;
    unread_num = 0;
    notif_period = 3000;
    msg_index = 1;
    select_state = 0;
    prev_selected = 0;
    prev_state = 0;
    firstTime = true;
    msg_display_updated = true;
    more_to_view = false;
    checkingNum = false;
    sending = false;
    DISPLAYING = false;
}
void MailApp::reset_screen(){
    tft.fillScreen(TFT_BLACK);
    if(prev_state == DISPLAY_MAIL && DISPLAYING == true){
        Serial.println("reset screen");
        msg_display_updated = false;
    }
    else if(prev_state == DISPLAY_MAIL){
        prev_state = SEND_MAIL;
    }
    else if (prev_state == OPTION_MENU){
        select_state = SELECT_SEND;
        prev_selected = 0;
    }
}

void MailApp::sendMail(char* recipient, char* subject, char* message){
    char body[1000];
    sprintf(body, "sender=%s&receiver=%s&subject=%s&message=%s", user_email, recipient, subject, message);
    http_request("POST",
        body,
        "http://608dev-2.net/sandbox/sc/team28/final_project/python/EmailApp/sendMail.py",
        "608dev-2.net");
    sending = true;
}

void MailApp::checkUnreadNum(){
    if(!request_sending){
        char query[200];
        sprintf(query, "user=%s", user_email);
        http_request("GET",
        query,
        "http://608dev-2.net/sandbox/sc/team28/final_project/python/EmailApp/checkUnread.py",
        "608dev-2.net");
        checkingNum = true;
    }
}

void MailApp::checkUnreadMail(){
    char body[500];
    sprintf(body, "user=%s&size=900", user_email);
    http_request("POST",
        body,
        "http://608dev-2.net/sandbox/sc/team28/final_project/python/EmailApp/checkUnread.py",
        "608dev-2.net");
    loading = true;
}

void MailApp::getSavedAccounts(){
    char query[200];
    sprintf(query, "user=%s", username);
    http_request("GET",
        query,
        "http://608dev-2.net/sandbox/sc/team28/final_project/python/EmailApp/email_login.py",
        "608dev-2.net");
    loading = true;
}

void MailApp::storeSavedAccounts(){
    deserializeJson(account_json, response);
    num_saved_account = account_json["num"];
    Serial.println("storing accounts");
}

void MailApp::loginEmail(char* account, char* password){
    char body[500];
    sprintf(body, "user=%s&email=%s&passcode=%s", username, account, password);
    http_request("POST",
        body,
        "http://608dev-2.net/sandbox/sc/team28/final_project/python/EmailApp/email_login.py",
        "608dev-2.net");
    loading = true;
}

void MailApp::setUserEmail(uint8_t account_index){
    if(account_index > 0){
        char key[5];
        memset(key, 0, sizeof(key));
        itoa(account_index, key, 10);
        strcpy(user_email, account_json[key]);
    }
}

void MailApp::mail_parsing(StaticJsonDocument<1000> &doc, int next_msg){
    Serial.println("in mail parsing");
    Serial.println(next_msg);
    tft.fillScreen(TFT_BLACK);
    uint8_t yPos = 150;
    uint16_t body_length = doc["num"];
    Serial.println(body_length);
    if(body_length > 0 && next_msg <= body_length){
        char temp[5];
        strcpy(temp, doc["More"]);
        if(strncmp(temp, "Y", 1)==0){
            more_unread = true;
        }
        char key[5];
        memset(key, 0, sizeof(key));
        itoa(next_msg, key, 10);
        char from[30];
        memset(from, 0, sizeof(from));
        strcpy(from, doc[key]["From"]);
        char subj[200];
        memset(subj, 0, sizeof(subj));
        strcpy(subj, doc[key]["Subject"]);
        char msg[1000];
        memset(msg, 0, sizeof(msg));
        strcpy(msg, doc[key]["Message"]);
        Serial.println(from);
        tft.setCursor(5, 3);
        tft.setTextColor(0x7BEF, TFT_BLACK);
        tft.println("From:");
        uint8_t new_y = tft.getCursorY() + 3;
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor(5, new_y);
        tft.println(from);
        new_y = tft.getCursorY() + 3;
        tft.setCursor(5, new_y);
        tft.setTextColor(0x7BEF, TFT_BLACK);
        tft.println("Subject:");
        new_y = tft.getCursorY() + 3;
        tft.setCursor(5, new_y);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.println(subj);
        new_y = tft.getCursorY() + 5;
        tft.setCursor(5, new_y);
        tft.println(msg);
        if(body_length > 1 && next_msg < body_length && next_msg != 1){
            tft.setTextColor(TFT_BLUE);
            tft.setCursor(3, 130);
            tft.println("RIGHT to View Last");
            tft.setCursor(3, 139);
            tft.println("DOWN to View Next");
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        else if(body_length > 1 && next_msg == body_length && more_unread == true){
            tft.setTextColor(TFT_BLUE);
            tft.setCursor(3, 130);
            tft.println("RIGHT to View Last");
            tft.setCursor(3, 139);
            tft.println("DOWN to View More");
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        else if(body_length > 1 && next_msg == body_length){
            tft.setTextColor(TFT_BLUE);
            tft.setCursor(3, 135);
            tft.println("RIGHT to View Last");
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        else if(body_length == 1 && more_unread == true){
            tft.setTextColor(TFT_BLUE);
            tft.setCursor(3, 135);
            tft.println("DOWN to GET More");
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        else if(body_length > 1 && next_msg == 1){
            tft.setTextColor(TFT_BLUE);
            tft.setCursor(3, 135);
            tft.println("DOWN to View Next");
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        if(next_msg < body_length){
            more_to_view = true;
        }
    }
}

void MailApp::draw_loading(){
    if(prev_state != LOADING_SCREEN){
        tft.fillScreen(TFT_BLACK);
        loading_animation.set_dimensions(64, 80, 12);
        prev_state = LOADING_SCREEN;
    }
    loading_animation.draw_screen();
}

uint8_t MailApp::handle_user_input() {
     //while in Mail app user can go back to LOGIN_MENU at any point by long press button 1
    if(button_input_one == 2){
        getSavedAccounts();
        prev_selected = 0;
        select_state = 0;
        prev_state = mail_state;
        mail_state = LOGIN_MENU;
    }
    //while in Mail app user can go back to OPTION_MENU at any point by long press button 1
    else if(button_input_two == 2 && user_email != NULL){
        checkUnreadNum();
        prev_selected = 0;
        select_state = 0;
        prev_state = mail_state;
        msg_index = 1;
        mail_state = OPTION_MENU;
    }
    //LOGIN_MENU gives use the option to select from a saved account on the server or log in to new email
    else if (mail_state == LOGIN_MENU){
        memset(user_email, 0, sizeof(user_email));
        DISPLAYING = false;
        if(loading && !request_sending){
            loading = false;
            storeSavedAccounts();
        }
        if(select_state == 0){
            if(num_saved_account > 0){
                select_state = 1;
            }
            else{
                select_state = 4;
            }
        }
        if(button_input_two == 1){
            prev_selected = select_state;
            select_state++;
            if(num_saved_account == 0){
                select_state = 4;
            }
            else if(select_state > num_saved_account){
                if(prev_selected == 4){
                    select_state = 1;
                }
                if(prev_selected == 1){
                    select_state = 4;
                }
            }
        }
        if(button_input_three == 1){
            if(select_state == 4){
                prev_selected = select_state;
                select_state = 0;
                prev_state = LOGIN_MENU;
                mail_state = USER_LOGIN;
            }
            else{
                setUserEmail(select_state);
                checkUnreadNum();
                timer = millis();
                prev_selected = select_state;
                select_state = 0;
                prev_state = LOGIN_MENU;
                mail_state = OPTION_MENU;
            }
        }
    }
    //user inputs a new email account to log in 
    else if(mail_state == USER_LOGIN){ 
        if(login_failed){
            if(button_input_three == 1){
                prev_state = LOGIN_MENU;
                login_failed = false;
            }
        }
        else if(keyboard.handle_user_input(button_input_one, button_input_two, button_input_three)){
            if(emailEntered == false){
                sprintf(email_temp, "%s", keyboard.get_current_message());
                keyboard.reset_message();
                emailEntered = true;
            }else{
                sprintf(other, "%s", keyboard.get_current_message());
                keyboard.reset_message();
                emailEntered = false;
                firstTime = true;
                loginEmail(email_temp, other);
            }
        }
        if(loading && !request_sending){
            loading = false;
            tft.fillScreen(TFT_BLACK);
            if(strncmp(response, "True", 4)==0){
            strcpy(user_email, email_temp);
            memset(email_temp, 0, sizeof(email_temp));
            checkUnreadNum();
            timer = millis();
            prev_state = USER_LOGIN;
            mail_state = OPTION_MENU;
            }
            else{
                login_failed = true;
                firstTime = true;
            }
        }
    }
    //once user has logged in / selected a save account navigates to OPTION_MENU where user selects what they want to do 
	else if (mail_state == OPTION_MENU){
        DISPLAYING = false;
        if(checkingNum && !request_sending){
            unread_num = atoi(response);
            checkingNum = false;
        }
        if(select_state == 0){
            if(unread_num > 0){
                select_state = SELECT_UNREAD;
            }
            else{
                select_state = SELECT_SEND;
            }
        }
        if(button_input_two == 1){
            prev_selected = select_state;
            select_state++;
            if(unread_num == 0){
                select_state = SELECT_SEND;
            }
            else if(select_state > SELECT_SEND){
                select_state = SELECT_UNREAD;
            }
		} 
        else if (button_input_three == 1) {
			if (select_state == SELECT_UNREAD) {
                //come back fix this, when enters display_mail in draw_screen email_json["num"] = 0
				checkUnreadMail();
                //int test = email_json["num"];
                //Serial.println("test");
                //Serial.println(test);
				mail_state = DISPLAY_MAIL;
				prev_state = OPTION_MENU;
			} else if (select_state == SELECT_SEND) {
                emailEntered = false;
                subjectEntered = false;
                messageEntered = false;
                memset(email_temp, 0, sizeof(email_temp));
                memset(other, 0, sizeof(other));
                memset(message, 0, sizeof(message));
				mail_state = SEND_MAIL;
				prev_state = OPTION_MENU;
			}
        }
		if (millis() - timer >= notif_period) {
			checkUnreadNum();
			timer = millis();
		}
	} 
    // user selects to send a mail
    else if (mail_state == SEND_MAIL) {
		if (keyboard.handle_user_input(button_input_one, button_input_two, button_input_three)) {
			if (emailEntered == false) {
				sprintf(email_temp, "%s", keyboard.get_current_message());
                keyboard.reset_message();
                emailEntered = true;
            } else if (subjectEntered == false) {
            	sprintf(other, "%s", keyboard.get_current_message());
            	keyboard.reset_message();
            	subjectEntered = true;
            } else {
                sprintf(message, "%s", keyboard.get_current_message()); 
                keyboard.reset_message();
                sendMail(email_temp, other, message);
                subjectEntered = false;
                emailEntered = false;
                messageEntered = false;
                memset(email_temp, 0, sizeof(email_temp));
                memset(other, 0, sizeof(other));
                memset(message, 0, sizeof(message));
                firstTime = true;
                mail_state = DISPLAY_MAIL;
                prev_state = SEND_MAIL; 
            }
		}
	} 
    // display mail returned by server
    else if(mail_state==DISPLAY_MAIL){
        if(sending && !request_sending){
            sending = false;
            prev_state = SEND_MAIL;
        }
        else if(loading && !request_sending){
            loading = false;
            prev_state = OPTION_MENU;
        }
        if (button_input_two == 1 && more_to_view == true){
            msg_index ++;
            msg_display_updated = false;
            more_to_view = false;
        } 
        else if (button_input_one == 1 && msg_index > 1){
            msg_index --;
            msg_display_updated = false;
        }
        else if(button_input_two == 1 && more_to_view != true && more_unread == true){
            mail_state = DISPLAY_MAIL;
            prev_state = OPTION_MENU;
        }
    }
    //long press button 3 to go to home app
    if (button_input_three == 2){
        return HOME; // switch to the home app
    } else {
        return NO_CHANGE; // stay on the mail app
    }
}

void MailApp::draw_screen() {
    if(mail_state == LOGIN_MENU){
        if(loading){
            draw_loading();
        }
        else{
            if (prev_state != LOGIN_MENU){
                tft.fillScreen(TFT_BLACK);
                prev_state = LOGIN_MENU;
                prev_selected = 0;
            }
            char notification[50];
            sprintf(notification, "%d accounts saved", num_saved_account);
            tft.setTextColor(TFT_RED, TFT_BLACK);
            tft.drawString(notification, 10, 5);
            tft.drawRect(2, 115, 122, 28, TFT_BLUE);
            if(prev_selected != select_state && prev_selected != 0){
                tft.fillRect(3, prev_selected*30-5, 120, 28, TFT_BLACK);
                tft.fillRect(3, select_state*30-5, 120, 28, TFT_GREEN);
                prev_selected = select_state;
            }
            else if(prev_selected == 0){
                tft.fillRect(3, select_state*30-5, 120, 28, TFT_GREEN);
                prev_selected = select_state;
            }
            //Serial.println("crash check 3");
            tft.setTextColor(TFT_WHITE);
            tft.drawString("NEW ACCOUNT", 32, 125);
            if(num_saved_account > 0){
                uint8_t yPos = 25;
                for(uint8_t i = 1; i <= num_saved_account; i++){
                    char key[5];
                    memset(key, 0, sizeof(key));
                    itoa(i, key, 10);
                    tft.drawRect(3, yPos, 122, 28, TFT_BLUE);
                    char draw_acct[50] = {NULL};
                    strcpy(draw_acct, account_json[key]);
                    //Serial.println("crash check 4");
                    uint8_t txt_y = yPos + 10;
                    tft.drawString(draw_acct, 5, txt_y);
                    yPos += 30;
                }
            }
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
    }
    else if(mail_state == USER_LOGIN){
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if(loading){
            draw_loading();
        }
        else{
            if(login_failed){
                if(firstTime == true){
                    tft.fillScreen(TFT_BLACK);
                    tft.setTextColor(TFT_RED, TFT_BLACK);
                    tft.setCursor(3,3);
                    tft.println(response);
                    tft.setTextColor(TFT_BLUE, TFT_BLACK);
                    tft.println("Short Press ENTER to try again");
                    tft.setCursor(3,tft.getCursorY()+5);
                    tft.println("Long Press RIGHT to return to Login Menu");
                    firstTime = false;
                }
            }
            else{
                if(prev_state != USER_LOGIN){
                 tft.fillScreen(TFT_BLACK);
                 prev_state = USER_LOGIN;
                }
                keyboard.draw_screen();
                if(emailEntered != true){
                    //enter email acocunt
                    tft.drawRect(5, 5, 115, 40, TFT_GREEN);
                    //tft.fillRect(5, 5, 115, 40, TFT_BLACK);
                    tft.drawString("Email:", 8, 8);
                    tft.drawString(keyboard.get_current_message(), 8, 18);
                    tft.drawRect(5, 50, 115, 40, TFT_WHITE);
                    tft.drawString("Password:", 8, 53);
                }
                else{
                    // enter password
                    tft.drawRect(5, 5, 115, 40, TFT_WHITE);
                    tft.drawString("Email:", 8, 8);
                    tft.setTextColor(0x7BEF, TFT_BLACK);
                    tft.drawString(email_temp, 8, 18);
                    tft.drawRect(5, 50, 115, 40, TFT_GREEN);
                    //tft.fillRect(5, 50, 115, 40, TFT_BLACK);
                    tft.setTextColor(TFT_WHITE, TFT_BLACK);
                    tft.drawString("Password:", 8, 53);
                    tft.drawString(keyboard.get_current_message(), 8, 63);
                    firstTime = true;
                }
            }
        }
    }

    else if (mail_state==OPTION_MENU){ 
        if(prev_state != OPTION_MENU){
            tft.fillScreen(TFT_BLACK);
            prev_state = OPTION_MENU;
        }
        char notification[200];
        tft.setTextColor(TFT_RED, TFT_BLACK);
        sprintf(notification, "%d new emails", unread_num);
        tft.drawString(notification, 30, 10);
        tft.setTextColor(TFT_WHITE);
        //check unread button
        if (unread_num > 0){
            tft.drawRect(8, 25, 114, 28, TFT_BLUE);
            tft.setTextColor(TFT_WHITE);
            tft.drawString("View All Unread", 20, 35);
        } else {
            tft.drawRect(8, 25, 114, 28, 0x7BEF);
            tft.setTextColor(0x7BEF);
            tft.drawString("View All Unread", 20, 35);
            tft.setTextColor(TFT_WHITE);
        }
        //send message button
        tft.drawRect(8, 55, 114, 28, TFT_BLUE);
        if(prev_selected != select_state && prev_selected != 0){
            tft.fillRect(8, prev_selected*30-5, 114, 28, TFT_BLACK);
            tft.fillRect(8, select_state*30-5, 114, 28, TFT_GREEN);
            prev_selected = select_state;
        }
        else if(prev_selected == 0){
            tft.fillRect(8, select_state*30-5, 114, 28, TFT_GREEN);
            prev_selected = select_state;
        }
        tft.setTextColor(TFT_WHITE);
        tft.drawString("Send Email", 32, 65);
    }

    else if (mail_state==SEND_MAIL){
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if(prev_state != SEND_MAIL){
             tft.fillScreen(TFT_BLACK);
             prev_state = SEND_MAIL;
        }
        keyboard.draw_screen();
        if(emailEntered != true){
            if(strlen(keyboard.get_current_message()) < strlen(prev_keyboard_input)){
                tft.fillRect(5, 5, 115, 25, TFT_BLACK);
            }
            tft.drawRect(5, 5, 115, 25, TFT_GREEN);
            tft.drawString("Recepient:", 10, 8);
            tft.drawRect(5, 35, 115, 60, TFT_WHITE);
            tft.drawString(keyboard.get_current_message(), 10, 18);
            sprintf(prev_keyboard_input, "%s", keyboard.get_current_message());
        }
        else if (emailEntered == true && subjectEntered != true){
            tft.drawRect(5, 5, 115, 25, TFT_WHITE);
            tft.drawString("Recepient:", 10, 8);
            tft.setTextColor(0x7BEF, TFT_BLACK);
            tft.drawString(email_temp, 8, 18);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            if(strlen(keyboard.get_current_message()) < strlen(prev_keyboard_input)){
                tft.fillRect(5, 35, 115, 60, TFT_BLACK);
            }
            tft.drawRect(5, 35, 115, 60, TFT_GREEN);
            tft.drawString("Subject:", 10, 37);
            tft.drawString(keyboard.get_current_message(), 10, 54);
            sprintf(prev_keyboard_input, "%s", keyboard.get_current_message());
            firstTime = true;
        }
        else if(emailEntered == true && subjectEntered == true && messageEntered != true && firstTime == true){
            tft.fillScreen(TFT_BLACK);
            firstTime = false;
            tft.drawRect(5, 5, 115, 80, TFT_GREEN);
            tft.drawString("Email Body:", 10, 8);
        }
        else if(emailEntered == true && subjectEntered == true && messageEntered != true){
            if(strlen(keyboard.get_current_message()) < strlen(prev_keyboard_input)){
                tft.fillRect(5, 5, 115, 80, TFT_BLACK);
                tft.drawRect(5, 5, 115, 80, TFT_GREEN);
                tft.drawString("Email Body:", 10, 8);
            }
            tft.drawString(keyboard.get_current_message(), 10, 18);
            sprintf(prev_keyboard_input, "%s", keyboard.get_current_message());
        }
    } 
    //fix issues with displaying unread when there is only one mail left blank screen 
    else if (mail_state==DISPLAY_MAIL){
        if(sending || loading){
            draw_loading();
        }
        else{
            if (prev_state != DISPLAY_MAIL){
                tft.fillScreen(TFT_BLACK);
            }
            if (prev_state == OPTION_MENU){
                //checkUnreadMail();
                Serial.println("deserializing unread emails");
                deserializeJson(email_json, response);
                DISPLAYING = true;
                mail_parsing(email_json, 1);
            }
            else if (prev_state == SEND_MAIL){
                tft.setCursor(5, 5);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.println(response);
                tft.setTextColor(TFT_BLUE);
                tft.setCursor(3, 30);
                tft.println("Long Press DOWN to go back");
                tft.setCursor(3,tft.getCursorY()+5);
                tft.println("Long Press RIGHT to go to Login Menu");
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
            }
            else if (msg_display_updated != true && DISPLAYING == true){
                mail_parsing(email_json, msg_index);
                msg_display_updated = true;
            }
            prev_state = DISPLAY_MAIL;
        }
    }
}
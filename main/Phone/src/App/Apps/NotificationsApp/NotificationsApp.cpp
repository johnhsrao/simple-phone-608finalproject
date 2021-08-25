#include "Arduino.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Phone.h"
#include <ArduinoJson.h> // Library to deserialize json
#include "App/App.h"
#include "NotificationsApp.h"

NotificationsApp::NotificationsApp() {  
    state_change = false;
    loading = false;
    timer = millis();
}

uint8_t NotificationsApp::handle_user_input() {
    if (button_input_three == 1 || button_input_three == 2) {
        return REMOVE_NOTIFICATION;
    }

    if (millis() - show_timer >= SHOW_PERIOD) {
        return REMOVE_NOTIFICATION;
    }

    return NO_CHANGE;
}

bool NotificationsApp::check_notifications() {
    if (millis() - timer >= GETTING_PERIOD && !request_sending && !loading) {
        // check notifications
        char destination[115];
        sprintf(destination, "http://608dev-2.net/sandbox/sc/team28/final_project/python/Notification/notification.py?user=%s", username);
        http_request(
            "GET",
            "",
            destination,
            "608dev-2.net"
        );
        loading = true;
        timer = millis();
    }

    if (loading && !request_sending) {
        deserialize_notifications_json();
        loading = false;
        if (message_num != 0 || mail_num != 0) {
            state_change = true;
            show_timer = millis();
            return true;
        }
    }

    return false;
}

void NotificationsApp::deserialize_notifications_json() {
	// Deserialize the JSON document
	StaticJsonDocument<1000> doc;
	DeserializationError error = deserializeJson(doc, response);

	// Test if parsing succeeds.
	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}

    message_num = doc["messageNum"];
    mail_num = doc["mailNum"];

    if (message_num == 1) {
        sprintf(message_from, doc["messageFrom"]);
        Serial.println(message_from);
    }
    if (mail_num == 1) {
        sprintf(mail_from, doc["mailFrom"]);
        Serial.println(mail_from);
    }
}

void NotificationsApp::draw_screen() {
    if (state_change) {
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE, TFT_KEY_GRAY);
        tft.fillRect(0, 0, 128, 60, TFT_KEY_GRAY);

        char notification[21];
        if (message_num == 1 && mail_num == 0) {
            tft.drawString("unread message from", 4, 12);
            strncpy(notification, message_from, 20);
            tft.drawString(notification, 4, 22);
        } else if (message_num == 1 && mail_num == 1) {
            tft.drawString("unread message from", 4, 2);
            strncpy(notification, message_from, 20);
            tft.drawString(notification, 4, 12);

            tft.drawString("unread email from", 4, 22);
            if (strlen(mail_from) <= 20) {
                strncpy(notification, mail_from, 20);
            } else {
                strncpy(notification, mail_from, 16);
                sprintf(notification + 16, " ...");
            }
            tft.drawString(notification, 4, 32);
        } else if (message_num == 1 && mail_num > 1) {
            tft.drawString("unread message from", 4, 5);
            strncpy(notification, message_from, 20);
            tft.drawString(notification, 4, 15);

            sprintf(notification, "%d unread emails", mail_num);
            tft.drawString(notification, 4, 28);
        } else if (message_num == 0 && mail_num == 1) {
            tft.drawString("unread email from", 4, 12);
            if (strlen(mail_from) <= 20) {
                strncpy(notification, mail_from, 20);
            } else {
                strncpy(notification, mail_from, 16);
                sprintf(notification + 16, " ...");
            }
            tft.drawString(notification, 4, 22);
        } else if (message_num > 1 && mail_num == 1) {
            sprintf(notification, "%d unread messages", message_num);
            tft.drawString(notification, 4, 5);

            tft.drawString("unread email from", 4, 18);
            if (strlen(mail_from) <= 20) {
                strncpy(notification, mail_from, 20);
            } else {
                strncpy(notification, mail_from, 16);
                sprintf(notification + 16, " ...");
            }
            tft.drawString(notification, 4, 28);
        } else if (message_num > 1 && mail_num == 0) {
            sprintf(notification, "%d unread messages", message_num);
            tft.drawString(notification, 4, 17);
        } else if (message_num == 0 && mail_num > 1) {
            sprintf(notification, "%d unread emails", mail_num);
            tft.drawString(notification, 4, 17);
        } else if (message_num > 1 && mail_num > 1) {
            sprintf(notification, "%d unread messages", message_num);
            tft.drawString(notification, 4, 8);

            sprintf(notification, "%d unread emails", mail_num);
            tft.drawString(notification, 4, 24);
        }
        
        tft.fillRect(39, 44, 50, 12, TFT_WHITE);
        tft.setTextColor(TFT_BLACK, TFT_WHITE); // set color of font to black foreground, white background
        tft.drawString("dismiss", 39+4, 44+2);

        tft.drawRect(38, 43, 52, 14, TFT_BLACK); // highlight the button

        state_change = false; // so the notification does not flash
    }
}
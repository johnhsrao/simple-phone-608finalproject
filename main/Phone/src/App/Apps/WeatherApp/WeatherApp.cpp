#include "Arduino.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "Phone.h"
#include "App/App.h"
#include "WeatherApp.h"
#include <ArduinoJson.h>
#include "string.h"

WeatherApp::WeatherApp() {  
    // background color
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // set color of font to white foreground, black background
    SELECT_MENU = 0;
    SELECT_PLACE = 1; 
    FAVORITES = 2;
    DISPLAY_INFO = 3;
    ERROR = 4;
    LOADING_SCREEN = 5;
    input[100];
    weather_state = 0;
    prev_state = 0;
    responsedoc;
    city[100];
    humidity[100] = {0};
    weather[100] = {0};
    
    fave1[100] = {0};
    fave2[100] = {0};
    fave3[100] = {0};
    fave4[100] = {0};
    fave5[100] = {0};
    location_state = 0;
    prev_loc_state = location_state;
    temperature[100] = {0};
    pointers_char[300] = {0};
    loading = false;
    gotWeather = false;
    parsed = false;
    local = false;
    
}

void WeatherApp::draw_loading(){
    if(prev_state != LOADING_SCREEN){
        tft.fillScreen(TFT_BLACK);
        loading_animation.set_dimensions(64, 80, 12);
        prev_state = LOADING_SCREEN;
    }
    loading_animation.draw_screen();
}

uint8_t WeatherApp::handle_user_input() {
    if(weather_state == SELECT_MENU){
        
        if(loading && !request_sending && parsed){
            parsed = false;
            loading = false;
            if(gotWeather){
                weather_state = DISPLAY_INFO;
                prev_state = SELECT_MENU;
            }else{
                weather_state = FAVORITES;
                prev_state = SELECT_MENU;
            }
            gotWeather = false;
        }
        if(loading && !request_sending && !parsed){
            if(gotWeather){
                parseWeather(city, local);
                local = false; 
                parsed = true;
            }else{
                parseResponse(response);
                parsed = true;
            }
            
        }
        if(button_input_one > 0 && !loading){
            getLocalWeather(lat, lon);
            local = true;
            loading = true;
            sprintf(city, "Your location");
            gotWeather = true;
        }
        else if(button_input_two > 0 && !loading){

            weather_state = SELECT_PLACE;
            prev_state = SELECT_MENU;

        }else if (button_input_three == 1 && !loading){
            char get_query[300];
            sprintf(get_query, "user=%s", username);
            http_request("GET", get_query, "/sandbox/sc/team28/final_project/python/WeatherApp/WeatherApp.py", "608dev-2.net");
            loading = true; 
            
        }
    }
    else if(weather_state == SELECT_PLACE){
        if(loading && !request_sending && parsed){
            parsed = false;
            loading = false;
            weather_state = DISPLAY_INFO;
            prev_state = SELECT_PLACE;
        }

        if(loading && !request_sending && !parsed){
            parseWeather(city, local);
            parsed = true;
        }

        if(keyboard.handle_user_input(button_input_one, button_input_two, button_input_three) && !loading && !request_sending){
            
            sprintf(input, "%s", keyboard.get_current_message());
            keyboard.reset_message();
            sprintf(city, "%s", input);
            getWeather(input);
            loading = true;
            memset(input, 0, strlen(input));
            
        }
    }else if (weather_state == FAVORITES){

        if(loading && !request_sending && parsed){
            parsed = false;
            loading = false;
            weather_state = DISPLAY_INFO;
            prev_state = FAVORITES;
        }
        if(loading && !request_sending && !parsed){
            parseWeather(city, local);
            parsed = true;
        }
        if (button_input_one > 0){
            weather_state = SELECT_MENU;
            prev_state = FAVORITES;
        }
        //get the cities from the python script here and assign to the variables like fave1, fave2, etc
        //pch = strtok(response, delimiter);
        
        else if (button_input_two > 0){
            location_state++;
            location_state %= 5;
        }
        else if (button_input_three == 1 && !loading){
            if (location_state == 0){
                Serial.println(fave1);
                getWeather(fave1);
                loading = true;
                sprintf(city, fave1);
            }else if (location_state == 1){
                getWeather(fave2);
                loading = true;
                sprintf(city, fave2);
            }else if (location_state == 2){
                getWeather(fave3);
                loading = true;
                sprintf(city, fave3);
            }else if (location_state == 3){
                getWeather(fave4);
                loading = true;
                sprintf(city, fave4);
            }else if (location_state == 4){
                getWeather(fave5);
                loading = true;
                sprintf(city, fave5);
            }
            
        }
    }
    else if(weather_state == ERROR){
        Serial.println("we are in the error state");
        if((button_input_one> 0) || (button_input_two > 0) || (button_input_three ==  1)){
            weather_state = SELECT_MENU;
            prev_state = ERROR;
        }
    }
    
    else if(weather_state == DISPLAY_INFO){
        if((button_input_one> 0) || (button_input_two > 0) || (button_input_three ==  1)){
            weather_state = SELECT_MENU;
            prev_state = DISPLAY_INFO;
            memset(city, 0, strlen(city));
        }
    }
    if (button_input_three == 2) {
        return HOME; // switch to the home app
    } else {
        return NO_CHANGE; // stay on the weather app
    }
}

void WeatherApp:: parseResponse(char* response){
    memset(fave1, 0, 100);
    memset(fave2, 0, 100);
    memset(fave3, 0, 100);
    memset(fave4, 0, 100);
    memset(fave5, 0, 100);
    const char delimiter[] = "&";
    char *pch = strtok(response, delimiter);
    int counter = 0;
    while (pch != NULL) {
        pointers_char[counter] = *pch;
        if (counter == 0) {
        strcpy(fave1, pch);
        }
        if (counter == 1) {
        strcpy(fave2, pch);
        }
        if (counter == 2) {
        strcpy(fave3, pch);
        }
        if (counter == 3) {
        strcpy(fave4, pch);
        }
        if (counter == 4) {
        strcpy(fave5, pch);
        }
        pch = strtok(NULL, delimiter);
        counter++;
    }

}

void WeatherApp::getWeather(char* input){
    char query[300];
    if (strlen(input) == 0){
        input = "Boston";
    }
    sprintf(query, "city=%s", input);
    http_request("GET",
        query,
        "/sandbox/sc/team28/final_project/python/WeatherApp/OpenWeather.py",
        "608dev-2.net");
}

void WeatherApp::getLocalWeather(double lat, double lon){
    char query[300];
    sprintf(query, "lat=%f&lon=%f", lat, lon);
    Serial.println(query);
    http_request("GET",
        query,
        "/sandbox/sc/team28/final_project/python/WeatherApp/OpenWeather.py",
        "608dev-2.net");
}

void WeatherApp::parseWeather(char* input, bool local){
    char comparison[] = "ERROR";
    const char delimiter[] = "&";
    if (strstr(response, comparison) != NULL){
        Serial.println("in the new if");
        weather_state = ERROR;
    }
    else{
        Serial.println("in the new else");
        char *pch = strtok(response, delimiter);
        pch = strtok(NULL, delimiter);
        sprintf(temperature,"%s", pch);
        pch = strtok(NULL, delimiter);
        sprintf(humidity, "%s %%", pch);
        pch = strtok(NULL, delimiter);
        sprintf(weather, "%s", pch);
        char post_query[300];
        if(!local){
            sprintf(post_query, "user=%s&location=%s&count=1", username, input);
            http_request("POST", post_query, "/sandbox/sc/team28/final_project/python/WeatherApp/WeatherApp.py", "608dev-2.net");
        }
    }
}

void WeatherApp::printWeather(){
    
    tft.drawString("Temperature: ", 5, 40);
    tft.drawString(temperature, 5, 50);
    tft.drawString("Weather: ", 5, 80);
    tft.drawString(weather, 5, 90);
    tft.drawString("Humidity: ", 5, 120);
    tft.drawString(humidity, 5, 130);
}

void WeatherApp::draw_screen() {
    
    if (weather_state == SELECT_MENU){
        if(loading){
            draw_loading();
        }
        else{
            if(prev_state != SELECT_MENU){
                tft.fillScreen(TFT_BLACK);
                prev_state = SELECT_MENU;
            }
            tft.drawString("Weather App", 30, 5);
            tft.drawString("Press button 1 to", 10, 20);
            tft.drawString("see your weather", 10, 30);
            tft.drawString("Press button 2 to", 10, 50);
            tft.drawString("see another city's", 10, 60);
            tft.drawString("weather", 10, 70);
            tft.drawString("Press button 3 to", 10, 90);
            tft.drawString("see favorites", 10, 100);
        }
        
    }
    else if(weather_state == SELECT_PLACE){
        if(loading){
            draw_loading();
        }
        else{
            if(prev_state != SELECT_PLACE){
                tft.fillScreen(TFT_BLACK);
                prev_state = SELECT_PLACE;
            }
            keyboard.draw_screen();
            tft.drawString("Enter city name", 5, 10);
            tft.drawString("City: ", 5, 40);
            tft.drawString(keyboard.get_current_message(), 40, 40);
        }
    }
    else if (weather_state == FAVORITES){
        if(loading){
            draw_loading();
        }
        else{
            if(prev_state != FAVORITES){
                tft.fillScreen(TFT_BLACK);
                prev_state = FAVORITES;
            }
            tft.drawString(fave1, 5, 5);
            tft.drawString(fave2, 5, 15);
            tft.drawString(fave3, 5, 25);
            tft.drawString(fave4, 5, 35);
            tft.drawString(fave5, 5, 45);
            
            if (location_state == 0){
                tft.drawString(" ", 80, 45);
                tft.drawString("<", 80, 5);
            }else if (location_state == 1){
                tft.drawString(" ", 80, 5);
                tft.drawString("<", 80, 15);
            }else if (location_state == 2){
                tft.drawString(" ", 80, 5);
                tft.drawString(" ", 80, 15);
                tft.drawString("<", 80, 25);
            }else if (location_state == 3){
                tft.drawString(" ", 80, 5);
                tft.drawString(" ", 80, 15);
                tft.drawString(" ", 80, 25);
                tft.drawString("<", 80, 35);
            }else if (location_state == 4){
                tft.drawString(" ", 80, 5);
                tft.drawString(" ", 80, 15);
                tft.drawString(" ", 80, 25);
                tft.drawString(" ", 80, 35);
                tft.drawString("<", 80, 45);
            }
        }
        
    }
    else if (weather_state == ERROR){
        loading = false;
        parsed = false;
        if(prev_state != ERROR){
            tft.fillScreen(TFT_BLACK);
            prev_state = ERROR;
        }
        tft.drawString("incorrect city input", 5, 5);
        tft.drawString("press any button", 5, 25);
        tft.drawString("to go back", 5, 35);
    }

    else if(weather_state == DISPLAY_INFO){
        if (prev_state != DISPLAY_INFO){
            tft.fillScreen(TFT_BLACK);
            prev_state = DISPLAY_INFO;
        }
        tft.drawString("Current City: ", 5, 5);
        tft.drawString(city, 5, 15);
        printWeather();
    }
    
}
void WeatherApp::reset_screen() {
  tft.fillScreen(TFT_BLACK);
}





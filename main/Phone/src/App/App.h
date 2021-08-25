/*
  App.h - Library for the App superclass. Handles user input and HTTP requests.
*/
#ifndef App_h
#define App_h
#include <memory>
#include "Arduino.h"
#include "Inputs/Button/Button.h"

class App {
  private:
    // static http request functions
    static void send_http_request(void *pvParameters);
    static void send_https_request(void *pvParameters);
    static uint8_t char_append(char* buff, char c, uint16_t buff_size);
    // static http request variables
    static uint16_t response_size;
    static uint16_t response_timeout;
    static bool serial;
    // non-static http request variables
    char request[4000];
    char host[25];
    // non-static location functions
    void generate_wifi_json(int n, char* json_body);
    int wifi_object_builder(char* object_string, uint32_t os_len, uint8_t channel, int signal_strength, uint8_t* mac_address);
  protected:
    // store username from login
    static char username[];
    // current location
    static double lat;
    static double lon;
    // boolean to check if request is done sending
    static bool request_sending;
    // user button inputs
    uint8_t button_input_one;
    uint8_t button_input_two;
    uint8_t button_input_three;
    // non-static http request variable and function
    char response[4000];
    void http_request(char* type, char* body, char* destination, char* host, bool secure = false, bool json = false);
    // get and process current location
    void get_current_location();
    void deserialize_location_json();
  public:
    App();
    void get_user_input();
    virtual uint8_t handle_user_input();
    virtual void draw_screen();
    virtual void reset_screen();
};

#endif
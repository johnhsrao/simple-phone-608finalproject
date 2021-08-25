/*
  Phone.h - Library for the Phone class. Handles the state machine and memory
            allocation for the phone. Stores an instance of the current app open.
*/
#ifndef TFT_HOME_SCREEN
#define TFT_HOME_SCREEN 0x6dbf
#endif

#ifndef TFT_KEY_GRAY
#define TFT_KEY_GRAY 0x6b4d
#endif

#ifndef Phone_h
#define Phone_h
#include <memory>
#include "Arduino.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> // Used in support of TFT Display
#include "App/App.h"
#include "App/Apps/NotificationsApp/NotificationsApp.h"
#include "App/Inputs/Button/Button.h"
#include "App/Inputs/Keyboard/Keyboard.h"
#include "App/Utilities/Loading/Loading.h"

// screen
static TFT_eSPI tft = TFT_eSPI();

// buttons
static Button button_one = Button(19);
static Button button_two = Button(5);
static Button button_three = Button(3);

// keyboard
static Keyboard keyboard = Keyboard();

// loading animation
static Loading loading_animation = Loading();

// phone states
static const uint8_t HOME = 0;
static const uint8_t MESSAGING = 1;
static const uint8_t MAIL = 2;
static const uint8_t CLOCK = 3;
static const uint8_t WEATHER = 4;
static const uint8_t GAME = 5;
static const uint8_t MUSIC = 6;
static const uint8_t INTERNET = 7;
static const uint8_t NO_CHANGE = 8;
static const uint8_t LOGIN = 9;
static const uint8_t SHOW_NOTIFICATION = 10;
static const uint8_t REMOVE_NOTIFICATION = 11;

// google ca cert
static const char* GOOGLE_CA_CERT = \
                      "-----BEGIN CERTIFICATE-----\n" \
                      "MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
                      "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
                      "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
                      "MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
                      "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
                      "hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
                      "v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
                      "eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
                      "tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
                      "C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
                      "zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
                      "mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
                      "V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
                      "bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
                      "3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
                      "J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
                      "291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
                      "ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
                      "AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
                      "TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
                      "-----END CERTIFICATE-----\n";

// google wifi localization api
static const char GOOGLE_API_KEY[] = "AIzaSyBzMBtpfat0bACF8Exz3Tq-3ttU7eyAgqE";

class Phone {
    private:
        uint8_t state;
        uint8_t previous_state;
        // apps
        std::unique_ptr<App> login_app;
        std::unique_ptr<NotificationsApp> notifications_app;
        std::unique_ptr<App> home_app;
        std::unique_ptr<App> messaging_app;
        std::unique_ptr<App> mail_app;
        std::unique_ptr<App> clock_app;
        std::unique_ptr<App> weather_app;
        std::unique_ptr<App> game_app;
        std::unique_ptr<App> music_app;
        std::unique_ptr<App> internet_app;
        void setup();
    public:
        Phone();
        void loop();
};

#endif
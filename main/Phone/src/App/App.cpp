#include <memory>
#include <string.h> // Library to manipulate strings
#include "Arduino.h"
#include <WiFi.h> // Connect to WiFi Network
#include <WiFiClientSecure.h> // For sending secure API requests
#include <ArduinoJson.h> // Library to deserialize json
#include "Phone.h"
#include "App.h"
#include "Inputs/Button/Button.h"

App::App() {
  // user button inputs
  button_input_one = 0;
  button_input_two = 0;
  button_input_three = 0;
}

// static class members
char App::username[19];
bool App::request_sending = false;
uint16_t App::response_size = 1000;
uint16_t App::response_timeout = 4000;
bool App::serial = true;
double App::lat = 0.00;
double App::lon = 0.00;

void App::get_user_input() {
  button_input_one = button_one.update_state();
  button_input_two = button_two.update_state();
  button_input_three = button_three.update_state();
}

uint8_t App::handle_user_input() {
  if (button_input_three == 2) {
    return 0; // return to the home app
  } else {
    return 8; // stay on the current app
  }
}

void App::draw_screen() {
  Serial.println("Parent class draw screen");
}

void App::reset_screen() {
  tft.fillScreen(TFT_BLACK);
}

void App::http_request(char* type, char* body, char* destination, char* http_host, bool secure, bool json) {
  // the host, destination, type, body, will be determined by the different app that needs to send a request
  
  // format the request
  int offset = 0;
  if (strcmp(type, "GET") == 0) {
    if (strlen(body) > 0) {
      offset += sprintf(request + offset, "GET %s?%s HTTP/1.1\r\n", destination, body);
    } else {
      offset += sprintf(request + offset, "GET %s HTTP/1.1\r\n", destination);
    }
    offset += sprintf(request + offset, "Host: %s\r\n", http_host);
    offset += sprintf(request + offset, "\r\n");
  } else if (strcmp(type, "POST") == 0) {
    offset += sprintf(request + offset, "POST %s HTTP/1.1\r\n", destination);
    offset += sprintf(request + offset, "Host: %s\r\n", http_host);
    if (json) {
      offset += sprintf(request + offset, "Content-Type: application/json\r\n");
    } else {
      offset += sprintf(request + offset, "Content-Type: application/x-www-form-urlencoded\r\n");
    }
    offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", strlen(body));
    offset += sprintf(request + offset, "%s\r\n", body);
  }
  
  Serial.println("request formatted");

  // copy the host string since it will go out of scope
  strcpy(host, http_host);

  TaskHandle_t HttpTask;

  request_sending = true;
  Serial.println("starting to create task");
  if (secure) {
    xTaskCreatePinnedToCore(
      send_https_request,
      "HttpsTask", 
      10000,
      this,
      1,
      &HttpTask,
      0
    );
  } else {
    xTaskCreatePinnedToCore(
      send_http_request,
      "HttpTask", 
      10000,
      this,
      1,
      &HttpTask,
      0
    );
  }
  Serial.println("task created");
}

uint8_t App::char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

void App::send_http_request(void *pvParameters) {
  Serial.println("trying to send request");
  App* app = (App*) pvParameters;
  // send the request
  WiFiClient client; // WiFiClient object
  uint32_t timer = millis();
  if (client.connect(app->host, 80)) { // try to connect to host on port 80
    if (serial) Serial.println(app->request); // Can do one-line if statements in C without curly braces
    client.print(app->request);
    memset(app->response, 0, response_size); // Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { // while we remain connected read out data coming back
      client.readBytesUntil('\n', app->response, response_size);
      if (serial) Serial.println(app->response);
      if (strcmp(app->response,"\r")==0) { // found a blank line!
        break;
      }
      memset(app->response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(app->response, 0, response_size); 
    count = millis();
    while (client.available()) { // read out remaining text (body of response)
      char_append(app->response, client.read(), response_size);
    }
    if (serial) Serial.println(app->response);
    client.stop();
    uint32_t time_elapsed = millis() - timer;
    Serial.print("Time: ");
    Serial.print(time_elapsed);
    Serial.println();
    if (serial) Serial.println("-----------");  
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }

  request_sending = false;
  vTaskDelete(NULL);
}

void App::send_https_request(void *pvParameters) {
  Serial.println("trying to send request");
  App* app = (App*) pvParameters;
  WiFiClientSecure client; // WiFiClient Secure object
  client.setCACert(GOOGLE_CA_CERT); // set cert for https
  if (client.connect(app->host,443)) { // try to connect to host on port 443
    if (serial) Serial.print(app->request);// Can do one-line if statements in C without curly braces
    client.print(app->request);
    //app->response[0] = '\0';
    memset(app->response, 0, response_size); // Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { // while we remain connected read out data coming back
      client.readBytesUntil('\n', app->response, response_size);
      if (serial) Serial.println(app->response);
      if (strcmp(app->response, "\r") == 0) { // found a blank line!
        break;
      }
      memset(app->response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(app->response, 0, response_size);
    count = millis();
    while (client.available()) { // read out remaining text (body of response)
      char_append(app->response, client.read(), response_size);
    }
    if (serial) Serial.println(app->response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }

  request_sending = false;
  vTaskDelete(NULL);
}

void App::get_current_location() {
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    // create json object containing all wifi networks for the Google Geolocation API
    char json_body[3000];
    generate_wifi_json(n, json_body);
    char destination[100];
    sprintf(destination, "https://www.googleapis.com/geolocation/v1/geolocate?key=%s", GOOGLE_API_KEY);
    http_request(
      "POST", 
      json_body,
      destination,
      "googleapis.com",
      true,
      true
    );
  }
}

void App::generate_wifi_json(int n, char* json_body) {
  int offset = sprintf(json_body, "{\"wifiAccessPoints\":[");
  int max_networks = max(min(30, n), 1);
  for (int i = 0; i < max_networks; ++i) { 
    uint8_t* mac = WiFi.BSSID(i);
    offset += wifi_object_builder(json_body + offset, 3000-offset, WiFi.channel(i), WiFi.RSSI(i), WiFi.BSSID(i));
    if (i != max_networks - 1){
      offset += sprintf(json_body + offset, ","); // add comma between json entries except trailing.
    }
  }
  sprintf(json_body + offset, "]}");
}

int App::wifi_object_builder(char* object_string, uint32_t os_len, uint8_t channel, int signal_strength, uint8_t* mac_address) {
  char wifi_access_point[100];
  char mac_address_str[30];
  int offset = 0;
  for (int i = 0; i < 6; i++) {
    offset += sprintf(mac_address_str + offset, "%x", mac_address[i]);
    if (i != 5) {
      offset += sprintf(mac_address_str + offset, ":");
    }
  }
  offset = 0;
  offset += sprintf(wifi_access_point, "{\"macAddress\":\"%s\",", mac_address_str); // mac address
  offset += sprintf(wifi_access_point + offset, "\"signalStrength\":%d,", signal_strength); // signal strength
  offset += sprintf(wifi_access_point + offset, "\"age\":0,"); // age
  offset += sprintf(wifi_access_point + offset, "\"channel\":%d}", channel); // channel
  if (strlen(wifi_access_point) <= os_len) {
    sprintf(object_string, wifi_access_point);
    return strlen(wifi_access_point);
  }
  return 0;
}

void App::deserialize_location_json() {
  // if response_buffer has anything extra beyond the the json then remove it
  char *p1 = strchr(response, '{'); // start of the json
  char *p2 = strrchr(response, '}'); // end of the json
  int len = p2-p1+1;
  char response_stripped[150];
  strncpy(response_stripped, p1, len); // copy the entire length of the json

  // Deserialize the JSON document
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, response_stripped);
  
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // set the latitude and longitude
  lat = doc["location"]["lat"];
  lon = doc["location"]["lng"];
}
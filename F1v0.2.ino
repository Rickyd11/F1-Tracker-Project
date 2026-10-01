// LIbraies needed for the project
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
// Custom folder that has circuit information
#include "tracks/tracks.h"

//WiFi Crditional
const char* ssid = "Your WiFi ";
const char* password = "Your WiFi Password";

// Create the CYD TFT display object
TFT_eSPI tft = TFT_eSPI();

// Screen Dimension
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// Header
#define HEADER_Y 0
#define HEADER_H 40

// Main Panels
#define MAIN_X 8
#define MAIN_Y 48
#define MAIN_W 204
#define MAIN_H 180

// Side Panel
#define SIDE_X 220
#define SIDE_Y 92
// Right panels
#define SIDE_X 220
#define SIDE_W 92

// Session Panel
#define SESSION_Y 48
#define SESSION_H 78

// Weather Pannel
#define WEATHER_Y 136
#define WEATHER_H 92

//Color's for the screen display but unfortunaley for this project 
//Colors
#define ui_black TFT_BLACK // White going to swap eventually
#define ui_yellow TFT_RED // Yellow
#define ui_navy TFT_NAVY // Light Blue
#define ui_white TFT_WHITE // Black going to swap eventually
#define ui_purple TFT_DARKGREEN // Purple
#define ui_silver TFT_SILVER // Silver
#define ui_bronze TFT_GOLD // Bronse/ brown

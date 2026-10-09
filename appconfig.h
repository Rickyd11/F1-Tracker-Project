#pragma once

#include <Arduino.h>

//Wifi creditional
inline const char* ssid = "Your WiFi SSID";
inline const char* password= "Your Wifi Password";

// Open F1 api link 
inline const char* openF1= "https://api.openf1.org/v1";

// Aplication timers
inline const unsigned long UPDATE_INTERVAL = 600000; // 10 min timer
inline const unsigned long TOUCH_DEBOUNCE = 500;

// Number of drivers that the program counts
#define MAX_DRIVERS 5

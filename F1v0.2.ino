// Libraries needed for the project
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "config/appconfig.h"
#include "data/F1Data.h"
#include "api/F1Client.h"
#include "ui/UI.h"
#include "input/TouchHandler.h"
// Custom folder that has circuit information
#include "tracks/tracks.h"



void setup() {

  Serial.begin(115200);

  delay(500);

  Serial.println("Slick Rick's F1 Tracker");

  // Display 
  initializeDisplay();
  showBootScreen();

  // Touch
  initializeTouch();

  //WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin( ssid, password);

  while(WiFi.status() != WL_CONNECTED){

    delay(500);
    Serial.print(".");

  }

  Serial.println("Wifi Connected");
  Serial.print("Ip: ");
  Serial.println(WiFi.localIP());

  // NTP
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  delay(2000);

  //OpenF1

  if(updateF1Data()){
    Serial.println("F1 data loaded.");
  }else{
    Serial.println("F1 data failed.");
  }

  currentScreen = SCREEN_LEADERBOARD;

  drawCurrentScreen();
}

void loop() {

  // Touch
  handleTouch();

  // Periodic Update
  static unsigned long lastUpdate = 0;

  if(millis() - lastUpdate >= UPDATE_INTERVAL){

    Serial.println("Refreshing F1 data...");
    if(updateF1Data()){
      
      drawCurrentScreen();

    }else{
      Serial.print("F1 update failed.");

    }
    lastUpdate = millis();
  }
  delay(10);
}

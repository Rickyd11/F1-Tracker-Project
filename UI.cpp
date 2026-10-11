#include "ui/UI.h"

#include "config/displayconfig.h"

#include "ui/LeaderboardScreen.h"
#include "ui/CircuitScreen.h"
#include "ui/FastestLapScreen.h"
#include "ui/TrackScreen.h"

// Create the CYD TFT display object
TFT_eSPI tft = TFT_eSPI();

Screen currentScreen = SCREEN_LEADERBOARD;

void initializeDisplay(){

    tft.init();
    
    tft.setRotation(3);

    tft.fillScreen(ui_black);

    pinMode(21, OUTPUT);

    digitalWrite(21, HIGH);
}

// Boot screen
void showBootScreen(){

    // Clear the display
  tft.fillScreen(ui_black);

     // arc 1
    tft.drawSmoothArc(127, 74, 36, 26, 1, 270, ui_purple, 0x0, true);
    // string 3
    tft.setTextColor(ui_navy);
    tft.setTextSize(1);
    tft.setFreeFont(&FreeMono12pt7b);
    tft.drawString("V0.2", 99, 124);
    // string 2
    tft.setTextSize(2);
    tft.setFreeFont(NULL);
    tft.drawString("Slick Rick", 69, 66);

  delay(1500);
}

// Draws Current Screen
void drawCurrentScreen(){

    tft.fillScreen(ui_black);

    switch(currentScreen){

        case SCREEN_LEADERBOARD: 
            drawLeaderboard();
            break;
        
        case SCREEN_CIRCUIT:
            drawCircuitInfo();
            break;

        case SCREEN_FASTEST_LAP:
            drawFastestLap();
            break;
        case SCREEN_TRACK:
            drawCurrentTrack();
            break;
    }
}

void nextScreen(){

    currentScreen =  static_cast<Screen>((currentScreen + 1) % SCREEN_COUNT);

    drawCurrentScreen();
}

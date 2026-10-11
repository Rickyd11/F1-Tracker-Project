#include "ui/FastestLapScreen.h"
#include "ui/UI.h"

#include "data/F1Data.h"
#include "config/displayconfig.h"

void drawFastestLap(){
    tft.fillScreen(ui_black);
    // Header
    tft.fillRect(0,0, SCREEN_WIDTH + 100, HEADER_H, ui_red);

  //Header Title
    tft.setTextColor(ui_black);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("Fastest Laps");

    // Fastest laps
    int y = 55;

    for (int i = 0; i < fastestLapCount; i++){
      
        // Position
        tft.setTextColor(ui_white);
        tft.setTextSize(2);
        tft.setCursor(10, y);
        tft.print(i + 1);

        // Driver name
        tft.setCursor(40, y);
        tft.print(fastestLaps[i].driverName);

        // Lap time
        tft.setTextSize(1);
        tft.setCursor(225, y + 5);
        tft.print(fastestLaps[i].formattedTime);

        // Debug
        Serial.print("Fastest Lap Display: ");
        Serial.print(i + 1);
        Serial.print(" ");
        Serial.print(fastestLaps[i].driverName);
        
        Serial.print(" ");
        Serial.println(fastestLaps[i].formattedTime);

        y += 30;
    }

    // No data
    if (fastestLapCount == 0){
        tft.setTextColor(ui_white);
        tft.setTextSize(2);

        tft.setCursor(20, 100);
        tft.print("No lap data");
    }
}

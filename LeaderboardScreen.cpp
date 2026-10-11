#include "ui/LeaderboardScreen.h"
#include "ui/UI.h"

#include "data/F1Data.h"
#include "config/displayconfig.h"

void drawLeaderboard(){

    tft.fillScreen(ui_black);

    // Header
    tft.fillRect(0,0,SCREEN_WIDTH + 60 ,HEADER_H, ui_purple);

    tft.setTextColor(ui_black);
    tft.setTextSize(2);
    tft.setCursor(10,10);
    tft.println("Leaderboard " + currentRace.circuit);

    int y = 90;
    // Drivers
    for(int i = 0; i < driverCount; i++){
        uint16_t positionColor;

        if(drivers[i].position == "1"){
          positionColor = ui_yellow;
        }else  if(drivers[i].position == "2"){
          positionColor = ui_silver;
        }else if(drivers[i].position == "3"){
          positionColor = ui_bronze;
        }else{
          positionColor = ui_white;
        }

        tft.setTextColor(positionColor);
        tft.setCursor(10, y);
        tft.print(drivers[i].position);

        tft.setCursor(45, y);
        tft.print(drivers[i].name);

        tft.setTextSize(1);
        tft.setCursor(175, y + 4);
        tft.print("#");
        tft.print(drivers[i].number);

        Serial.print("Display: ");
        Serial.print(drivers[i].position);
        Serial.print(" ");
        Serial.print(drivers[i].name);
        Serial.print(" #");
        Serial.println(drivers[i].number);

        y += 28;
    }
}

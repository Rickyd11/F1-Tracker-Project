#include "ui/TrackScreen.h"

#include "ui/UI.h"
#include "data/F1Data.h"
#include "config/displayconfig.h"

#include "tracks/tracks.h"


void drawCurrentTrack(){

    tft.fillScreen(ui_black);
    tft.fillRect(0,0, SCREEN_WIDTH + 100, HEADER_H, ui_navy);
    tft.setTextColor(ui_black);
    tft.setTextSize(2);
    tft.setCursor(10,10);
    tft.println("Track");

    tft.setTextColor(ui_navy);
    tft.setCursor(10,48);
    tft.print(currentRace.circuit);

    for(int i = 0; i < trackBitmapCount; i++){

        if(currentRace.circuit.equalsIgnoreCase(trackBitmaps[i].circuitName)){

            // Confirms track 
            Serial.print("Track found: ");
            Serial.println(trackBitmaps[i].circuitName);

            //Centers the bitmap
            int x = (tft.width() - trackBitmaps[i].width) / 2;

            // Positon the bitmpa underneath
            int y = 70;

            //Draws the bitmap
            drawTrackBitmap(trackBitmaps[i].bitmap,x,y,trackBitmaps[i].width,trackBitmaps[i].height);

            tft.setTextColor(ui_purple);
            tft.setTextSize(2);

            int textWidth = currentRace.circuit.length() * 12;

            int textX = (tft.width() - textWidth) / 2;

            tft.setCursor(textX, 220);
            Serial.println(currentRace.circuit);

            return;
        }
    }

    Serial.print("Track not found: ");
    Serial.println(currentRace.circuit);
  
    tft.setTextColor(ui_white);
    tft.setTextSize(2);
    tft.setCursor(35, 110);
    tft.println("TRACK NOT FOUND");
    tft.setTextSize(1);
    tft.setCursor(35, 140);
    tft.print("Circuit: ");
    tft.println(currentRace.circuit);
}

void drawTrackBitmap(const uint8_t* bitmap, int x, int y, int width, int height) {

    x = (SCREEN_WIDTH - width) / 2;
    y = 65;

    tft.pushImage(x, y, width, height, const_cast<uint8_t*>(bitmap));
}

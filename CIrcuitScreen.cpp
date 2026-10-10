#include "ui/CircuitScreen.h"
#include "ui/UI.h"
#include "data/F1Data.h"
#include "config/displayconfig.h"

void drawCircuitInfo()
{
    tft.fillScreen(ui_black);

  // Green Header Box for the scren
    tft.fillRect( 0, 0, SCREEN_WIDTH + 100, HEADER_H, ui_green);
  // Shows the Cirucit Name in the Header
    tft.setTextColor(ui_black);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("Circuit: ");
    tft.println(currentRace.circuit);
  // Country and Session information to be displayed in the screen
    tft.setTextColor(ui_green);
    tft.setCursor(15, 110);
    tft.print("Country: ");
    tft.println(currentRace.country);

    tft.setCursor(15, 150);
    tft.print("Session: ");
    tft.println(currentRace.sessionName);

    tft.setCursor(15, 180);
    tft.print("Session Key: ");
    tft.println(currentRace.sessionKey);
}

#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

enum Screen{
    SCREEN_LEADERBOARD = 0,
    SCREEN_CIRCUIT = 1,
    SCREEN_FASTEST_LAP = 2,
    SCREEN_TRACK = 3
};

#define SCREEN_COUNT 4

extern TFT_eSPI tft;

extern Screen currentScreen;

void initializeDisplay();
void showBootScreen();
void drawCurrentScreen();
void nextScreen();

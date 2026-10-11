#include "data/F1Data.h"

// Driver count
Driver drivers[MAX_DRIVERS];

int driverCount = 0;

// Fastest lap 
FastestLap fastestLaps[MAX_DRIVERS];
int fastestLapCount = 0;

// Race Session
RaceSession currentRace = {
    0,
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN",
    "UNKNOWN"
};

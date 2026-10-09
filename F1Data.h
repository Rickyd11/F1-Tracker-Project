#ifndef F1DATA_H
#define F1DATA_H

#include <Arduino.h>

#define MAX_DRIVERS 5

// Driver structure 
struct Driver{
    String name;
    String number;
    String position;
};

// Race session 
struct RaceSession {
    int sessionKey;
    String sessionName;
    String raceName;
    String country;
    String circuit;
};

// Fastest Lap struct
struct FastestLap{
    String driverName;
    String driverNumber;
    String formattedTime;
    float lapTime;
};

extern Driver drivers[MAX_DRIVERS];
extern int driverCount;
extern FastestLap fastestLaps[MAX_DRIVERS];
extern int fastestLapCount;
extern RaceSession currentRace;

#endif

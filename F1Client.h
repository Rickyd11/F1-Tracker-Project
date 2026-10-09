#ifndef F1CLIENT_H
#define F1CLIENT_H

#include <Arduino.h>
#include <ArduinoJson.h>

bool getOpenF1Data(
    const String& url,
    String& payload
);

bool updateF1Data();

#endif

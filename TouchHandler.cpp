#include "input/TouchHandler.h"

#include "config/appconfig.h"
#include "config/displayconfig.h"
#include "ui/UI.h"

#include <SPI.h>
#include <XPT2046_Touchscreen.h>

// Create a separate SPI bus for the touchscreen.
SPIClass touchSPI(HSPI);
// Create the touchscreen object.
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

//Global Variable
unsigned long lastTouch = 0;


void initializeTouch(){

    touchSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);

    touch.begin(touchSPI);

    touch.setRotation(3);
    Serial.println("Touch Initialized");


}

void handleTouch(){

    if(!touch.touched()){
        return;
    }
    unsigned long now =  millis();

    if(now - lastTouch < TOUCH_DEBOUNCE){
        return;
    }

    lastTouch = now;

    TS_Point point = touch.getPoint();

    Serial.print("Touch: ");
    Serial.print(point.x);
    Serial.print(", ");
    Serial.println(point.y);


    nextScreen();
}

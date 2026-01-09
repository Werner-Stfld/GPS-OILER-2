#include <arduino.h>

#include "ActionScreen.h"

void ActionScreen::setup() {
    spr.fillScreen(TFT_WHITE);         
    spr.setTextWrap(false);            
    spr.setTextColor(TFT_BLACK, TFT_WHITE); 
    // spr.setTextFont(4);
    printTitle();
    spr.drawRect(1, 124, 122, 10, TFT_BLACK);        // Border of the bar chart
}

void ActionScreen::loop(ScreenArgs &args) {
    if (!displayTimeout.timedOut()) 
        return;
    if (!updateRequired)
        return;

    updateRequired = false;
    byte v = map(args.timeToActionInPercent, 0, 100, 0, 122); // map percent to rect length
    spr.fillRect(1, 53, v, 10, TFT_BLACK);      // Draws the bar depending on the time, the button is pressed value
    spr.pushSprite(0,0);
}
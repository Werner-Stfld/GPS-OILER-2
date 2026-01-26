#include <arduino.h>

#include "ActionScreen.h"

void ActionScreen::setup() {
    Serial.println("ActionScreen::setup()");
    spr.fillRect(0,0,TFT_HEIGHT, TFT_WIDTH, TFT_WHITE);
    spr.setTextWrap(false);            
    spr.setTextColor(TFT_BLACK); 
    spr.setTextSize (1);
    spr.setTextFont(4);
    printTitle();
    spr.drawRect(1, 117, 158, 10, TFT_BLACK);        // Border of the bar chart
    updateRequired = true;
}

void ActionScreen::loop(ScreenArgs &args) {
    if (!displayTimeout.retriggered()) 
        return;
    byte v = map(args.timeToActionInPercent, 0, 100, 0, 158); // map percent to rect length
    uint32_t color = TFT_BLACK;
    if (args.timeToActionInPercent >= 100) {
        color = TFT_RED;
    }
    spr.fillRect(1, 117, v, 10, color);      // Draws the bar depending on the time, the button is pressed value
    spr.pushSprite(0,0);
}
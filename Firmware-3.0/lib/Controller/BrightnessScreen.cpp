#include <arduino.h>

#include "BrightnessScreen.h"

void BrightnessScreen::setup() {
    Serial.println("BrightnessScreen::setup()");
    spr.fillRect(0,0,TFT_HEIGHT, TFT_WIDTH, TFT_WHITE);
    spr.setTextWrap(false);            
    spr.setTextColor(TFT_BLACK); 
    spr.setTextSize (1);
    spr.setTextFont(4);
    spr.setCursor(1, 24);
    spr.println("Helligkeit");
    updateRequired = true;
    displayTimeout.nextTimeout(300); // Timeout as long as timeout for next screen
}

NextScreenAction BrightnessScreen::ScreenAction(ButtonState buttonState) {
    if (buttonState == ButtonState::ShortFallingEdge) {
        _brightness.flush();
        return NextScreenAction::nextScreen;
    }
    return NextScreenAction::keepScreen;
}

void BrightnessScreen::loop(ScreenArgs &args) {
    if (!displayTimeout.timedOut())
        return;
    displayTimeout.nextTimeout(50); // After first Timeout 50 ms
    
    if (args.timeToActionInPercent) {
        int b = _brightness.get();
        b += sign*2;
        if (b < 0) {
            b = 0;
            sign = 1;
        }
        if (b > 100) {
            b = 100;
            sign = -1;
        }
        _brightness.set(b, SetMode::flush);
        execute(b);
    }
    byte v = map( _brightness.get(), 0, 100, 0, 158); // map percent to rect length
    spr.fillRect(1, 110, 158, 20, TFT_WHITE);        // Border of the bar chart
    spr.fillRect(1, 110, v, 20, TFT_RED);      // Draws the bar depending on the time, the button is pressed value
    spr.drawRect(1, 110, 158, 20, TFT_BLACK);        // Border of the bar chart
    spr.pushSprite(0,0);
}
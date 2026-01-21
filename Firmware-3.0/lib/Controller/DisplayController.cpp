#include <Complex.h>

#include "globals.h"
#include "webController.h"
#include "DisplayController.h"
#include "icons.h"


void DisplayController::setup() {
    restore();

    tft.init(INITR_GREENTAB2);
    // tft.writecommand(0x01); // Software reset 
    delay(150);
    tft.setRotation(1);
    // display.setFont(4); 
   tft.fillScreen(TFT_WHITE);
   spr.createSprite(TFT_HEIGHT, TFT_WIDTH);  // Vollbild-Sprite
   currentScreen()->setup();
}

DisplayController::DisplayController() {  // Pins des displays sind in platformio.ini definiert.
    // Create ist of variable containers for variable handling
    add(&currScreen);
    add(&brightness);
    add(&oilsymbol_Zeit);
    add(&timeZone);
};

void DisplayController::loop(bool pressed)
{
    buttonHandler.loop(pressed);
    ButtonState buttonState = buttonHandler.GetButtonState();
    NextScreenAction screenAction = currentScreen()->ScreenAction(buttonState);
    if (screenAction == NextScreenAction::nextScreen) {
        int no = currScreen.get();
        no++;
        if (no >= numScreens) no = 0;
        currScreen.set(no, SetMode::flush);
        currentScreen()->setup();
    }
    if (screenAction == NextScreenAction::defaultScreen) {
        currScreen.set(0, SetMode::flush);
        currentScreen()->setup();
    }
    ScreenArgs args;
    args.timeToActionInPercent = buttonHandler.TimeToActionInPercent();
    currentScreen()->loop(args);
}

#include <Complex.h>

#include "globals.h"
#include "webController.h"
#include "DisplayController.h"
#include "icons.h"

unsigned long actionTimeout = 3000;

void DisplayController::setup() {
    restore();

    Serial.println("init");
    tft.init(INITR_GREENTAB2);

    Serial.println("setRotation");
    tft.setRotation(3);
    // display.setFont(4); 
   tft.fillScreen(TFT_GREEN);
   spr.createSprite(TFT_HEIGHT, TFT_WIDTH);  // Vollbild-Sprite
   currentScreen = &defaultScreen;
   currentScreen->setup();
}

DisplayController::DisplayController(): currentScreen(&defaultScreen) {  // Pins des displays sind in platformio.ini definiert.
    // Create ist of variable containers for variable handling
    add(&Start_disp_1);
    add(&Start_disp_2);
    add(&defaultScreen.oilsymbol_Zeit);
    add(&defaultScreen.timeZone);

    // setup screen chain
    defaultScreen.next = &resetTankScreen;
    resetTankScreen.next = &wifiQrScreen;
    wifiQrScreen.next = &webQrScreen;
    webQrScreen.next = &defaultScreen;
};

Timer pressedTmo = Timer(200);

ButtonState GetButtonState (unsigned long fallingEdge) {
    if (fallingEdge < 30) // too short
        return ButtonState::none;
    if (fallingEdge > actionTimeout && fallingEdge < 10000) // execute provided action
        return ButtonState::LongFallingEdge;
    if (fallingEdge < 300)
        return ButtonState::ShortFallingEdge;
    return ButtonState::none;
}

void DisplayController::loop(bool pressed)
{
    buttonHandler.loop(pressed);
    ScreenArgs args;
    args.timeToActionInPercent = buttonHandler.PressedTime()*100 / actionTimeout;
    ButtonState buttonState = GetButtonState(buttonHandler.FallingEdge());
    if (buttonState == ButtonState::ShortFallingEdge) {
        if (currentScreen->next != nullptr) {
            currentScreen = currentScreen->next;
            currentScreen->setup();
        }
    }
    if (buttonState == ButtonState::LongFallingEdge) {
        currentScreen->execute();
        currentScreen = &defaultScreen; // After executing a command, return to the default screen.
    }
    currentScreen->loop(args);
}

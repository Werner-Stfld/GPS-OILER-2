#include <Complex.h>

#include "globals.h"
#include "webController.h"
#include "DisplayController.h"
#include "icons.h"

unsigned long actionTimeout = 3000;

void DisplayController::setup() {
    restore();

    Serial.println("init");
    display.initR(INITR_BLACKTAB);
    // display.setSPISpeed(40000000);
    display.setCursor(0,0);

    Serial.println("setRotation");
    display.setRotation(3);
    Serial.println("setTextColor");
    display.setTextColor(ST77XX_BLACK, ST77XX_WHITE); 
    Serial.println("setTextFont");
    display.fillScreen(ST77XX_BLUE);
    display.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    display.setTextSize(1);
    display.println("Hello World");
    // display.setFont(4); 
    // currentScreen->setup();
}

DisplayController::DisplayController(): currentScreen(&defaultScreen) {  // Pins des displays sind in platformio.ini definiert.

    add(&Start_disp_1);
    add(&Start_disp_2);
    add(&defaultScreen.oilsymbol_Zeit);
    add(&defaultScreen.timeZone);
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

unsigned int i = 0;
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

#pragma once

#include <arduino.h>
#include <TFT_eSPI.h>

#include "Timer.h"
#include "userVar.h"
#include "gpsController.h"
#include "ScreenBase.h"
#include "QrScreen.h"
#include "ActionScreen.h"
#include "DefaultScreen.h"
#include "BrightnessScreen.h"
#include "InfoScreen.h"

// detects falling edges of a button. Returns the time, the button has been pressed
// while pressed. 
class ButtonHandler {
    unsigned long risingEdge = 0;
    unsigned long fallingEdge = 0;
    unsigned long pressedTime = 0;
    bool lastPressed = false;
    unsigned long longFallingEdgeTimeout = 2000;
    unsigned long shortFallingEdgeTimeout = 300;

public:
    ButtonHandler() {
    }

    ButtonState GetButtonState () {
        if (fallingEdge < 30) // too short
            return ButtonState::none;
        if (fallingEdge > longFallingEdgeTimeout && fallingEdge < 10000) // execute provided action
            return ButtonState::LongFallingEdge;
        if (fallingEdge < shortFallingEdgeTimeout)
            return ButtonState::ShortFallingEdge;
        return ButtonState::none;
    }

    unsigned long FallingEdge() {
        return fallingEdge;
    }
    unsigned long PressedTime() {
        return pressedTime;
    }

    unsigned long PressedTimeAfterFallingEdge() {
        return (pressedTime <= shortFallingEdgeTimeout)? 0: pressedTime - shortFallingEdgeTimeout;
    }

    unsigned long TimeToActionInPercent() {
        return (pressedTime <= shortFallingEdgeTimeout)? 0: (pressedTime - shortFallingEdgeTimeout)*100/longFallingEdgeTimeout;
    }

    void loop(bool pressed) {
        unsigned long m  = millis();
        fallingEdge = 0;
        pressedTime = 0;
        if (lastPressed != pressed) {
            if (pressed) {
                risingEdge = m;            
            } else {
                if (risingEdge != 0) { // omit fallingEdge if no raisingEdge has been detected so far
                    fallingEdge = m - risingEdge;
                }
            }
            lastPressed = pressed;
        }
        if (pressed) {
            if (risingEdge != 0) { // omit pressed time  if no raisingEdge has been detected so far
                pressedTime = m - risingEdge;
            }
        }
    }
};

class DisplayController;

constexpr int numScreens = 8;
class DisplayController : public VarContainer 
{
    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);

    ButtonHandler buttonHandler = ButtonHandler();
    bool updateRequired = true;
    int rainAverage = 0;

    float batteryVoltage = 12.0;

    WiFiQrScreen wifiQrScreen = WiFiQrScreen(spr);
    WebQrScreen webQrScreen = WebQrScreen(spr);
    DefaultScreen defaultScreen = DefaultScreen(spr, timeZone, oilsymbol_Zeit);
    ResetTankScreen resetTankScreen = ResetTankScreen(spr);
    ResetWiFiScreen resetWiFiScreen = ResetWiFiScreen(spr);
    ResetSettingsScreen resetSettingsScreen = ResetSettingsScreen(spr);
    BrightnessScreen brightnessScreen = BrightnessScreen(spr, brightness);
    InfoScreen infoScreen = InfoScreen(spr);

    ScreenBase *screens[numScreens] = {
        &defaultScreen, 
        &brightnessScreen, 
        &resetTankScreen,
        &infoScreen,
        &wifiQrScreen,
        &webQrScreen,
        &resetWiFiScreen,
        &resetSettingsScreen};

    Timer displayTimeout = Timer(1000); 

public:
    void setup();

    void loop(bool pressed);
    IntVar currScreen = IntVar(String("Startbildschirm: "), 0, PrefKeys::currentScreen);
    IntVar brightness = IntVar(String("Helligkeit: "), 100, PrefKeys::brightness);
    IntVar oilsymbol_Zeit = IntVar(String("Zeit Ölsymbol:"), 3, PrefKeys::oilsymbol_Zeit);
    IntVar timeZone = IntVar(String("Zeitzone:"), 1, PrefKeys::timezone);                 

    ScreenBase *currentScreen() {
        int no = currScreen.get();
        if (no < 0) no = 0;
        if (no >= numScreens) no = numScreens - 1;
        return screens[no];
    }

    DisplayController();

    void setBatteryVoltage(float value) {
        if (value != batteryVoltage) {
            batteryVoltage = value;
            updateRequired = true;
        }
    }

    void triggerShowOiling()
    {
        defaultScreen.triggerShowOiling();
    }

    void OnTankReset(void tankReset()) {
        resetTankScreen.execute = tankReset;
    }

    void OnWiFiReset(void wiFiReset()) {
        resetWiFiScreen.execute = wiFiReset;
    }

    void OnSettingsReset(void settingsReset()) {
        resetSettingsScreen.execute = settingsReset;
    }

    void (*onWiFiOnOff)(bool) = [](bool f)  {

    };

    void OnWiFiOnOff(void wifiOnOff(bool)) {
        onWiFiOnOff = wifiOnOff;
    }
};

extern DisplayController displayController;

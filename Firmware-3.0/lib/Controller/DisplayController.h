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

// detects falling edges of a button. Returns the time, the button has been pressed
// while pressed. 
class ButtonHandler {
    unsigned long risingEdge = 0;
    unsigned long fallingEdge = 0;
    unsigned long pressedTime = 0;
    bool lastPressed = false;
public:
    ButtonHandler() {
    }

    unsigned long FallingEdge() {
        return fallingEdge;
    }
    unsigned long PressedTime() {
        return pressedTime;
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

enum ButtonState {
    none,
    LongFallingEdge,
    ShortFallingEdge,  
};

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

    ScreenBase *currentScreen;
    // Anzeigen des Startbildschirm auf dem OLED-Display
    void displayDefaultScreen();
    void displayTankResetScreen();
    void displayWifiQrScreen();
    void displayWebQrScreen();
    void displayVersionScreen();
    void displayWifiResetScreen();
    void displaySettingsResetScreen();

    enum displayState
    {
        stateSetup,
        stateScreen1,
        stateScreen2,
        stateScreen3,
    } state = displayState::stateSetup;

    Timer displayTimeout = Timer(1000); 

public:
    void setup();

    void loop(bool pressed);
    IntVar Start_disp_1 = IntVar(String("Startbildschirm 1:"), 2, PrefKeys::Start_disp_1);
    IntVar Start_disp_2 = IntVar(String("Startbildschirm 2:"), 5, PrefKeys::Start_disp_2);
    IntVar oilsymbol_Zeit = IntVar(String("Zeit Ölsymbol:"), 6, PrefKeys::oilsymbol_Zeit);
    IntVar timeZone = IntVar(String("Zeitzone:"), 1, PrefKeys::timezone);                 

    DisplayController();

    void setDirection(int value)
    {
        if (value != defaultScreen.direction)
        {
            defaultScreen.direction = value;
            defaultScreen.updateRequired = true;
        }
    }

    void setSpeed(float value)
    {
        if (value != defaultScreen.speed)
        {
            defaultScreen.speed = value;
            defaultScreen.updateRequired = true;
        }
    }
    void setDistance(int value)
    {
        if (value != defaultScreen.distance)
        {
            defaultScreen.distance = value;
            updateRequired = true;
        }
    }
    void setRainAverage(int value)
    {
        if (value != rainAverage)
        {
            rainAverage = value;
            updateRequired = true;
        }
    }

    void setTankPercent(int value)
    {
        if (value != defaultScreen.tankPercent)
        {
            defaultScreen.tankPercent = value;
            defaultScreen.updateRequired = true;
        }
    }
    void setShowOiling(bool value)
    {
        if (value != defaultScreen.showOiling)
        {
            defaultScreen.showOiling = value;
            updateRequired = true;
        }
    }
    void setShowSattelite(bool value)
    {
        if (value != defaultScreen.showSattelite)
        {
            defaultScreen.showSattelite = value;
            defaultScreen.updateRequired = true;
        }
    }
    void setNoSattelite(int value)
    {
        if (value != defaultScreen.noSattelite)
        {
            defaultScreen.noSattelite = value;
            defaultScreen.updateRequired = true;
        }
    }
    int getNoSattelite()
    {
        return defaultScreen.noSattelite;
    }
    void setShowRaining(bool value)
    {
        if (value != defaultScreen.showRaining)
        {
            defaultScreen.showRaining = value;
            defaultScreen.updateRequired = true;
        }
    }

    void setTime(gpsTime value)
    {
        if (value.hour != defaultScreen.time.hour || value.minute != defaultScreen.time.minute)
        {
            defaultScreen.time = value;
            defaultScreen.updateRequired = true;
        }
    }
    
    void setAlt(int value)
    {
        if (defaultScreen.alt != value)
        {
            defaultScreen.alt = value;
            defaultScreen.updateRequired = true;
        }
    }

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

    void setOilingDistanceInPercent(int value) {
        if (value != defaultScreen.oilingDistanceInPercent) {
            defaultScreen.oilingDistanceInPercent = value;
            defaultScreen.updateRequired = true;
        }
    }

    void OnTankReset(void tankReset()) {
        resetTankScreen.execute = tankReset;
    }

    void (*onWiFiReset)() = []()  {

    };
    void OnWiFiReset(void wiFiReset()) {
        onWiFiReset = wiFiReset;
    }
    void (*onSettingsReset)() = []()  {

    };
    void OnSettingsReset(void settingsReset()) {
        onSettingsReset = settingsReset;
        wifiQrScreen.execute = settingsReset;
    }
    void (*onWiFiOnOff)(bool) = [](bool f)  {

    };

    void OnWiFiOnOff(void wifiOnOff(bool)) {
        onWiFiOnOff = wifiOnOff;
    }
};

extern DisplayController displayController;

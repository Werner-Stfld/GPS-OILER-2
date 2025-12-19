#pragma once

#include <Adafruit_GFX.h>     //
#include <Adafruit_SSD1306.h> // Für das OLED
#include <QRCodeGFX.h>
#include <complex.h>

#include "Timer.h"
#include "userVar.h"
#include "gpsController.h"

// detects falling edges of a button. Returns the time, the button has been pressed
// while button is pressed, 
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

void doNothing();

class ScreenBase {
public:
    virtual void loop() = 0;
    bool updateRequired;
    void (*execute)();
    ScreenBase *next;

    ScreenBase() {
        updateRequired = true;
        execute = doNothing;
        next = this;
    }
};

class MyScreen1 : public ScreenBase {
    Adafruit_SSD1306 &display;
    public:
    void loop();
    MyScreen1(Adafruit_SSD1306 &_display): display(_display)  {
    }
};

class MyScreen2 : public ScreenBase {
    QRCodeGFX qrcode;
    Adafruit_SSD1306 &display;
    public:
    void loop();
    MyScreen2(Adafruit_SSD1306 &_display):qrcode(QRCodeGFX(_display)),display(_display)  {
    }
};

class MyScreen3 : public ScreenBase {
    Adafruit_SSD1306 &display;

    void displaySpeed();
    void displayDirection();
    void displayTime();
    void displayTank();
    void displayNoSattelite();
    void displayOiling();
    void displayCompass();        // compass needle filled triangle toward north
    void displayDirectionOnMap(); // direction as arrow on a map

    public:
    bool showRaining = false;
    bool showSpeed = false;
    bool showSattelite = 0;
    float speed = 0;
    int direction = 0;
    gpsTime time = {0,0,0};
    IntVar &timeZone;
    int tankPercent = 0;
    bool showOiling = false;
    int oilingDistanceInPercent = 0;
    int noSattelite = 0;

    void loop();
    MyScreen3(Adafruit_SSD1306 &_display, IntVar &_timeZone):display(_display), timeZone(_timeZone)  {
    }
};

const boolean invert_Display = true; // Display Invertieren oder nicht
class DisplayController : public VarContainer 
{
    Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64); // Definition für das OLED
    ButtonHandler buttonHandler = ButtonHandler();
    bool updateRequired = true;
    int distance = 0;
    int rainAverage = 0;

    Timer timeoutShowOiling = Timer(0);
    float batteryVoltage = 12.0;

    MyScreen1 scr1 = MyScreen1(display);
    MyScreen2 scr2 = MyScreen2(display);
    MyScreen3 scr3 = MyScreen3(display, timeZone);

    ScreenBase &currentScreen;
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

    void displayDistance();

public:
    void setup();

    void loop(bool pressed);
    IntVar Start_disp_1 = IntVar(String("Startbildschirm 1:"), 2, PrefKeys::Start_disp_1);               // Zeit für Startdisplay 1 in Sec.
    IntVar Start_disp_2 = IntVar(String("Startbildschirm 2:"), 5, PrefKeys::Start_disp_2);               // Zeit für Startdisplay 2 in Sec.
    IntVar oilsymbol_Zeit = IntVar(String("Zeit Ölsymbol:"), 6, PrefKeys::oilsymbol_Zeit);                 // Zeit in Sec. wie lange das OilSymbol erscheint
    IntVar timeZone = IntVar(String("Zeitzone:"), 1, PrefKeys::timezone);                 // Zeit in Sec. wie lange das OilSymbol erscheint

    DisplayController();

    void setDirection(int value)
    {
        if (value != scr3.direction)
        {
            scr3.direction = value;
            scr3.updateRequired = true;
        }
    }

    void setSpeed(float value)
    {
        if (value != scr3.speed)
        {
            scr3.speed = value;
            scr3.updateRequired = true;
        }
    }
    void setDistance(int value)
    {
        if (value != distance)
        {
            distance = value;
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
        if (value != scr3.tankPercent)
        {
            scr3.tankPercent = value;
            scr3.updateRequired = true;
        }
    }
    void setShowOiling(bool value)
    {
        if (value != scr3.showOiling)
        {
            scr3.showOiling = value;
            updateRequired = true;
        }
    }
    void setShowSattelite(bool value)
    {
        if (value != scr3.showSattelite)
        {
            scr3.showSattelite = value;
            scr3.updateRequired = true;
        }
    }
    void setNoSattelite(int value)
    {
        if (value != scr3.noSattelite)
        {
            scr3.noSattelite = value;
            scr3.updateRequired = true;
        }
    }
    int getNoSattelite()
    {
        return scr3.noSattelite;
    }
    void setShowRaining(bool value)
    {
        if (value != scr3.showRaining)
        {
            scr3.showRaining = value;
            scr3.updateRequired = true;
        }
    }

    void setTime(gpsTime value)
    {
        if (value.hour != scr3.time.hour || value.minute != scr3.time.minute)
        {
            scr3.time = value;
            scr3.updateRequired = true;
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
        timeoutShowOiling.nextTimeout(oilsymbol_Zeit.get()*1000);
        if (!scr3.showOiling) 
            scr3.updateRequired = true;
        scr3.showOiling = true;
    }

    void setOilingDistanceInPercent(int value) {
        if (value != scr3.oilingDistanceInPercent) {
            scr3.oilingDistanceInPercent = value;
            scr3.updateRequired = true;
        }
    }

    void (*onTankReset)() = []()  {

    };
    void OnTankReset(void tankReset()) {
        onTankReset = tankReset;
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
        scr1.execute = settingsReset;
    }
    void (*onWiFiOnOff)(bool) = [](bool f)  {

    };

    void OnWiFiOnOff(void wifiOnOff(bool)) {
        onWiFiOnOff = wifiOnOff;
    }
};

extern DisplayController displayController;

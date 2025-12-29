#pragma once

#include <QRCode.h>
#include <complex.h>
#include <fs.h>

#include "Timer.h"
#include "userVar.h"
#include "gpsController.h"

const boolean invert_Display = true; // Display Invertieren oder nicht

void doNothing();

class ScreenBase;
struct ScreenArgs {
    unsigned long timeToActionInPercent;
};

class ScreenBase {
public:
    virtual void loop(ScreenArgs &state) = 0;
    virtual void setup() = 0;
    bool updateRequired;
    void (*execute)();
    ScreenBase *next;
    Timer displayTimeout = Timer(300);

    ScreenBase() {
        updateRequired = true;
        execute = doNothing;
        next = this;
    }
};


class WiFiQrScreen : public ScreenBase {
    Adafruit_ST7735  &display;
    public:
    void loop(ScreenArgs &state);
    void setup ();
    WiFiQrScreen(Adafruit_ST7735  &_display): display(_display)  {
    }
};

class WebQrScreen : public ScreenBase {
    Adafruit_ST7735  &display;
    public:
    void loop(ScreenArgs &state);
    void setup ();
    WebQrScreen(Adafruit_ST7735  &_display):display(_display)  {
    }
};

class ResetTankScreen : public ScreenBase {
    Adafruit_ST7735  &display;
    public:
    void loop(ScreenArgs &args);
    void setup ();
    ResetTankScreen(Adafruit_ST7735  &_display):display(_display)  {}
};

class ResetWiFiScreen : public ScreenBase {
    Adafruit_ST7735  &display;
    public:
    void loop(ScreenArgs &state);
    void setup ();
    ResetWiFiScreen(Adafruit_ST7735  &_display):display(_display)  {}
};

class ResetSettingsScreen : public ScreenBase {
    Adafruit_ST7735  &display;
    public:
    void loop(ScreenArgs &state);
    void setup ();
    ResetSettingsScreen(Adafruit_ST7735  &_display):display(_display)  {}
};

class DefaultScreen : public ScreenBase {
    Adafruit_ST7735  &display;
    Timer timeoutShowOiling = Timer(0);

    void displaySpeed();
    void displayDirection();
    void displayTime();
    void displayTank();
    void displayNoSattelite();
    void displayOiling();
    void displayCompass();        // compass needle filled triangle toward north
    void displayDirectionOnMap(); // direction as arrow on a map
    void displayDistance();

    public:
    bool showRaining = false;
    bool showSpeed = false;
    bool showSattelite = 0;
    int distance = 0;
    float speed = 0;
    int direction = 0;
    gpsTime time = {0,0,0};
    IntVar &timeZone;
    IntVar &oilsymbol_Zeit;
    int tankPercent = 0;
    bool showOiling = false;
    int oilingDistanceInPercent = 0;
    int noSattelite = 0;
    void loop(ScreenArgs &state);
    void setup ();
    void triggerShowOiling();
    DefaultScreen(Adafruit_ST7735  &_display, IntVar &_timeZone, IntVar & _oilsymbol_Zeit):
        display(_display), 
        timeZone(_timeZone), 
        oilsymbol_Zeit(_oilsymbol_Zeit)  {}
};

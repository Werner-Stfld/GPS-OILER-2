#pragma once

#include <QRCode.h>
#include <complex.h>
#include <fs.h>

#include "Timer.h"
#include "userVar.h"
#include "gpsController.h"
#include <TFT_eSPI.h>

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
    TFT_eSprite  &display;
    public:
    void loop(ScreenArgs &state);
    void setup ();
    WiFiQrScreen(TFT_eSprite  &_display): display(_display)  {
    }
};

class WebQrScreen : public ScreenBase {
    TFT_eSprite  &display;
    public:
    void loop(ScreenArgs &state);
    void setup ();
    WebQrScreen(TFT_eSprite  &_display):display(_display)  {
    }
};

class ResetTankScreen : public ScreenBase {
    TFT_eSprite  &display;
    public:
    void loop(ScreenArgs &args);
    void setup ();
    ResetTankScreen(TFT_eSprite  &_display):display(_display)  {}
};

class ResetWiFiScreen : public ScreenBase {
    TFT_eSprite  &display;
    public:
    void loop(ScreenArgs &state);
    void setup ();
    ResetWiFiScreen(TFT_eSprite  &_display):display(_display)  {}
};

class ResetSettingsScreen : public ScreenBase {
    TFT_eSprite  &display;
    public:
    void loop(ScreenArgs &state);
    void setup ();
    ResetSettingsScreen(TFT_eSprite  &_display):display(_display)  {}
};

class DefaultScreen : public ScreenBase {
    TFT_eSprite  &display;
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
    DefaultScreen(TFT_eSprite  &_display, IntVar &_timeZone, IntVar & _oilsymbol_Zeit):
        display(_display), 
        timeZone(_timeZone), 
        oilsymbol_Zeit(_oilsymbol_Zeit)  {}
};

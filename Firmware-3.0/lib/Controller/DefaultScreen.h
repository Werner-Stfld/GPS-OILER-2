#pragma once

#include "ScreenBase.h"

class DefaultScreen : public ScreenBase {
    Timer timeoutShowOiling = Timer(0);

    void displaySpeed();
    void displayDirection();
    void displayTime();
    void displayAlt();
    void displayTank();
    void displayNoSattelite();
    void displayOiling();
    void displayCompass();        // compass needle filled triangle toward north
    void displayDirectionOnMap(); // direction as arrow on a map
    void drawScale(int cx, int cy, int r);
    public:
    bool showRaining = false;
    bool showSattelite = 0;
    int distance = 0;
    float speed = 0;
    int direction = 0;
    int alt = 0;
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
    Timer speedToggleTimeout = Timer(500);

    DefaultScreen(TFT_eSprite  &_spr, IntVar &_timeZone, IntVar & _oilsymbol_Zeit):  
        ScreenBase(_spr),
        timeZone(_timeZone), 
        oilsymbol_Zeit(_oilsymbol_Zeit)  {}
};

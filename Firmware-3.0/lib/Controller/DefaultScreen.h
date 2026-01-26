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
    void updateData();

    bool showRaining = false;
    bool showSattelite = 0;
    bool warningTankDisplay = false;
    float speed = 0;
    int direction = 0;
    int alt = 0;
    gpsTime time = {0,0,0};
    int tankPercent = 0;
    bool showOiling = false;
    int oilingDistanceInPercent = 0;
    int noSattelite = 0;
    void loop(ScreenArgs &state);
    void setup ();
    Timer tankToggleTimeout = Timer(500);

    u_int16_t fgColor = TFT_BLACK;
    u_int16_t bgColor = TFT_WHITE;
    u_int16_t bgCompass = TFT_DARKGREY;
    u_int16_t speedFgColor = TFT_RED;
    u_int16_t tankWarningColor = TFT_BLUE;
    u_int16_t tankWarningFgColor = TFT_WHITE;
    u_int16_t tankBarColor = TFT_RED;

    public:
    IntVar &timeZone;
    void triggerShowOiling();
    DefaultScreen(TFT_eSprite  &_spr, IntVar &_timeZone, bool darkMode = false):  
        ScreenBase(_spr),
        timeZone(_timeZone)  {
            if (darkMode) {
                fgColor = TFT_DARKGREY;
                bgColor = TFT_BLACK;
                bgCompass = TFT_BLACK;
                speedFgColor = TFT_RED;
                tankWarningColor = TFT_BLUE;
                tankWarningFgColor = TFT_WHITE;
                tankBarColor = TFT_RED;
            }
        }
};

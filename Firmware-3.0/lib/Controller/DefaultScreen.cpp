#include <Complex.h>
#include <TFT_eSPI.h>
#include <math.h>

#include "globals.h"
#include "webController.h"
#include "DisplayController.h"
#include "DistanceController.h"
#include "RainController.h"
#include "TankController.h"

#include "icons.h"

void initSinTab();

void DefaultScreen::setup() {
    Serial.println("DefaultScreen::setup()");

    spr.setTextWrap(false);            // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
    spr.setTextColor(TFT_BLACK);
    spr.setTextSize (1);
    spr.setTextFont(6);
    initSinTab();
    updateRequired = true;
}

void DefaultScreen::updateData() {
    {
        int value = distanceController.oilingDistanceInPercent();
        if (value != oilingDistanceInPercent) {
            oilingDistanceInPercent = value;
            updateRequired = true;
        }
    }
    {
        float value = gpsController.speed();
        if (value != speed)
        {
            speed = value;
            updateRequired = true;
        }
    }
    {
        int value = tankController.fillGradeInPercent();
        if (value != tankPercent)
        {
            tankPercent = value;
            updateRequired = true;
        }
    }
    {
        bool value = !gpsController.emergency();
        if (value != showSattelite)
        {
            showSattelite = value;
            updateRequired = true;
        }
    }
    {
        int value = gpsController.sattelites();
        if (value != noSattelite)
        {
            noSattelite = value;
            updateRequired = true;
        }
    }

    {
        bool value = rainController.isRaining();
        if (value != showRaining)
        {
            showRaining = value;
            updateRequired = true;
        }
    }
    {
        int value = gpsController.course();
        if (value != direction)
        {
            direction = value;
            updateRequired = true;
        }
    }
    {
        gpsTime value = gpsController.time();
        if (value.hour != time.hour || value.minute != time.minute)
        {
            time = value;
            updateRequired = true;
        }
    }
    {
        int value = gpsController.altitude();
        if (alt != value)
        {
            alt = value;
            updateRequired = true;
        }
    }
}

void DefaultScreen::triggerShowOiling() {
    timeoutShowOiling.nextTimeout(oilsymbol_Zeit.get()*1000);
    if (!showOiling) 
        updateRequired = true;
    showOiling = true;
}

void DefaultScreen::loop(ScreenArgs &args) {

    if (timeoutShowOiling.timedOut())
    {
        timeoutShowOiling.nextTimeout(0); // stop timer
        if (showOiling)
            updateRequired = true;
        showOiling=false; // remove oilcan
    }
    if (tankPercent < 15) {
        if (tankToggleTimeout.timedOut()) {
            warningTankDisplay = !warningTankDisplay;
            updateRequired = true;
        }
    } else {
        if (warningTankDisplay)
            updateRequired = true;
        warningTankDisplay = false;
    }

    if (!displayTimeout.timedOut()) 
        return;

    updateData();

    if (!updateRequired)
        return;
    updateRequired = false;

    spr.fillRect(0,0,TFT_HEIGHT, TFT_WIDTH, TFT_WHITE);
    displaySpeed();

    displayDirection();
    displayTime();
    displayAlt();
    displayTank();
    displayNoSattelite();
    displayOiling();
    if (showRaining) spr.drawBitmap(30, 6, iconRaining(), 16, 16, TFT_BLACK);
    spr.pushSprite(0,0);
}

void DefaultScreen::displaySpeed()
{
    if (!showSattelite)
        return;
    char tmp[10];
    dtostrf(speed, 3, 0, tmp);
    spr.setTextSize (1);
    spr.setTextFont(6);
    spr.setTextColor(TFT_RED);
    int16_t w = spr.textWidth(tmp);
    spr.drawString(tmp, 158-w, 2);
    spr.drawRoundRect(73, 1, 160-74, 40, 4, TFT_BLACK);  // Rahmen für die Geschwindigkeit
    spr.setTextColor(TFT_BLACK);
}

void DefaultScreen::displayTank()
{
    if (warningTankDisplay) {
        spr.fillRect(1, 100, 72, 26, TFT_BLUE);    // Draws full bar in RED
        char tmp[10];
        dtostrf(tankPercent, 2, 0, tmp);
        int l = strlen(tmp);
        tmp[l] = '%';
        tmp[l+1] = 0;
        spr.setTextFont(4);
        spr.setTextColor(TFT_WHITE);
        uint16_t cwidth = spr.textWidth(tmp);
        spr.drawString(tmp, 36-cwidth/2, 102);
    } 
    byte v = map(tankPercent, 0, 100, 0, 72); // map percent to rect length
    spr.fillRect(1, 100, v, 26, TFT_RED);      // Draws the bar depending on the sensor value
    spr.drawRect(1, 100, 72, 26, TFT_BLACK);        // Border of the bar chart
}

void DefaultScreen::displayTime()
{
    if (!showSattelite)
        return;
    int hour = time.hour + timeZone.get();
    while (hour >23) hour-=24;
    while (hour < 0) hour+=24;
    char tmp[10];
    if (hour < 10) {
        tmp[0] = ' ';
        itoa(hour, tmp + 1, 10);
    } else {
        itoa(hour, tmp, 10);
    }
    tmp[2] = ':';
    if (time.minute < 10) {
        tmp[3] = '0';
        itoa(time.minute, tmp + 4, 10);
    } else {
        itoa(time.minute, tmp + 3, 10);
    }
    spr.setCursor(6, 46);
    spr.setTextSize (1);
    spr.setTextFont(4);
    spr.println(tmp);
    spr.drawRoundRect(1, 45, 72, 23, 4, TFT_BLACK);  // Rahmen für die Zeit
}

void DefaultScreen::displayAlt()
{
    if (!showSattelite)
        return;
    char tmp[10];
    itoa(alt, tmp, 10);
    spr.setTextSize (1);
    spr.setTextFont(4);
    int16_t w = spr.textWidth(tmp);
    spr.drawString(tmp, 70 - w,73);
    spr.drawRoundRect(1, 72, 72, 24, 4, TFT_BLACK);
}

void DefaultScreen::displayNoSattelite()
{
    for (int i = 0; i < 6;i++) {
        int h=4 + i*7;
        if (i < (noSattelite - 2)) {
            spr.fillRect (42 + i*5, 41 - h, 4, h, TFT_BLACK);
        } else {
            spr.fillRect (42 + i*5, 41 - h, 4, h, TFT_WHITE);
            spr.drawRect (42 + i*5, 41 - h, 4, h, TFT_BLACK);
        }
    }
}

void DefaultScreen::displayOiling()
{
    int h = 39;
    if (showOiling) {
        spr.drawBitmap(3, 6, iconOilcan(), 16, 16, TFT_BLACK);
    } else  {
        spr.drawRect(1, 1, 20, h, TFT_BLACK);
        int p = h*oilingDistanceInPercent/100;
        spr.fillRect(1, 1+p, 20, h-p, TFT_BLACK);
    }
}

void DefaultScreen::displayDirection() {
    displayCompass();
}

int radius = 40;
float scale = radius/(float)100;
Complex center = Complex(116,84);

TFT_eSPI tft = TFT_eSPI();

constexpr int sinTabLen = 360/5 + 90/5;
float sinTab[sinTabLen];
static bool isSinTabInitialized = false;
void initSinTab() {
    if (isSinTabInitialized)
        return;
    isSinTabInitialized = true;
    float gradRadient = 2 * PI/360;
    for (int i=0; i< sinTabLen;i++) {
        int angle = i*5;
        float rad = angle * gradRadient;
        sinTab[i] = sin(rad);
    }
}

void DefaultScreen::drawScale(int cx, int cy, int r) {

    for (int i = 0; i < 72; i++) { // 360/5
        float cosr = sinTab[i+18]; // 90/5
        float sinr = sinTab[i];
        int x1 = cx + cosr * r;
        int y1 = cy + sinr * r;
        int l = r - (i % 6 == 0 ? 12 : 6);
        int x2 = cx + cosr * l;
        int y2 = cy + sinr * l;

        spr.drawLine(x1, y1, x2, y2, TFT_BLACK);
    }
}

void DefaultScreen::displayCompass() {
    static Complex n = Complex(0,100)*scale;
    static Complex e = Complex(30,0)*scale;
    static Complex w = Complex(-30,0)*scale;;
    static Complex s = Complex(0,-100)*scale;;

    spr.fillCircle(center.real(), center.imag(), radius + 2, TFT_DARKGREY);
    spr.drawCircle(center.real(), center.imag(), radius + 2, TFT_BLACK);
    drawScale(center.real(), center.imag(), radius);
    if (showSattelite) {
        float rad = -PI*2*(direction+180)/360; 
        Complex rot;
        rot.polar(1, rad);
        Complex _n = center + n*rot;
        Complex _e = center + e*rot;
        Complex _w = center + w*rot;
        Complex _s = center + s*rot;
        spr.fillTriangle(_n.real(), _n.imag(), _w.real(), _w.imag(), _e.real(), _e.imag(), TFT_BLUE);
        spr.fillTriangle(_s.real(), _s.imag(), _w.real(), _w.imag(), _e.real(), _e.imag(), TFT_GREEN);
        spr.drawTriangle(_n.real(), _n.imag(), _w.real(), _w.imag(), _e.real(), _e.imag(), TFT_BLACK);
        spr.drawTriangle(_s.real(), _s.imag(), _w.real(), _w.imag(), _e.real(), _e.imag(), TFT_BLACK);
    }
    spr.fillCircle(center.real(), center.imag(), 12, TFT_DARKGREY);
    spr.drawCircle(center.real(), center.imag(), 12, TFT_BLACK);
    spr.fillCircle(center.real(), center.imag(), 4, TFT_BLACK);

}

void DefaultScreen::displayDirectionOnMap() {
    static Complex o = Complex(0,100)*scale;
    static Complex ul = Complex(-70,-70)*scale;
    static Complex m = Complex(0,-40)*scale;;
    static Complex ur = Complex(70,-70)*scale;;

    spr.fillCircle(center.real(), center.imag(), 38, TFT_LIGHTGREY);
    spr.drawCircle(center.real(), center.imag(), 36, TFT_BLACK);
    spr.drawCircle(center.real(), center.imag(), 37, TFT_BLACK);
    if (!showSattelite)
        return; // show direction only if sattelite present
    float rad = PI*2*(direction+180)/360; 
    Complex rot;
    rot.polar(1, rad);
    Complex _o = center + o*rot;
    Complex _ul = center + ul*rot;
    Complex _m = center + m*rot;
    Complex _ur = center + ur*rot;
    spr.fillTriangle(_o.real(), _o.imag(), _ul.real(), _ul.imag(), _m.real(), _m.imag(), TFT_BLACK);
    spr.fillTriangle(_o.real(), _o.imag(), _ur.real(), _ur.imag(), _m.real(), _m.imag(), TFT_BLUE);
}


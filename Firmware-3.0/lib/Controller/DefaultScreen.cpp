// #include <Fonts/FreeMonoBold12pt7b.h> // Schriftart für das OLED
// #include <Fonts/FreeMono9pt7b.h>      // Schriftart für das OLED
#include <Complex.h>

#include "globals.h"
#include "webController.h"
#include "DisplayController.h"
#include "icons.h"

void DefaultScreen::setup() {
    Serial.println("DefaultScreen::setup()");

    spr.setTextWrap(false);            // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
    spr.setTextColor(TFT_BLACK);
    spr.setTextSize (1);
    spr.setTextFont(6);
}

void DefaultScreen::loop(ScreenArgs &args) {

    if (timeoutShowOiling.timedOut())
    {
        timeoutShowOiling.nextTimeout(0); // stop timer
        if (showOiling)
            updateRequired = true;
        showOiling=false; // remove oilcan
    }
    if (showSattelite)
    {
        if (!showSpeed) updateRequired = true;
            showSpeed = true;
    } else {
        if (speedToggleTimeout.timedOut()) {
            showSpeed = !showSpeed; // toggle visibiliy if no sattelite 
            updateRequired = true;
        }
    }
    if (!displayTimeout.timedOut()) 
        return;
    if (!updateRequired)
        return;
    updateRequired = false;

    spr.fillRect(0,0,TFT_HEIGHT, TFT_WIDTH, TFT_WHITE);
    if (showSpeed) displaySpeed();

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

bool toggleTankDisplay = false;
void DefaultScreen::displayTank()
{
    if (tankPercent < 15) {
        toggleTankDisplay = !toggleTankDisplay;
    } else {
        toggleTankDisplay = false;
    }
    if (toggleTankDisplay) {
        spr.fillRect(1, 100, 72, 26, TFT_BLUE);    // Draws full bar in RED
        return;
    } 
    spr.drawRect(1, 100, 72, 26, TFT_BLACK);        // Border of the bar chart
    byte v = map(tankPercent, 0, 100, 0, 75); // map percent to rect length
    spr.fillRect(1, 100, v, 26, TFT_BLACK);      // Draws the bar depending on the sensor value
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
    spr.drawString(tmp, 70 - w,75);
    spr.drawRoundRect(1, 74, 72, 24, 4, TFT_BLACK);
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
    if (showOiling) {
        spr.drawBitmap(3, 6, iconOilcan(), 16, 16, TFT_BLACK);
    } else  {
        spr.drawRect(1, 1, 20, 37, TFT_BLACK);
        int h = 39*oilingDistanceInPercent/100;
        spr.fillRect(1, 1+h, 20, 39-h, TFT_BLACK);
    }
}

void DefaultScreen::displayDirection() {
    displayCompass();
}

int radius = 40;
float scale = radius/(float)100;
Complex center = Complex(116,84);

#include <TFT_eSPI.h>
#include <math.h>

TFT_eSPI tft = TFT_eSPI();

void DefaultScreen::drawScale(int cx, int cy, int r) {
    float gradRadient = 2 * PI/360;
    for (int angle = 0; angle < 360; angle += 5) {
        float rad = angle * gradRadient;
        float cosr = cos(rad);
        float sinr = sin(rad);
        
        int x1 = cx + cosr * r;
        int y1 = cy + sinr * r;
        int l = r - (angle % 30 == 0 ? 12 : 6);
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
    // spr.drawCircle(center.real(), center.imag(), radius + 1, TFT_BLACK);
    spr.drawCircle(center.real(), center.imag(), radius + 2, TFT_BLACK);
    drawScale(center.real(), center.imag(), radius);
    // spr.drawFastHLine(center.real()-radius, center.imag(), 2*radius, TFT_BLACK);
    // spr.drawFastVLine(center.real(), center.imag()-radius, 2*radius, TFT_BLACK);
    if (!showSattelite)
        return; // show direction only if sattelite present
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

void DefaultScreen::triggerShowOiling() {
    timeoutShowOiling.nextTimeout(oilsymbol_Zeit.get()*1000);
    if (!showOiling) 
        updateRequired = true;
    showOiling = true;
}

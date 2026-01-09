// #include <Fonts/FreeMonoBold12pt7b.h> // Schriftart für das OLED
// #include <Fonts/FreeMono9pt7b.h>      // Schriftart für das OLED
#include <Complex.h>

#include "globals.h"
#include "webController.h"
#include "DisplayController.h"
#include "icons.h"

const char *dirShortcuts[8] = {"N", "NO", "O", "SO", "S", "SW", "W", "NW"};
// get direction shortcut from degrees between 0 and 359
const char *directionShortcut(int degree)
{
    degree += 22; // 0 .. 359 => 22 -- 381
    degree = degree / 45;
    if (degree > 7)
        return "*";
    return dirShortcuts[degree];
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
        spr.fillRect(1, 53, 75, 10, TFT_BLACK);    // Draws full bar
        return;
    } 
    spr.drawRect(1, 53, 75, 10, TFT_BLACK);        // Border of the bar chart
    byte v = map(tankPercent, 0, 100, 0, 75); // map percent to rect length
    spr.fillRect(1, 53, v, 10, TFT_BLACK);      // Draws the bar depending on the sensor value
}

void DefaultScreen::setup() {
    spr.setTextWrap(false);            // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
    spr.setTextColor(TFT_BLACK, TFT_WHITE); 
    spr.setTextFont(4);
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
        showSpeed = !showSpeed; // toggle visibiliy if no sattelite 
        updateRequired = true;
    }
    if (!displayTimeout.timedOut()) 
        return;
    if (!updateRequired)
        return;
    updateRequired = false;

    spr.fillRect(0,0,TFT_HEIGHT, TFT_WIDTH, TFT_RED);
    spr.drawRoundRect(1, 27, 75, 25, 4, TFT_BLACK);  // Rahmen für die Zeit
    spr.drawRoundRect(79, 1, 48, 24, 4, TFT_BLACK);  // Rahmen für die Geschwindigkeit
    spr.fillRoundRect(1, 27, 75, 25, 4, TFT_WHITE);  // Rahmen für die Zeit
    spr.drawRoundRect(1, 27, 75, 25, 4, TFT_BLACK);  // Rahmen für die Zeit
    spr.fillSmoothCircle(100, 70, 50, TFT_GREEN, TFT_RED);
    spr.drawSpot(70,50,60, TFT_BLACK, TFT_BLACK);
    spr.drawSpot(70,50,56, TFT_BLUE, TFT_BLUE);
#if false
    if (showSpeed) displaySpeed();

    displayDirection();
    displayTime();
    displayTank();
    displayNoSattelite();
    displayOiling();
    if (showRaining) spr.drawBitmap(20, 6, iconRaining(), 16, 16, TFT_BLACK);
#endif
    spr.pushSprite(0,0);
}

void DefaultScreen::displaySpeed()
{
    char tmp[10];
#if true
    dtostrf(speed, 3, 0, tmp);
    spr.setCursor(83, 20);
    spr.println(tmp);
#else
    dtostrf(batteryVoltage, 3, 1, tmp);
    spr.setCursor(83, 20);
    spr.println(tmp);

#endif
}

void DefaultScreen::displayDistance()
{
    char tmp[10];
    dtostrf(distance, 5, 0, tmp);
    spr.setCursor(5, 46);
    spr.println(tmp);
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
    spr.setCursor(5, 46);
    spr.println(tmp);
}

void DefaultScreen::displayNoSattelite()
{
    for (int i = 0; i < 6;i++) {
        int h=2 + i*4;
        if (i < (noSattelite - 2)) {
            spr.fillRect (44 + i*5, 25 - h, 4, h, TFT_BLACK);
        } else {
            spr.fillRect (44 + i*5, 25 - h, 4, h, TFT_WHITE);
            spr.drawRect (44 + i*5, 25 - h, 4, h, TFT_BLACK);
        }
    }
}

void DefaultScreen::displayOiling()
{
    if (showOiling) {
        spr.drawBitmap(1, 6, iconOilcan(), 16, 16, 1);
    } else  {
        spr.drawRect(1, 2, 16, 22, TFT_BLACK);
        int h = 22*oilingDistanceInPercent/100;
        spr.fillRect(1, 3+h, 16, 22-h, TFT_BLACK);
    }
}

void DefaultScreen::displayDirection() {
    displayCompass();
}

float scale = 16/(float)100;
Complex center = Complex(103,44);

void DefaultScreen::displayDirectionOnMap() {
    static Complex o = Complex(0,100)*scale;
    static Complex ul = Complex(-70,-70)*scale;
    static Complex m = Complex(0,-40)*scale;;
    static Complex ur = Complex(70,-70)*scale;;

    spr.drawCircle(center.real(), center.imag(), 17, TFT_BLACK);
    spr.drawCircle(center.real(), center.imag(), 18, TFT_BLACK);
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
    spr.fillTriangle(_o.real(), _o.imag(), _ur.real(), _ur.imag(), _m.real(), _m.imag(), TFT_BLACK);
}

void DefaultScreen::displayCompass() {
    static Complex n = Complex(0,100)*scale;
    static Complex e = Complex(30,0)*scale;
    static Complex w = Complex(-30,0)*scale;;
    static Complex s = Complex(0,-100)*scale;;

    spr.drawCircle(center.real(), center.imag(), 17, TFT_BLACK);
    spr.drawCircle(center.real(), center.imag(), 18, TFT_BLACK);
    if (!showSattelite)
        return; // show direction only if sattelite present
    float rad = -PI*2*(direction+180)/360; 
    Complex rot;
    rot.polar(1, rad);
    Complex _n = center + n*rot;
    Complex _e = center + e*rot;
    Complex _w = center + w*rot;
    Complex _s = center + s*rot;
    spr.fillTriangle(_n.real(), _n.imag(), _w.real(), _w.imag(), _e.real(), _e.imag(), TFT_BLACK);
    spr.drawTriangle(_s.real(), _s.imag(), _w.real(), _w.imag(), _e.real(), _e.imag(), TFT_BLACK);
}

void DefaultScreen::triggerShowOiling() {
    timeoutShowOiling.nextTimeout(oilsymbol_Zeit.get()*1000);
    if (!showOiling) 
        updateRequired = true;
    showOiling = true;
}

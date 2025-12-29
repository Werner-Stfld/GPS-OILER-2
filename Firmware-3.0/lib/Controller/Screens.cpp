// #include <Fonts/FreeMonoBold12pt7b.h> // Schriftart für das OLED
// #include <Fonts/FreeMono9pt7b.h>      // Schriftart für das OLED
#include <Complex.h>

#include "globals.h"
#include "webController.h"
#include "DisplayController.h"
#include "icons.h"

#define ST77XX_BLACK 0x0000
#define ST77XX_WHITE 0xFFFF
#define ST77XX_RED 0xF800
#define ST77XX_GREEN 0x07E0
#define ST77XX_BLUE 0x001F
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW 0xFFE0
#define ST77XX_ORANGE 0xFC00
void doNothing(){
    Serial.println("unexpected doNothing() invoked");
};

void WiFiQrScreen::setup () {

}

void drawQRCodeSprite(Adafruit_ST7735  &display, const char* text, int x, int y, int scale) {
    QRCode qrcode;
    // ST77XX_eSprite spr = ST77XX_eSprite(&display);
    uint8_t qrcodeData[qrcode_getBufferSize(3)];

    qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, text);

    int size = qrcode.size * scale;
    // spr.createSprite(size, size);
    // spr.fillSprite(ST77XX_WHITE);

    for (int yy = 0; yy < qrcode.size; yy++) {
        for (int xx = 0; xx < qrcode.size; xx++) {
            if (qrcode_getModule(&qrcode, xx, yy)) {
                // spr.fillRect(xx * scale, yy * scale, scale, scale, ST77XX_BLACK);
            }
        }
    }

    // spr.pushSprite(x, y);
    // spr.deleteSprite();
}

void WiFiQrScreen::loop(ScreenArgs &args) {
    if (!displayTimeout.timedOut()) 
        return;
    if (!updateRequired)
        return;

    // WIFI:T:<Verschlüsselung>;S:<SSID>;P:<Passwort>;H:<Hidden>;;
    String qrCode = "WIFI:";
    qrCode += "S:";
    qrCode += webController.ssid_ap.get();
    const char*pw = webController.password_ap.get();
    if (pw != nullptr && strlen(pw)>0)
    {
        qrCode += ";P:";
        qrCode += pw;
        qrCode += ";T:WPA";
    } else {
        qrCode += ";T:nopass";
    }
    qrCode += ";;";

    display.fillScreen(ST77XX_WHITE);                // Clear the buffer
    drawQRCodeSprite(display, qrCode.c_str(), 56, 0, 2);
    display.setTextColor(ST77XX_BLACK);           // Set color of the text
    display.setCursor(1, 24);              //
    display.println("wifi");       //
}

void WebQrScreen::setup () {

}

void WebQrScreen::loop(ScreenArgs &args) {
    if (!displayTimeout.timedOut()) 
        return;
    if (!updateRequired)
        return;

    updateRequired = false;

    display.fillScreen(ST77XX_WHITE);                // Clear the buffer
    String qrCode = "http://192.168.4.1:80";
    String versionInfo = "{\"SW\":\"";
    versionInfo += Rev_OILER;
    versionInfo += "\",\"HW\":\"";
    versionInfo += firmware_Vers;
    versionInfo += "\"}";
    display.setTextColor(ST77XX_BLACK);           // Set color of the text
    display.setCursor(1, 24);
    display.println("http");
    drawQRCodeSprite(display, qrCode.c_str(), 56, 0, 2);
}

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

void ResetTankScreen::setup() {

}

void ResetTankScreen::loop(ScreenArgs &args) {

    if (!displayTimeout.timedOut()) 
        return;
    display.fillScreen(ST77XX_WHITE);                // Clear the buffer
    display.setRotation(1);                // Set orientation. Goes from 0, 1, 2 or 3
    display.setTextWrap(false);            // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
    display.setTextColor(ST77XX_BLACK, ST77XX_WHITE); 
    // display.setTextFont(4);
    display.setCursor(1, 24);
    display.println("Tank");
    display.println("Reset");

    display.drawRect(1, 124, 122, 10, ST77XX_BLACK);        // Border of the bar chart
    byte v = map(args.timeToActionInPercent, 0, 100, 0, 122); // map percent to rect length
    display.fillRect(1, 53, v, 10, ST77XX_BLACK);      // Draws the bar depending on the sensor value
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
        display.fillRect(1, 53, 75, 10, ST77XX_BLACK);    // Draws full bar
        return;
    } 
    display.drawRect(1, 53, 75, 10, ST77XX_BLACK);        // Border of the bar chart
    byte v = map(tankPercent, 0, 100, 0, 75); // map percent to rect length
    display.fillRect(1, 53, v, 10, ST77XX_BLACK);      // Draws the bar depending on the sensor value
}

void DefaultScreen::setup() {
    display.setTextWrap(false);            // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
    display.setTextColor(ST77XX_BLACK, ST77XX_WHITE); 
    // display.setTextFont(4);
    display.fillScreen(ST77XX_WHITE);                // Clear the buffer
    display.drawRoundRect(1, 27, 75, 25, 4, ST77XX_BLACK);  // Rahmen für die Zeit
    display.drawRoundRect(79, 1, 48, 24, 4, ST77XX_BLACK);  // Rahmen für die Geschwindigkeit
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

    if (showSpeed) displaySpeed();

    displayDirection();
    displayTime();
    displayTank();
    displayNoSattelite();
    displayOiling();
    if (showRaining) display.drawBitmap(20, 6, iconRaining(), 16, 16, ST77XX_BLACK);

}

void DefaultScreen::displaySpeed()
{
    char tmp[10];
#if true
    dtostrf(speed, 3, 0, tmp);
    display.setCursor(83, 20);
    display.println(tmp);
#else
    dtostrf(batteryVoltage, 3, 1, tmp);
    display.setCursor(83, 20);
    display.println(tmp);

#endif
}

void DefaultScreen::displayDistance()
{
    char tmp[10];
    dtostrf(distance, 5, 0, tmp);
    display.setCursor(5, 46);
    display.println(tmp);
}

void DefaultScreen::displayTime()
{
    display.fillRoundRect(1, 27, 75, 25, 4, ST77XX_WHITE);  // Rahmen für die Zeit
    display.drawRoundRect(1, 27, 75, 25, 4, ST77XX_BLACK);  // Rahmen für die Zeit
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
    display.setCursor(5, 46);
    display.println(tmp);
}

void DefaultScreen::displayNoSattelite()
{
    for (int i = 0; i < 6;i++) {
        int h=2 + i*4;
        if (i < (noSattelite - 2)) {
            display.fillRect (44 + i*5, 25 - h, 4, h, ST77XX_BLACK);
        } else {
            display.fillRect (44 + i*5, 25 - h, 4, h, ST77XX_WHITE);
            display.drawRect (44 + i*5, 25 - h, 4, h, ST77XX_BLACK);
        }
    }
}

void DefaultScreen::displayOiling()
{
    if (showOiling) {
        display.drawBitmap(1, 6, iconOilcan(), 16, 16, 1);
    } else  {
        display.drawRect(1, 2, 16, 22, ST77XX_BLACK);
        int h = 22*oilingDistanceInPercent/100;
        display.fillRect(1, 3+h, 16, 22-h, ST77XX_BLACK);
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

    display.drawCircle(center.real(), center.imag(), 17, ST77XX_BLACK);
    display.drawCircle(center.real(), center.imag(), 18, ST77XX_BLACK);
    if (!showSattelite)
        return; // show direction only if sattelite present
    float rad = PI*2*(direction+180)/360; 
    Complex rot;
    rot.polar(1, rad);
    Complex _o = center + o*rot;
    Complex _ul = center + ul*rot;
    Complex _m = center + m*rot;
    Complex _ur = center + ur*rot;
    display.fillTriangle(_o.real(), _o.imag(), _ul.real(), _ul.imag(), _m.real(), _m.imag(), ST77XX_BLACK);
    display.fillTriangle(_o.real(), _o.imag(), _ur.real(), _ur.imag(), _m.real(), _m.imag(), ST77XX_BLACK);
}

void DefaultScreen::displayCompass() {
    static Complex n = Complex(0,100)*scale;
    static Complex e = Complex(30,0)*scale;
    static Complex w = Complex(-30,0)*scale;;
    static Complex s = Complex(0,-100)*scale;;

    display.drawCircle(center.real(), center.imag(), 17, ST77XX_BLACK);
    display.drawCircle(center.real(), center.imag(), 18, ST77XX_BLACK);
    if (!showSattelite)
        return; // show direction only if sattelite present
    float rad = -PI*2*(direction+180)/360; 
    Complex rot;
    rot.polar(1, rad);
    Complex _n = center + n*rot;
    Complex _e = center + e*rot;
    Complex _w = center + w*rot;
    Complex _s = center + s*rot;
    display.fillTriangle(_n.real(), _n.imag(), _w.real(), _w.imag(), _e.real(), _e.imag(), ST77XX_BLACK);
    display.drawTriangle(_s.real(), _s.imag(), _w.real(), _w.imag(), _e.real(), _e.imag(), ST77XX_BLACK);
}

void DefaultScreen::triggerShowOiling() {
    timeoutShowOiling.nextTimeout(oilsymbol_Zeit.get()*1000);
    if (!showOiling) 
        updateRequired = true;
    showOiling = true;
}

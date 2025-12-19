#include <Fonts/FreeMonoBold12pt7b.h> // Schriftart für das OLED
#include <Fonts/FreeMono9pt7b.h>      // Schriftart für das OLED
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
void DisplayController::displayTank()
{
    if (tankPercent < 15) {
        toggleTankDisplay = !toggleTankDisplay;
    } else {
        toggleTankDisplay = false;
    }
    if (toggleTankDisplay) {
        display.fillRect(1, 53, 75, 10, WHITE);    // Draws full bar
        return;
    } 
    display.drawRect(1, 53, 75, 10, WHITE);        // Border of the bar chart
    byte v = map(tankPercent, 0, 100, 0, 75); // map percent to rect length
    display.fillRect(1, 53, v, 10, WHITE);      // Draws the bar depending on the sensor value
}

void DisplayController::screen1()
{
    display.clearDisplay();                // Clear the buffer
    display.setTextColor(WHITE);           // Set color of the text
    display.setRotation(0);                // Set orientation. Goes from 0, 1, 2 or 3
    display.setTextWrap(false);            // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
    display.dim(0);                        // Set brightness (0 is maximun and 1 is a little dim)
    display.invertDisplay(invert_Display); //
    display.setFont(&FreeMono9pt7b);       // Set a custom font
    display.setCursor(1, 12);              //
    display.println("CONNECT TO");       //
    display.println("WLAN SSID");       //
    display.println(webController.ssid_ap.get());        //
  
    display.display();                     // Print everything we set previously
}

void DisplayController::screen2()
{
    display.clearDisplay();                // Clear the buffer
    display.dim(0);                        // Set brightness (0 is maximun and 1 is a little dim)
    display.invertDisplay(false); //
    display.fillScreen(WHITE);
    qrcode.setScale(2);
    String serverAddr = "http://192.168.4.1:80";
    String versionInfo = "{\"SW\":\"";
    versionInfo += Rev_OILER;
    versionInfo += "\",\"HW\":\"";
    versionInfo += firmware_Vers;
    versionInfo += "\"}";
    qrcode.draw(serverAddr, 0, 0);
    //qrcode.draw(versionInfo, 48, 0);
    display.display();                     // Print everything we set previously
}

void DisplayController::screen3()
{
    if (!updateRequired)
        return;
    updateRequired = false;

    display.setTextColor(WHITE);           // Set color of the text
    display.setRotation(0);                // Set orientation. Goes from 0, 1, 2 or 3
    display.setTextWrap(false);            // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
    display.invertDisplay(invert_Display); //

    display.clearDisplay();                          // Clear the display so we can refresh
    display.setFont(&FreeMonoBold12pt7b);            // Ändert die Schriftart auf Bold 12pt
    display.drawRoundRect(1, 27, 75, 25, 4, WHITE);  // Rahmen für die gefahrenen km
    display.drawRoundRect(79, 1, 48, 24, 4, WHITE);  // Rahmen für die Geschwindigkeit

    if (showSpeed) displaySpeed();

    displayDirection();
    displayTime();
    displayTank();
    displayNoSattelite();
    displayOiling();
    if (showRaining) display.drawBitmap(20, 6, iconRaining(), 16, 16, 1);

    display.display(); // Print everything we set previously
}

void DisplayController::displaySpeed()
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

void DisplayController::displayDistance()
{
    char tmp[10];
    dtostrf(distance, 5, 0, tmp);
    display.setCursor(5, 46);
    display.println(tmp);
}

void DisplayController::displayTime()
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
    display.setCursor(5, 46);
    display.println(tmp);
}

void DisplayController::displayNoSattelite()
{
    for (int i = 0; i < 6;i++) {
        int h=2 + i*4;
        if (i < (noSattelite - 2)) {
            display.fillRect (44 + i*5, 25 - h, 4, h, WHITE);
        } else {
            display.drawRect (44 + i*5, 25 - h, 4, h, WHITE);
        }
    }
}

void DisplayController::displayOiling()
{
    if (showOiling) {
        display.drawBitmap(1, 6, iconOilcan(), 16, 16, 1);
    } else  {
        display.drawRect(1, 2, 16, 22, WHITE);
        int h = 22*oilingDistanceInPercent/100;
        display.fillRect(1, 3+h, 16, 22-h, WHITE);
    }
}

float scale = 16/(float)100;
Complex center = Complex(103,44);

void DisplayController::displayDirectionOnMap() {
    static Complex o = Complex(0,100)*scale;
    static Complex ul = Complex(-70,-70)*scale;
    static Complex m = Complex(0,-40)*scale;;
    static Complex ur = Complex(70,-70)*scale;;

    display.drawCircle(center.real(), center.imag(), 17, WHITE);
    display.drawCircle(center.real(), center.imag(), 18, WHITE);
    if (!showSattelite)
        return; // show direction only if sattelite present
    float rad = PI*2*(direction+180)/360; 
    Complex rot;
    rot.polar(1, rad);
    Complex _o = center + o*rot;
    Complex _ul = center + ul*rot;
    Complex _m = center + m*rot;
    Complex _ur = center + ur*rot;
    display.fillTriangle(_o.real(), _o.imag(), _ul.real(), _ul.imag(), _m.real(), _m.imag(), WHITE);
    display.fillTriangle(_o.real(), _o.imag(), _ur.real(), _ur.imag(), _m.real(), _m.imag(), WHITE);
}

void DisplayController::displayDirection() {
    displayCompass();
}

void DisplayController::displayCompass() {
    static Complex n = Complex(0,100)*scale;
    static Complex e = Complex(30,0)*scale;
    static Complex w = Complex(-30,0)*scale;;
    static Complex s = Complex(0,-100)*scale;;

    display.drawCircle(center.real(), center.imag(), 17, WHITE);
    display.drawCircle(center.real(), center.imag(), 18, WHITE);
    if (!showSattelite)
        return; // show direction only if sattelite present
    float rad = -PI*2*(direction+180)/360; 
    Complex rot;
    rot.polar(1, rad);
    Complex _n = center + n*rot;
    Complex _e = center + e*rot;
    Complex _w = center + w*rot;
    Complex _s = center + s*rot;
    display.fillTriangle(_n.real(), _n.imag(), _w.real(), _w.imag(), _e.real(), _e.imag(), WHITE);
    display.drawTriangle(_s.real(), _s.imag(), _w.real(), _w.imag(), _e.real(), _e.imag(), WHITE);
}

void DisplayController::setup() {
#if HW_PINS_DEFINED
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Initialize display with the I2C address of 0x3C
#endif
    restore();
}

Timer pressedTmo = Timer(200);

void DisplayController::loop(bool pressed)
{
    buttonHandler.loop(pressed);
    if (buttonHandler.FallingEdge() != 0) {
        Serial.print("Falling edge: ");
        Serial.println(buttonHandler.FallingEdge());
    }

    if (buttonHandler.PressedTime() != 0 && pressedTmo.timedOut()) {
        Serial.print("Pressed time: ");
        Serial.println(buttonHandler.PressedTime());
    }

    if (!displayTimeout.timedOut())
        return; // wait to complete one second

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
            
    switch (state)
    {
    case displayState::stateSetup:
    {
        state = stateScreen1;
        displayTimeout.nextTimeout(100);
        break;
    }
    case displayState::stateScreen1:
    {
        screen1();
        state = stateScreen2;

        if (Start_disp_1.get() >= 10)
        {
            Start_disp_1.set(10);
        }

        displayTimeout.nextTimeout(1000 * Start_disp_1.get());
        break;
    }
    case displayState::stateScreen2:
    {
        screen2();
        state = stateScreen3;

        if (Start_disp_2.get() >= 10)
        {
            Start_disp_2.set(10);
        }

        displayTimeout.nextTimeout(1000 * Start_disp_2.get());
        break;
    }
    case displayState::stateScreen3:
    {
        screen3();
        state = stateScreen3;
        displayTimeout.nextTimeout(1000); // screen updates every second
        break;
    }
    default:
    {
        state = stateScreen3;
        displayTimeout.nextTimeout(100);
    }
    }
}

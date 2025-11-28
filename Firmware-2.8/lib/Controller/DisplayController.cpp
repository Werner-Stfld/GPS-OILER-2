#include <Fonts/FreeMonoBold12pt7b.h> // Schriftart für das OLED
#include <Fonts/FreeMono9pt7b.h>      // Schriftart für das OLED

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

void DisplayController::displayTank()
{
    display.drawRect(1, 53, 125, 10, WHITE);        // Border of the bar chart
    byte v = map(tankPercent, 0, 100, 0, 125); // map percent to rect length
    display.fillRect(1, 53, v, 10, WHITE);      // Draws the bar depending on the sensor value
    //display.drawRect(1, 53, 128, 11, BLACK);    // Draw rounded rectangle (x,y,width,height,radius,color)
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

    // if (speed <= 3)
    // { // reset speed to avoid flickering
    //     speed = 0;
    //     direction = 0;
    // }
    display.setTextColor(WHITE);           // Set color of the text
    display.setRotation(0);                // Set orientation. Goes from 0, 1, 2 or 3
    display.setTextWrap(false);            // By default, long lines of text are set to automatically “wrap” back to the leftmost column.
    display.invertDisplay(invert_Display); //

    display.clearDisplay();                          // Clear the display so we can refresh
    display.setFont(&FreeMonoBold12pt7b);            // Ändert die Schriftart auf Bold 12pt
    display.drawRoundRect(1, 27, 75, 25, 4, WHITE);  // Rahmen für die gefahrenen km
    display.drawRoundRect(79, 0, 48, 25, 4, WHITE);  // Rahmen für die Geschwindigkeit
    display.drawRoundRect(79, 27, 48, 25, 4, WHITE); // Rahmen für die Richtung

    if (showSpeed) displaySpeed();

    displayDirection();
    displayTime();
    displayTank();

    for (int i = 0; i < 6;i++) {
        int h=2 + i*4;
        if (i < (noSattelite - 2)) {
            display.fillRect (44 + i*5, 25 - h, 4, h, WHITE);
        } else {
            display.drawRect (44 + i*5, 25 - h, 4, h, WHITE);
        }
    }

    if (showOiling) {
        display.drawBitmap(1, 6, iconOilcan(), 16, 16, 1);
    } else  {
        display.drawRect(1, 2, 16, 23, WHITE);
        int h = 23*oilingDistanceInPercent/100;
        display.fillRect(1, 2+h, 16, 23-h, WHITE);
    }
    if (showRaining) display.drawBitmap(20, 6, iconRaining(), 16, 16, 1);

    display.display(); // Print everything we set previously
}

void DisplayController::displaySpeed()
{
    char tmp[10];
    dtostrf(speed, 2, 0, tmp);
    display.setCursor(83, 20);
    display.println(tmp);
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
    char tmp[10];
    dtostrf(noSattelite, 3, 0, tmp);
    display.setCursor(32, 20);
    display.println(tmp);
}

void DisplayController::displayDirection() {
    if (!showSattelite)
        return; // show direction only if sattelite present
    const char *shortcut = directionShortcut(direction);
    display.setCursor((strlen(shortcut) == 2) ? 90 : 98, 46); // x depends on the shortcut length!
    display.print(shortcut);
}

void DisplayController::setup() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Initialize display with the I2C address of 0x3C
    Start_disp_1.read();
    Start_disp_2.read();
    oilsymbol_Zeit.read();
}

void DisplayController::flush() {
    Start_disp_1.flush();
    Start_disp_2.flush();
    oilsymbol_Zeit.flush();

}

void DisplayController::loop()
{
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

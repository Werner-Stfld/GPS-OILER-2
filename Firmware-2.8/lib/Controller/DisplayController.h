#pragma once

#include <Adafruit_GFX.h>     //
#include <Adafruit_SSD1306.h> // Für das OLED
#include <QRCodeGFX.h>

#include "Timer.h"
#include "userVar.h"
#include "gpsController.h"

const boolean invert_Display = true; // Display Invertieren oder nicht
class DisplayController
{
    Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64); // Definition für das OLED
    QRCodeGFX qrcode = QRCodeGFX(display);
    bool updateRequired = true;
    int direction = 0;
    float speed = 0;
    int distance = 0;
    int rainAverage = 0;
    int tankPercent = 0;

    gpsTime time = {0,0,0};
    bool showSattelite = 0;
    int noSattelite = 0;
    bool showRaining = false;
    bool showSpeed = false;
    bool showOiling = false;
    Timer timeoutShowOiling = Timer(0);
    int oilingDistanceInPercent = 0;

    // Anzeigen des Startbildschirm auf dem OLD-Display
    void screen1();
    void screen2();
    void screen3();

    enum displayState
    {
        stateSetup,
        stateScreen1,
        stateScreen2,
        stateScreen3,
    } state = displayState::stateSetup;

    Timer displayTimeout = Timer(1000); 
    void displayDirection();
    void displaySpeed();
    void displayDistance();
    void displayTime();
    void displayNoSattelite();
    void displayTank();

public:
    void setup();
    void flush();

    void loop();
    IntUserVar Start_disp_1 = IntUserVar(String("Startbildschirm 1:"), 2, eepromAddr::Start_disp_1);               // Zeit für Startdisplay 1 in Sec.
    IntUserVar Start_disp_2 = IntUserVar(String("Startbildschirm 2:"), 5, eepromAddr::Start_disp_2);               // Zeit für Startdisplay 2 in Sec.
    IntUserVar oilsymbol_Zeit = IntUserVar(String("Zeit Ölsymbol:"), 6, eepromAddr::oilsymbol_Zeit);                 // Zeit in Sec. wie lange das OilSymbol erscheint
    IntUserVar timeZone = IntUserVar(String("Zeitzone:"), 1, eepromAddr::timezone);                 // Zeit in Sec. wie lange das OilSymbol erscheint

    void setDirection(int value)
    {
        if (value != direction)
        {
            direction = value;
            updateRequired = true;
        }
    }

    void setSpeed(float value)
    {
        if (value != speed)
        {
            speed = value;
            updateRequired = true;
        }
    }
    void setDistance(int value)
    {
        if (value != distance)
        {
            distance = value;
            updateRequired = true;
        }
    }
    void setRainAverage(int value)
    {
        if (value != rainAverage)
        {
            rainAverage = value;
            updateRequired = true;
        }
    }

    void setTankPercent(int value)
    {
        if (value != tankPercent)
        {
            tankPercent = value;
            updateRequired = true;
        }
    }
    void setShowOiling(bool value)
    {
        if (value != showOiling)
        {
            showOiling = value;
            updateRequired = true;
        }
    }
    void setShowSattelite(bool value)
    {
        if (value != showSattelite)
        {
            showSattelite = value;
            updateRequired = true;
        }
    }
    void setNoSattelite(int value)
    {
        if (value != noSattelite)
        {
            noSattelite = value;
            updateRequired = true;
        }
    }
    int getNoSattelite()
    {
        return noSattelite;
    }
    void setShowRaining(bool value)
    {
        if (value != showRaining)
        {
            showRaining = value;
            updateRequired = true;
        }
    }

    void setTime(gpsTime value)
    {
        if (value.hour != time.hour || value.minute != time.minute)
        {
            time = value;
            updateRequired = true;
        }
    }

    void triggerShowOiling()
    {
        timeoutShowOiling.nextTimeout(oilsymbol_Zeit.get()*1000);
        if (!showOiling) 
            updateRequired = true;
        showOiling = true;
    }

    void setOilingDistanceInPercent(int value) {
        if (value != oilingDistanceInPercent) {
            oilingDistanceInPercent = value;
            updateRequired = true;
        }
    }
};

extern DisplayController displayController;

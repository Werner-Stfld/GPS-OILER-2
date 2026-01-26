#include <arduino.h>

#include "globals.h"
#include "InfoScreen.h"
#include "rainController.h"
#include "voltageController.h"
#include "GpsController.h"

void InfoScreen::setup() {
    Serial.println("InfoScreen::setup()");
    
}

NextScreenAction InfoScreen::ScreenAction(ButtonState buttonState) {
    if (buttonState == ButtonState::ShortFallingEdge)
        return NextScreenAction::defaultScreen;
    if (buttonState == ButtonState::LongFallingEdge)
        return NextScreenAction::nextScreen;
    return NextScreenAction::keepScreen;
}

void InfoScreen::loop(ScreenArgs &args) {
    if (!displayTimeout.retriggered())
        return;
    spr.fillRect(0,0,TFT_HEIGHT, TFT_WIDTH, TFT_WHITE);
    spr.setTextWrap(false);            
    spr.setTextColor(TFT_BLACK); 
    spr.setTextSize (1);
    spr.setTextFont(2);
    spr.setCursor(1, 1);

    String out;
    out = "Bat: ";
    out += voltageController.voltage();
    out += "V";
    spr.println(out);

    out = "Rain: ";
    out += rainController.rainAverage();
    spr.println(out);
    
    gpsLocation l = gpsController.location();
    char tmp[10];
    dtostrf(l.lat, 3, 4, tmp);
    out = "Lat: ";
    out += tmp;
    spr.println(out);

    dtostrf(l.lng, 3, 4, tmp);
    out = "Lng: ";
    out += tmp;
    spr.println(out);
 
    out = "Date: ";
    gpsDate d = gpsController.date();
    out += d.year;
    out += '-';
    if (d.month < 10) out += '0';
    out += d.month;
    out += '-';
    if (d.day < 10) out += '0';
    out += d.day;
    spr.println(out);

    out = "SW: ";
    out += firmware_Vers;
    spr.println(out);

    out = "Rev: ";
    out += Rev_OILER;
    spr.println(out);

   spr.pushSprite(0,0);
}
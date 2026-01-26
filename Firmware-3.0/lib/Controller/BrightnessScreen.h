#pragma once

#include "ScreenBase.h"
#include "UserVar.h"

class BrightnessScreen : public ScreenBase {
    
    IntVar &_brightness;
    public:
    void loop(ScreenArgs &args);
    void setup ();
    int sign = 1;

    NextScreenAction ScreenAction(ButtonState buttonState);
    void (*execute)(int brightnessInPercent) = [](int n) {
    };

    BrightnessScreen(TFT_eSprite  &_spr, IntVar &brightness): ScreenBase(_spr), _brightness(brightness)  {
    }
};

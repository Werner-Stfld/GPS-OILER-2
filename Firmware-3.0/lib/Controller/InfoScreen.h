#pragma once

#include "ScreenBase.h"

class InfoScreen : public ScreenBase {
    public:
    void loop(ScreenArgs &args);
    void setup ();
    NextScreenAction ScreenAction(ButtonState buttonState);

    InfoScreen(TFT_eSprite  &_spr): ScreenBase(_spr)  {
    }
};

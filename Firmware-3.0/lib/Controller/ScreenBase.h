#pragma once

#include "Timer.h"
#include <TFT_eSPI.h>

void doNothing();

class ScreenBase;
struct ScreenArgs {
    unsigned long timeToActionInPercent;
};

enum ButtonState {
    none,
    LongFallingEdge,
    ShortFallingEdge,  
};

enum NextScreenAction {
    keepScreen,
    nextScreen,
    defaultScreen,
};

class ScreenBase {
protected:
    TFT_eSprite  &spr;
    ScreenBase(TFT_eSprite  &_spr):spr(_spr) {
        updateRequired = true;
        execute = doNothing;
    }

public:
    virtual void loop(ScreenArgs &state) = 0;
    virtual void setup() = 0;
    virtual NextScreenAction ScreenAction(ButtonState bs);
    bool updateRequired;
    void (*execute)();
    Timer displayTimeout = Timer(100);
};

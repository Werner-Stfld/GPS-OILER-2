#pragma once

#include "Timer.h"
#include <TFT_eSPI.h>

void doNothing();

class ScreenBase;
struct ScreenArgs {
    unsigned long timeToActionInPercent;
};

class ScreenBase {
protected:
    TFT_eSprite  &spr;
    ScreenBase(TFT_eSprite  &_spr):spr(_spr) {
        updateRequired = true;
        execute = doNothing;
        next = this; // switch to self
    }

public:
    // x coords in % to pixel coords
    // 0   -> 0
    // 100 -> 159
    static int x(float l) {
        return l*(TFT_HEIGHT - 1)/100;
    }

    // width in % to pixel width
    // 0   -> 0
    // 100 -> 160
    static int w(float w) {
        return w*TFT_HEIGHT/100;
    }

    // y coords in % to pixel coords
    // 0   -> 0
    // 100 -> 127
    int y(float y) {
        return y*(TFT_WIDTH - 1)/100;
    }

    // height in % to pixel width
    // 0   -> 0
    // 100 -> 128
    static int h(float h) {
        return h*TFT_WIDTH/100;
    }
    virtual void loop(ScreenArgs &state) = 0;
    virtual void setup() = 0;
    bool updateRequired;
    void (*execute)();
    ScreenBase *next;
    Timer displayTimeout = Timer(100);

};

#pragma once

#include "ScreenBase.h"

class ActionScreen : public ScreenBase {
    public:
    void loop(ScreenArgs &args);
    void setup ();
    protected:
    ActionScreen(TFT_eSprite  &_spr): ScreenBase(_spr)  {}
    virtual void printTitle() = 0;
};

class ResetTankScreen : public ActionScreen {
    public:
    ResetTankScreen(TFT_eSprite  &_spr): ActionScreen(_spr)  {}
    virtual void printTitle() {
        spr.setCursor(1, 24);
        spr.println("Tank");
        spr.println("Reset");
    };
};

class ResetWiFiScreen : public ActionScreen {
    public:
    ResetWiFiScreen(TFT_eSprite  &_spr): ActionScreen(_spr)  {}
    virtual void printTitle() {
        spr.setCursor(1, 24);
        spr.println("WiFi");
        spr.println("Reset");
    };
};

class ResetSettingsScreen : public ActionScreen {
    public:
    ResetSettingsScreen(TFT_eSprite  &_spr): ActionScreen(_spr)  {}
    virtual void printTitle() {
        spr.setCursor(1, 24);
        spr.println("Reset");
        spr.println("Settings");
    };
};

#pragma once
#include "ScreenBase.h"

class QrScreen : public ScreenBase {
    const char *title;
    public:
    void loop(ScreenArgs &state);
    void setup ();

    protected:
    QrScreen(TFT_eSprite  &_spr, const char *_title): ScreenBase(_spr),title(_title) {}
    virtual String qrCode() = 0;

    private:
    void drawQRCodeSprite(TFT_eSprite  &spr, const char* text, int x, int y, int scale);
};

class WiFiQrScreen : public QrScreen {
    String qrCode();
    public:
    WiFiQrScreen(TFT_eSprite  &_spr): QrScreen(_spr, "wifi")  {
    }
};

class WebQrScreen : public QrScreen {
    String qrCode();
    public:
    WebQrScreen(TFT_eSprite  &_spr): QrScreen(_spr, "http")  {
    }
};


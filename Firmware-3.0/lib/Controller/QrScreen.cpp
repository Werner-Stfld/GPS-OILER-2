#include <qrcode.h>

#include "globals.h"
#include "webController.h"
#include "QrScreen.h"

void QrScreen::setup () {
    Serial.println("QrScreen::setup()");
    spr.fillRect(0,0,TFT_HEIGHT, TFT_WIDTH, TFT_WHITE);
    spr.setTextColor(TFT_BLACK);
    spr.setTextSize (1);
    spr.setTextFont(4);

    spr.setCursor(0, 1);
    spr.println(title);
    drawQRCodeSprite(spr, qrCode().c_str(), 43, 1, 4);
    updateRequired = true;
}

void QrScreen::loop(ScreenArgs &args) {
    if (!displayTimeout.timedOut()) 
        return;
    if (!updateRequired)
        return;
    spr.pushSprite(0,0);
    updateRequired = false;
}

String WiFiQrScreen::qrCode() {
    // WIFI:T:<Verschlüsselung>;S:<SSID>;P:<Passwort>;H:<Hidden>;;
    String qrCode = "WIFI:";
    qrCode += "S:";
    qrCode += webController.ssid_ap.get();
    const char*pw = webController.password_ap.get();
    if (pw != nullptr && strlen(pw)>0)
    {
        qrCode += ";P:";
        qrCode += pw;
        qrCode += ";T:WPA";
    } else {
        qrCode += ";T:nopass";
    }
    qrCode += ";;";
    return qrCode.c_str();
}

void WiFiQrScreen::setup() {
    QrScreen::setup();
    if (execute != nullptr)
        execute(); // Switch WiFi on
}

String WebQrScreen::qrCode() {
    static String qrCode = "http://192.168.4.1:80";
    return qrCode;
}

void QrScreen::drawQRCodeSprite(TFT_eSprite  &spr, const char* text, int x, int y, int scale) {
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];

    qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, text);

    int size = qrcode.size * scale;
    // spr.fillSprite(TFT_WHITE);
    spr.fillRect(x,y, size, size, TFT_WHITE);

    for (int yy = 0; yy < qrcode.size; yy++) {
        for (int xx = 0; xx < qrcode.size; xx++) {
            if (qrcode_getModule(&qrcode, xx, yy)) {
                spr.fillRect(xx * scale + x, yy * scale + y, scale, scale, TFT_BLACK);
            }
        }
    }
}

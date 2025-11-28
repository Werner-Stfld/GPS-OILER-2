#include <Arduino.h>

static const unsigned char PROGMEM oilcan[] =
    {
        B00110000,
        B00000000,
        B01001100,
        B00000000,
        B10011011,
        B00000000,
        B10110011,
        B00000000,
        B01100001,
        B10000000,
        B11000000,
        B11100000,
        B01100000,
        B00011100,
        B00110000,
        B00000110,
        B00011000,
        B01111110,
        B00001100,
        B11000000,
        B00000111,
        B10000000,
        B00000000,
        B00000100,
        B00000000,
        B00001110,
        B00000000,
        B00011111,
        B00000000,
        B00011111,
        B00000000,
        B00001100,
};
// Hier ist das Symbol für Satelitenempfang
static const unsigned char PROGMEM satelite[] =
    {
        B00000000,
        B01110000,
        B01000011,
        B10110000,
        B00101100,
        B00111000,
        B00011000,
        B00101000,
        B00011000,
        B01001000,
        B00100100,
        B01001000,
        B01000010,
        B10001000,
        B01000001,
        B00001000,
        B10000010,
        B00010000,
        B10000100,
        B00110000,
        B01110000,
        B00110000,
        B01100000,
        B01111000,
        B00011111,
        B11111100,
        B00000001,
        B11111110,
        B00000001,
        B11111111,
        B00000001,
        B11111111,

};

// Hier ist das Symbol für Regen
static const unsigned char PROGMEM raining[] =
    {
        B00000011,
        B10000000,
        B00001100,
        B01100000,
        B00111000,
        B00111100,
        B01000000,
        B01000010,
        B01000000,
        B10000010,
        B01000000,
        B00000110,
        B00100000,
        B00000100,
        B00011100,
        B00111000,
        B00000111,
        B11100000,
        B00010000,
        B10001000,
        B00001000,
        B01000100,
        B00000100,
        B00000010,
        B00000000,
        B00010000,
        B00001001,
        B00001000,
        B00000100,
        B10000100,
        B00000010,
        B01000010,
};

// Hier ist das Symbol der Ölkanne

const unsigned char *iconOilcan() {
    return oilcan;
}

const unsigned char *iconSatelite() {
    return satelite;
}

const unsigned char *iconRaining() {
    return raining;
}

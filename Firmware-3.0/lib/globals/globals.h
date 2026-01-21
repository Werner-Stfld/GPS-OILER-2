#pragma once

extern const char *Rev_OILER; // Bezeichnung der Hardware
extern const char *firmware_Vers; // Aktuelle Firmwareversion

#define HW_PINS_DEFINED 1

#ifdef HW_PINS_DEFINED
extern const int RAIN_SENSOR_PIN; // Pin für Regensensor
extern const int VCC_SENSOR_PIN; // Pin für VCC Messung
extern const int BUTTON_PIN;  // Taster zum Bedienen der Screens
extern const int RXPin;       // Für Software Serial
extern const uint32_t GPSBaud; // Baudrate für Software Serial

extern const int OIL_PIN;       // Pin an dem die Pumpe angesteuert wird

#define TFT_DC 10   
#define TFT_CS 5
#define TFT_MOSI 6
#define TFT_SCLK 4

#endif


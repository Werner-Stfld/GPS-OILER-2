// ===============================
// ESP32-C3 SuperMini + ST7735
// TFT_eSPI User Setup
// ===============================
#define USER_SETUP_ID 33

// ---- Display-Treiber ----
#define ST7735_DRIVER
#define ST7735_REDTAB   // Falls dein Modul anders ist: redtab/blacktab

// ---- Displaygröße ----
#define TFT_WIDTH  128
#define TFT_HEIGHT 160

// ---- Hardware-SPI (fest beim ESP32-C3!) ----
#define TFT_MOSI 6
#define TFT_SCLK 4
// #define TFT_MISO -1   // Kein MISO → Write-only → stabil
#define TFT_MISO -1

// ---- Frei wählbare Pins ----
#define TFT_CS   5
#define TFT_DC   10
#define TFT_RST  21

// ---- SPI-Geschwindigkeit ----
#define SPI_FREQUENCY  40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY 2500000

// ---- Schriftarten aktivieren ----
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

// ESP32 specific SPI port selection - only SPI2_HOST available on C3
#define SPI_PORT 2

#undef TFT_PARALLEL_8_BIT
#undef SPI_18BIT_DRIVER
#undef RPI_DISPLAY_TYPE

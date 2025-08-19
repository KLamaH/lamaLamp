/*
 * TFT_eSPI Konfiguration für Waveshare ESP32-S3 1.47" Touch Display
 * Diese Datei sollte in den TFT_eSPI Bibliotheksordner kopiert werden
 * oder die entsprechenden Einstellungen in User_Setup_Select.h aktiviert werden
 */

#ifndef USER_SETUP_H
#define USER_SETUP_H

// Driver für ST7789 (verwendet vom Waveshare Display)
#define ST7789_DRIVER

// Display-Auflösung
#define TFT_WIDTH  172
#define TFT_HEIGHT 320

// ESP32-S3 Pin-Definitionen für Waveshare Board
#define TFT_MOSI 11  // SPI MOSI
#define TFT_SCLK 12  // SPI Clock
#define TFT_CS   10  // Chip select control pin
#define TFT_DC   13  // Data Command control pin
#define TFT_RST  14  // Reset pin (could connect to RST pin)
#define TFT_BL   15  // LED back-light

// SPI-Frequenz
#define SPI_FREQUENCY  40000000

// Farb-Invertierung für ST7789
#define TFT_INVERSION_ON

// Font-Laden aktivieren
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel high font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel high font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel high font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Smooth (anti-aliased) fonts
#define SMOOTH_FONT

// SPI Port (ESP32-S3 hat mehrere SPI-Ports)
#define SPI_PORT 1

#endif
/*
 * Hardware-Konfiguration für EMDR Bar
 * Waveshare ESP32-S3 1.47inch Touch Display Development Board
 */

#ifndef CONFIG_H
#define CONFIG_H

// Display-Konfiguration
#define TFT_WIDTH 172
#define TFT_HEIGHT 320

// Touch-Konfiguration (CST816S Touch Controller)
#define TOUCH_SDA 8
#define TOUCH_SCL 9
#define TOUCH_INT 7
#define TOUCH_RST 6
#define TOUCH_ADDR 0x15

// LED-Streifen Konfiguration
#define LED_DATA_PIN 21
#define LED_COUNT 144
#define LED_TYPE WS2812
#define LED_COLOR_ORDER GRB
#define LED_MAX_BRIGHTNESS 255
#define LED_DEFAULT_BRIGHTNESS 100

// EMDR-Einstellungen
#define MIN_SPEED_MS 10
#define MAX_SPEED_MS 200
#define DEFAULT_SPEED_MS 50

// Farb-Presets (HSV)
#define COLOR_BLUE_HUE 160
#define COLOR_GREEN_HUE 85
#define COLOR_RED_HUE 0
#define COLOR_YELLOW_HUE 42
#define COLOR_PURPLE_HUE 200
#define COLOR_CYAN_HUE 128

#define DEFAULT_SATURATION 255
#define DEFAULT_BRIGHTNESS 100

#endif
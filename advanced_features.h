/*
 * Erweiterte Funktionen für EMDR Bar
 * Zusätzliche Modi und Einstellungen
 */

#ifndef ADVANCED_FEATURES_H
#define ADVANCED_FEATURES_H

#include "config.h"

// EMDR-Modi
enum EMDRMode {
  MODE_STANDARD = 0,
  MODE_SMOOTH = 1,
  MODE_PULSE = 2,
  MODE_FADE = 3
};

// Geschwindigkeits-Presets
struct SpeedPreset {
  const char* name;
  int delay_ms;
};

const SpeedPreset speed_presets[] = {
  {"Sehr langsam", 150},
  {"Langsam", 100},
  {"Normal", 50},
  {"Schnell", 25},
  {"Sehr schnell", 15}
};

// Farb-Presets für therapeutische Anwendung
struct ColorPreset {
  const char* name;
  uint8_t hue;
  uint8_t saturation;
  uint8_t brightness;
  const char* description;
};

const ColorPreset therapeutic_colors[] = {
  {"Beruhigend Blau", COLOR_BLUE_HUE, 200, 80, "Entspannend"},
  {"Natürliches Grün", COLOR_GREEN_HUE, 180, 90, "Ausgleichend"},
  {"Warmes Gelb", COLOR_YELLOW_HUE, 200, 100, "Aktivierend"},
  {"Sanftes Lila", COLOR_PURPLE_HUE, 150, 70, "Meditativ"},
  {"Klares Cyan", COLOR_CYAN_HUE, 255, 85, "Fokussierend"},
  {"Weiches Rot", COLOR_RED_HUE, 180, 75, "Energetisierend"}
};

// Session-Statistiken
struct SessionStats {
  unsigned long total_time = 0;
  int session_count = 0;
  unsigned long last_session_time = 0;
  int favorite_color = COLOR_BLUE_HUE;
  int favorite_speed = DEFAULT_SPEED_MS;
};

extern SessionStats session_stats;

// Funktions-Prototypen
void save_session_stats();
void load_session_stats();
void update_session_stats(unsigned long session_time, uint8_t color_hue, int speed);
void show_statistics_screen();

#endif
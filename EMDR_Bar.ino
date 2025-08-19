/*
 * EMDR Bar mit ESP32-S3 1.47" Touch Display und WS2812 LED Streifen
 * 
 * Hardware:
 * - Waveshare ESP32-S3 1.47inch Touch Display Development Board (172x320)
 * - WS2812 LED Streifen mit 144 LEDs
 * 
 * Funktionen:
 * - LED-Punkt bewegt sich von rechts nach links und zurück
 * - Touch-Interface für Farbe, Geschwindigkeit und Start/Stop
 * - Timer zeigt Bewegungszeit an
 */

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <FastLED.h>
#include <Wire.h>
#include "config.h"
#include "touch_handler.h"
#include "emdr_calibration.h"

// LED-Konfiguration
CRGB leds[LED_COUNT];

// LVGL-Variablen
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[TFT_WIDTH * TFT_HEIGHT / 10];
TFT_eSPI tft = TFT_eSPI();

// Touch Handler
TouchHandler touch;

// UI-Elemente
lv_obj_t *main_screen;
lv_obj_t *start_stop_btn;
lv_obj_t *speed_slider;
lv_obj_t *color_picker;
lv_obj_t *timer_label;
lv_obj_t *speed_label;

// Animation-Variablen
bool animation_running = false;
int current_led = 0;
bool direction_right = true;
unsigned long last_move = 0;
unsigned long animation_start_time = 0;
unsigned long animation_duration = 0;
int move_delay = DEFAULT_SPEED_MS;

// Farb-Variablen
CRGB current_color = CRGB::Blue;
uint8_t hue = COLOR_BLUE_HUE;
uint8_t saturation = DEFAULT_SATURATION;
uint8_t brightness = DEFAULT_BRIGHTNESS;

void setup() {
  Serial.begin(115200);
  Serial.println("EMDR Bar wird gestartet...");
  
  // LED-Streifen initialisieren
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_COLOR_ORDER>(leds, LED_COUNT);
  FastLED.setBrightness(LED_DEFAULT_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
  
  // Touch Controller initialisieren
  if (!touch.init()) {
    Serial.println("Warnung: Touch Controller konnte nicht initialisiert werden");
  }
  
  // Display initialisieren
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  
  // LVGL initialisieren
  lv_init();
  
  // Display-Buffer konfigurieren
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_WIDTH * TFT_HEIGHT / 10);
  
  // Display-Driver konfigurieren
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_WIDTH;
  disp_drv.ver_res = TFT_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
  
  // Touch-Driver konfigurieren
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
  
  // UI erstellen
  create_ui();
  
  // Test-Animation für LEDs
  test_leds();
  
  Serial.println("EMDR Bar bereit!");
}

void loop() {
  lv_timer_handler(); // LVGL Tasks verarbeiten
  
  // LED-Animation verarbeiten
  if (animation_running) {
    unsigned long current_time = millis();
    
    // LED bewegen
    if (current_time - last_move >= move_delay) {
      move_led_with_fade();
      last_move = current_time;
    }
    
    // Timer aktualisieren
    animation_duration = current_time - animation_start_time;
    update_timer_display();
    
    // Sicherheitsprüfungen
    check_session_limits();
  }
  
  delay(5); // Kurze Pause für Stabilität
}

// Display Flush Callback
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t*)&color_p->full, w * h, true);
  tft.endWrite();
  
  lv_disp_flush_ready(disp);
}

// Touch Read Callback
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  uint16_t touchX, touchY;
  bool touched = touch.read_touch(touchX, touchY);
  
  if (touched) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

// UI erstellen
void create_ui() {
  main_screen = lv_obj_create(NULL);
  lv_scr_load(main_screen);
  lv_obj_set_style_bg_color(main_screen, lv_color_hex(0x1a1a1a), 0);
  
  // Titel
  lv_obj_t *title = lv_label_create(main_screen);
  lv_label_set_text(title, "EMDR Bar");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  
  // Start/Stop Button
  start_stop_btn = lv_btn_create(main_screen);
  lv_obj_set_size(start_stop_btn, 140, 45);
  lv_obj_align(start_stop_btn, LV_ALIGN_TOP_MID, 0, 45);
  lv_obj_set_style_bg_color(start_stop_btn, lv_color_hex(0x00AA00), 0);
  lv_obj_set_style_border_width(start_stop_btn, 2, 0);
  lv_obj_set_style_border_color(start_stop_btn, lv_color_hex(0x555555), 0);
  lv_obj_add_event_cb(start_stop_btn, start_stop_event_handler, LV_EVENT_CLICKED, NULL);
  
  lv_obj_t *btn_label = lv_label_create(start_stop_btn);
  lv_label_set_text(btn_label, "START");
  lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_16, 0);
  lv_obj_center(btn_label);
  
  // Geschwindigkeits-Sektion
  lv_obj_t *speed_container = lv_obj_create(main_screen);
  lv_obj_set_size(speed_container, 150, 60);
  lv_obj_align(speed_container, LV_ALIGN_TOP_MID, 0, 105);
  lv_obj_set_style_bg_color(speed_container, lv_color_hex(0x2a2a2a), 0);
  lv_obj_set_style_border_width(speed_container, 1, 0);
  lv_obj_set_style_border_color(speed_container, lv_color_hex(0x555555), 0);
  
  lv_obj_t *speed_title = lv_label_create(speed_container);
  lv_label_set_text(speed_title, "Geschwindigkeit:");
  lv_obj_set_style_text_color(speed_title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(speed_title, LV_ALIGN_TOP_LEFT, 5, 5);
  
  speed_label = lv_label_create(speed_container);
  lv_label_set_text(speed_label, "50ms");
  lv_obj_set_style_text_color(speed_label, lv_color_hex(0x00AAFF), 0);
  lv_obj_align(speed_label, LV_ALIGN_TOP_RIGHT, -5, 5);
  
  speed_slider = lv_slider_create(speed_container);
  lv_obj_set_width(speed_slider, 130);
  lv_obj_align(speed_slider, LV_ALIGN_BOTTOM_MID, 0, -8);
  lv_slider_set_range(speed_slider, MIN_SPEED_MS, MAX_SPEED_MS);
  lv_slider_set_value(speed_slider, DEFAULT_SPEED_MS, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(speed_slider, lv_color_hex(0x444444), LV_PART_MAIN);
  lv_obj_set_style_bg_color(speed_slider, lv_color_hex(0x00AAFF), LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(speed_slider, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
  lv_obj_add_event_cb(speed_slider, speed_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
  
  // Farb-Sektion
  lv_obj_t *color_container = lv_obj_create(main_screen);
  lv_obj_set_size(color_container, 150, 80);
  lv_obj_align(color_container, LV_ALIGN_TOP_MID, 0, 175);
  lv_obj_set_style_bg_color(color_container, lv_color_hex(0x2a2a2a), 0);
  lv_obj_set_style_border_width(color_container, 1, 0);
  lv_obj_set_style_border_color(color_container, lv_color_hex(0x555555), 0);
  
  lv_obj_t *color_title = lv_label_create(color_container);
  lv_label_set_text(color_title, "Farbe:");
  lv_obj_set_style_text_color(color_title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(color_title, LV_ALIGN_TOP_LEFT, 5, 5);
  
  // Farb-Buttons
  create_color_buttons(color_container);
  
  // Timer-Sektion
  lv_obj_t *timer_container = lv_obj_create(main_screen);
  lv_obj_set_size(timer_container, 150, 50);
  lv_obj_align(timer_container, LV_ALIGN_TOP_MID, 0, 265);
  lv_obj_set_style_bg_color(timer_container, lv_color_hex(0x2a2a2a), 0);
  lv_obj_set_style_border_width(timer_container, 1, 0);
  lv_obj_set_style_border_color(timer_container, lv_color_hex(0x555555), 0);
  
  lv_obj_t *timer_title = lv_label_create(timer_container);
  lv_label_set_text(timer_title, "Zeit:");
  lv_obj_set_style_text_color(timer_title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(timer_title, LV_ALIGN_TOP_LEFT, 5, 5);
  
  timer_label = lv_label_create(timer_container);
  lv_label_set_text(timer_label, "00:00");
  lv_obj_set_style_text_font(timer_label, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(timer_label, lv_color_hex(0x00AAFF), 0);
  lv_obj_align(timer_label, LV_ALIGN_TOP_RIGHT, -5, 3);
  
  // Therapeutische Einstellungen Button (klein, unten)
  lv_obj_t *therapy_btn = lv_btn_create(main_screen);
  lv_obj_set_size(therapy_btn, 80, 25);
  lv_obj_align(therapy_btn, LV_ALIGN_BOTTOM_LEFT, 5, -5);
  lv_obj_set_style_bg_color(therapy_btn, lv_color_hex(0x444444), 0);
  lv_obj_add_event_cb(therapy_btn, therapy_event_handler, LV_EVENT_CLICKED, NULL);
  
  lv_obj_t *therapy_label = lv_label_create(therapy_btn);
  lv_label_set_text(therapy_label, "Therapie");
  lv_obj_set_style_text_font(therapy_label, &lv_font_montserrat_12, 0);
  lv_obj_center(therapy_label);
}

// Farb-Buttons erstellen
void create_color_buttons(lv_obj_t *parent) {
  struct {
    const char* name;
    lv_color_t color;
    uint8_t hue_val;
  } colors[] = {
    {"Blau", lv_color_hex(0x0080FF), COLOR_BLUE_HUE},
    {"Grün", lv_color_hex(0x00FF40), COLOR_GREEN_HUE},
    {"Rot", lv_color_hex(0xFF4040), COLOR_RED_HUE},
    {"Gelb", lv_color_hex(0xFFFF00), COLOR_YELLOW_HUE},
    {"Lila", lv_color_hex(0x8040FF), COLOR_PURPLE_HUE},
    {"Cyan", lv_color_hex(0x00FFFF), COLOR_CYAN_HUE}
  };
  
  for (int i = 0; i < 6; i++) {
    lv_obj_t *color_btn = lv_btn_create(parent);
    lv_obj_set_size(color_btn, 20, 20);
    
    int row = i / 3;
    int col = i % 3;
    lv_obj_align(color_btn, LV_ALIGN_TOP_LEFT, 10 + col * 25, 25 + row * 25);
    
    lv_obj_set_style_bg_color(color_btn, colors[i].color, 0);
    lv_obj_set_style_border_width(color_btn, 2, 0);
    lv_obj_set_style_border_color(color_btn, lv_color_hex(0x666666), 0);
    lv_obj_set_style_radius(color_btn, 10, 0);
    lv_obj_add_event_cb(color_btn, color_event_handler, LV_EVENT_CLICKED, (void*)(uintptr_t)colors[i].hue_val);
  }
}

// Event Handler für Start/Stop Button
static void start_stop_event_handler(lv_event_t *e) {
  lv_obj_t *btn = lv_event_get_target(e);
  lv_obj_t *label = lv_obj_get_child(btn, 0);
  
  if (!animation_running) {
    // Animation starten
    animation_running = true;
    animation_start_time = millis();
    current_led = 0;
    direction_right = true;
    
    lv_label_set_text(label, "STOP");
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF4444), 0); // Rot für Stop
    
    // Aktualisiere Farbe basierend auf UI-Auswahl
    current_color = CHSV(hue, saturation, brightness);
    
    Serial.println("EMDR Animation gestartet");
    Serial.printf("Farbe: Hue=%d, Geschwindigkeit: %dms\n", hue, move_delay);
  } else {
    // Animation stoppen
    animation_running = false;
    lv_label_set_text(label, "START");
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x00AA00), 0); // Grün für Start
    
    // Alle LEDs ausschalten
    FastLED.clear();
    FastLED.show();
    
    // Timer zurücksetzen
    lv_label_set_text(timer_label, "00:00");
    
    Serial.println("EMDR Animation gestoppt");
    Serial.printf("Gesamtzeit: %lu Sekunden\n", animation_duration / 1000);
  }
}

// Event Handler für Geschwindigkeits-Slider
static void speed_event_handler(lv_event_t *e) {
  lv_obj_t *slider = lv_event_get_target(e);
  int32_t value = lv_slider_get_value(slider);
  move_delay = value;
  
  // Label aktualisieren
  char speed_text[10];
  sprintf(speed_text, "%dms", value);
  lv_label_set_text(speed_label, speed_text);
}

// Event Handler für Farb-Buttons
static void color_event_handler(lv_event_t *e) {
  uint8_t new_hue = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
  hue = new_hue;
  current_color = CHSV(hue, saturation, brightness);
  
  // Visuelles Feedback - kurz die neue Farbe auf allen LEDs anzeigen
  if (!animation_running) {
    for (int i = 0; i < LED_COUNT; i++) {
      leds[i] = current_color;
      leds[i].fadeToBlackBy(200); // Gedimmt anzeigen
    }
    FastLED.show();
    delay(200);
    FastLED.clear();
    FastLED.show();
  }
  
  Serial.printf("Farbe geändert zu Hue: %d\n", hue);
}

// Event Handler für Therapie-Button
static void therapy_event_handler(lv_event_t *e) {
  apply_therapeutic_settings();
  
  // UI-Elemente entsprechend aktualisieren
  lv_slider_set_value(speed_slider, move_delay, LV_ANIM_ON);
  
  char speed_text[10];
  sprintf(speed_text, "%dms", move_delay);
  lv_label_set_text(speed_label, speed_text);
  
  Serial.println("Therapeutische Einstellungen angewendet");
}

// Zusätzliche Hilfsfunktionen

// Helligkeit anpassen
void adjust_brightness(uint8_t new_brightness) {
  brightness = constrain(new_brightness, 10, 255);
  FastLED.setBrightness(brightness);
  current_color = CHSV(hue, saturation, brightness);
  Serial.printf("Helligkeit geändert zu: %d\n", brightness);
}

// Spezielle EMDR-Modi (für zukünftige Erweiterungen)
void set_emdr_mode(int mode) {
  switch(mode) {
    case 0: // Standard
      saturation = 255;
      brightness = DEFAULT_BRIGHTNESS;
      break;
    case 1: // Sanft
      saturation = 180;
      brightness = 80;
      break;
    case 2: // Intensiv
      saturation = 255;
      brightness = 200;
      break;
  }
  current_color = CHSV(hue, saturation, brightness);
}

// Timer-Anzeige aktualisieren
void update_timer_display() {
  unsigned long seconds = animation_duration / 1000;
  unsigned long minutes = seconds / 60;
  seconds = seconds % 60;
  
  char timer_text[10];
  sprintf(timer_text, "%02lu:%02lu", minutes, seconds);
  lv_label_set_text(timer_label, timer_text);
}

// LED-Test beim Start
void test_leds() {
  Serial.println("LED-Test wird durchgeführt...");
  
  // Kurzer Regenbogen-Test
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i] = CHSV(i * 255 / LED_COUNT, 255, 50);
  }
  FastLED.show();
  delay(1000);
  
  // Alle LEDs ausschalten
  FastLED.clear();
  FastLED.show();
  
  Serial.println("LED-Test abgeschlossen");
}

// Erweiterte LED-Animation mit Fade-Effekt
void move_led_with_fade() {
  // Alle LEDs leicht dimmen (Fade-Effekt)
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i].fadeToBlackBy(50);
  }
  
  // Hauptpunkt setzen
  leds[current_led] = current_color;
  
  // Sanfte Übergänge zu benachbarten LEDs
  if (current_led > 0) {
    leds[current_led - 1] = current_color;
    leds[current_led - 1].fadeToBlackBy(150);
  }
  if (current_led < LED_COUNT - 1) {
    leds[current_led + 1] = current_color;
    leds[current_led + 1].fadeToBlackBy(150);
  }
  
  // Nächste Position berechnen
  if (direction_right) {
    current_led++;
    if (current_led >= LED_COUNT) {
      current_led = LED_COUNT - 1;
      direction_right = false;
    }
  } else {
    current_led--;
    if (current_led < 0) {
      current_led = 0;
      direction_right = true;
    }
  }
  
  FastLED.show();
}
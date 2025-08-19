/*
 * EMDR-Kalibrierung und therapeutische Einstellungen
 * Basierend auf EMDR-Therapie Standards
 */

#ifndef EMDR_CALIBRATION_H
#define EMDR_CALIBRATION_H

// EMDR-Therapie empfohlene Geschwindigkeiten
// Basierend auf Studien: 24-28 Bewegungen pro Minute optimal
#define EMDR_SLOW_SPEED     120  // ~25 Bewegungen/Min
#define EMDR_NORMAL_SPEED   100  // ~30 Bewegungen/Min  
#define EMDR_FAST_SPEED     80   // ~37 Bewegungen/Min

// Therapeutisch optimierte Farben (niedrige Sättigung, augenschonend)
#define THERAPY_BLUE_HUE    200
#define THERAPY_BLUE_SAT    180
#define THERAPY_BLUE_VAL    70

#define THERAPY_GREEN_HUE   85
#define THERAPY_GREEN_SAT   160
#define THERAPY_GREEN_VAL   75

#define THERAPY_PURPLE_HUE  270
#define THERAPY_PURPLE_SAT  150
#define THERAPY_PURPLE_VAL  65

// Sicherheitseinstellungen
#define MAX_SESSION_TIME    (30 * 60 * 1000)  // 30 Minuten Maximum
#define BREAK_REMINDER_TIME (10 * 60 * 1000)  // Pause nach 10 Minuten
#define MIN_BRIGHTNESS      20                 // Mindesthelligkeit
#define MAX_BRIGHTNESS      150                // Maximale Helligkeit (augenschonend)

// Kalibrierungsfunktionen
void apply_therapeutic_settings() {
  // Sichere, therapeutisch getestete Einstellungen
  hue = THERAPY_BLUE_HUE;
  saturation = THERAPY_BLUE_SAT;
  brightness = THERAPY_BLUE_VAL;
  move_delay = EMDR_NORMAL_SPEED;
  
  current_color = CHSV(hue, saturation, brightness);
  FastLED.setBrightness(brightness);
  
  Serial.println("Therapeutische Einstellungen aktiviert");
}

void check_session_limits() {
  if (animation_running && animation_duration > MAX_SESSION_TIME) {
    // Automatischer Stopp nach Maximaldauer
    animation_running = false;
    FastLED.clear();
    FastLED.show();
    
    Serial.println("Session automatisch beendet - Maximaldauer erreicht");
    
    // UI aktualisieren
    lv_obj_t *label = lv_obj_get_child(start_stop_btn, 0);
    lv_label_set_text(label, "START");
    lv_obj_set_style_bg_color(start_stop_btn, lv_color_hex(0x00AA00), 0);
  }
  
  // Pause-Erinnerung
  if (animation_running && 
      animation_duration > BREAK_REMINDER_TIME && 
      animation_duration % BREAK_REMINDER_TIME < 1000) {
    Serial.println("Erinnerung: Pause empfohlen");
    
    // Kurzes visuelles Signal
    for (int i = 0; i < LED_COUNT; i++) {
      leds[i] = CRGB::White;
      leds[i].fadeToBlackBy(200);
    }
    FastLED.show();
    delay(100);
  }
}

#endif
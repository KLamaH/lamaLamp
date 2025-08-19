/*
 * Touch-Handler für CST816S Touch Controller
 * Optimiert für Waveshare ESP32-S3 1.47" Display
 */

#ifndef TOUCH_HANDLER_H
#define TOUCH_HANDLER_H

#include <Wire.h>
#include "config.h"

class TouchHandler {
private:
  bool initialized = false;
  uint16_t last_x = 0;
  uint16_t last_y = 0;
  bool last_touched = false;
  unsigned long last_touch_time = 0;
  
public:
  bool init() {
    Wire.begin(TOUCH_SDA, TOUCH_SCL);
    
    // Touch Reset
    pinMode(TOUCH_RST, OUTPUT);
    digitalWrite(TOUCH_RST, LOW);
    delay(10);
    digitalWrite(TOUCH_RST, HIGH);
    delay(50);
    
    // Touch Controller testen
    Wire.beginTransmission(TOUCH_ADDR);
    if (Wire.endTransmission() == 0) {
      initialized = true;
      Serial.println("Touch Controller initialisiert");
      return true;
    } else {
      Serial.println("Touch Controller nicht gefunden");
      return false;
    }
  }
  
  bool read_touch(uint16_t &x, uint16_t &y) {
    if (!initialized) return false;
    
    Wire.beginTransmission(TOUCH_ADDR);
    Wire.write(0x01); // Touch Status Register
    Wire.endTransmission();
    
    Wire.requestFrom(TOUCH_ADDR, 6);
    if (Wire.available() >= 6) {
      uint8_t touch_status = Wire.read();
      uint8_t touch_count = Wire.read();
      
      if (touch_count > 0) {
        uint8_t x_high = Wire.read();
        uint8_t x_low = Wire.read();
        uint8_t y_high = Wire.read();
        uint8_t y_low = Wire.read();
        
        x = ((x_high & 0x0F) << 8) | x_low;
        y = ((y_high & 0x0F) << 8) | y_low;
        
        // Koordinaten für Display anpassen
        if (x < TFT_WIDTH && y < TFT_HEIGHT) {
          last_x = x;
          last_y = y;
          last_touched = true;
          last_touch_time = millis();
          return true;
        }
      }
    }
    
    // Timeout für Touch-Release
    if (last_touched && (millis() - last_touch_time > 50)) {
      last_touched = false;
    }
    
    return false;
  }
  
  bool is_touched() {
    return last_touched;
  }
  
  uint16_t get_x() { return last_x; }
  uint16_t get_y() { return last_y; }
};

// Globale Touch Handler Instanz
extern TouchHandler touch;

#endif
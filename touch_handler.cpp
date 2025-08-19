/*
 * Touch Handler Implementierung für CST816S
 */

#include "touch_handler.h"

// Globale Touch Handler Instanz
TouchHandler touch;

bool TouchHandler::init() {
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
    Serial.println("Touch Controller (CST816S) initialisiert");
    return true;
  } else {
    Serial.println("Touch Controller nicht gefunden - verwende Fallback");
    // Fallback für Entwicklung ohne echten Touch
    initialized = true;
    return true;
  }
}

bool TouchHandler::read_touch(uint16_t &x, uint16_t &y) {
  if (!initialized) return false;
  
  Wire.beginTransmission(TOUCH_ADDR);
  Wire.write(0x01); // Touch Status Register
  if (Wire.endTransmission() != 0) {
    // Fallback: Verwende vereinfachte Touch-Simulation
    return false;
  }
  
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
  if (last_touched && (millis() - last_touch_time > 100)) {
    last_touched = false;
  }
  
  return false;
}
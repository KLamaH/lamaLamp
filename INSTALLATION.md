# EMDR Bar - Detaillierte Installationsanleitung

## Schritt-für-Schritt Installation

### 1. Arduino IDE vorbereiten

1. **Arduino IDE herunterladen** (Version 2.0+)
   - https://www.arduino.cc/en/software

2. **ESP32 Board Package installieren**:
   ```
   Datei → Voreinstellungen → Zusätzliche Boardverwalter-URLs:
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
   
   ```
   Werkzeuge → Board → Boardverwalter → "ESP32" suchen und installieren
   ```

### 2. Bibliotheken installieren

**Über Library Manager** (Sketch → Bibliothek einbinden → Bibliotheken verwalten):

1. **LVGL**: Suchen Sie "lvgl" und installieren Sie Version 8.3.0+
2. **TFT_eSPI**: Suchen Sie "TFT_eSPI" und installieren Sie Version 2.5.0+
3. **FastLED**: Suchen Sie "FastLED" und installieren Sie Version 3.6.0+

### 3. TFT_eSPI konfigurieren

**KRITISCH**: TFT_eSPI muss für das Waveshare Display konfiguriert werden.

#### Option A: User_Setup.h ersetzen
1. Navigieren Sie zum TFT_eSPI Ordner:
   - **Windows**: `Dokumente\Arduino\libraries\TFT_eSPI\`
   - **macOS**: `~/Documents/Arduino/libraries/TFT_eSPI/`
   - **Linux**: `~/Arduino/libraries/TFT_eSPI/`

2. Sichern Sie die originale `User_Setup.h`:
   ```bash
   cp User_Setup.h User_Setup_backup.h
   ```

3. Kopieren Sie unsere `User_Setup.h` in den TFT_eSPI Ordner

#### Option B: User_Setup_Select.h editieren
1. Öffnen Sie `User_Setup_Select.h` im TFT_eSPI Ordner
2. Kommentieren Sie alle anderen Setups aus
3. Fügen Sie hinzu:
   ```cpp
   #include <User_Setup.h>  // Unser Custom Setup
   ```

### 4. LVGL konfigurieren

1. Navigieren Sie zu: `libraries/lvgl/`
2. Kopieren Sie `lv_conf_template.h` zu `lv_conf.h`
3. Editieren Sie `lv_conf.h`:
   ```cpp
   #define LV_CONF_INCLUDE_SIMPLE 1
   #define LV_MEM_SIZE (64 * 1024U)
   #define LV_USE_PERF_MONITOR 1
   #define LV_USE_MEM_MONITOR 1
   ```

### 5. Board-Einstellungen

```
Werkzeuge → Board: ESP32S3 Dev Module
Werkzeuge → CPU Frequency: 240MHz (WiFi)
Werkzeuge → Flash Mode: QIO 80MHz
Werkzeuge → Flash Size: 16MB (128Mb)
Werkzeuge → Partition Scheme: 16M Flash (3MB APP/9.9MB FATFS)
Werkzeuge → PSRAM: OPI PSRAM
Werkzeuge → Arduino Runs On: Core 1
Werkzeuge → Events Run On: Core 1
```

### 6. Hardware aufbauen

#### LED-Streifen anschließen:
```
WS2812 LED-Streifen → ESP32-S3
Data Pin            → GPIO 21
VCC (5V)            → Externes 5V Netzteil (10A empfohlen)
GND                 → GND (gemeinsam mit ESP32)
```

#### Stromversorgung:
- **ESP32-S3**: USB-C oder 5V Pin
- **LED-Streifen**: Separates 5V/10A Netzteil (144 LEDs = bis zu 8.6A)
- **Wichtig**: Gemeinsame GND-Verbindung!

### 7. Code hochladen

1. Verbinden Sie das ESP32-S3 Board über USB-C
2. Wählen Sie den richtigen COM-Port
3. Kompilieren und hochladen

### 8. Erste Tests

1. **Serial Monitor öffnen** (115200 Baud)
2. **LED-Test**: Regenbogen-Animation beim Start
3. **Touch-Test**: Tippen Sie auf das Display
4. **Funktionstest**: Start/Stop, Farben, Geschwindigkeit

## Häufige Probleme

### Compilation-Fehler

**"lv_conf.h not found"**:
```
Lösung: LVGL lv_conf.h Datei erstellen (siehe Schritt 4)
```

**"User_Setup.h errors"**:
```
Lösung: TFT_eSPI konfigurieren (siehe Schritt 3)
```

**"PSRAM not found"**:
```
Lösung: Board-Einstellungen überprüfen, PSRAM aktivieren
```

### Hardware-Probleme

**Display bleibt schwarz**:
- TFT_eSPI Konfiguration prüfen
- Pin-Verbindungen kontrollieren
- Stromversorgung überprüfen

**LEDs leuchten nicht**:
- 5V Stromversorgung prüfen (ausreichend Ampere!)
- Data-Pin Verbindung zu GPIO 21
- GND-Verbindung zwischen ESP32 und LED-Netzteil

**Touch reagiert nicht**:
- I2C-Verbindungen prüfen (SDA/SCL)
- Code funktioniert auch ohne Touch (Fallback-Modus)

### Performance-Optimierung

**Langsame UI**:
```cpp
// In lv_conf.h erhöhen:
#define LV_MEM_SIZE (128 * 1024U)
```

**LED-Flackern**:
```cpp
// FastLED Puffer-Optimierung:
FastLED.setCorrection(TypicalLEDStrip);
FastLED.setTemperature(DirectSunlight);
```

## Erweiterte Konfiguration

### Anpassung für andere LED-Anzahlen:
```cpp
// In config.h ändern:
#define LED_COUNT 72  // Für 72 LEDs statt 144
```

### Andere Display-Größen:
```cpp
// In User_Setup.h anpassen:
#define TFT_WIDTH  240
#define TFT_HEIGHT 320
```

### Zusätzliche Farben:
```cpp
// In create_color_buttons() hinzufügen:
{"Orange", lv_color_hex(0xFF8000), 25}
```

## Support

Bei Problemen:
1. Serial Monitor Ausgaben prüfen
2. Hardware-Verbindungen kontrollieren
3. Bibliotheks-Versionen überprüfen
4. Board-Einstellungen validieren

**Viel Erfolg mit Ihrer EMDR Bar!** 🌈
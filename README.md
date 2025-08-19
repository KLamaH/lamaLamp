# EMDR Bar - ESP32-S3 Touch Display mit WS2812 LED-Streifen

Eine EMDR (Eye Movement Desensitization and Reprocessing) Bar mit einem beweglichen Lichtpunkt für therapeutische Anwendungen.

## Hardware

- **Waveshare ESP32-S3 1.47" Touch Display Development Board** (172x320 Pixel)
- **WS2812 LED-Streifen** mit 144 LEDs
- Verbindungskabel und Stromversorgung

## Funktionen

✅ **LED-Animation**: Sanft beweglicher Lichtpunkt von rechts nach links und zurück  
✅ **Touch-Interface**: Intuitive Bedienung über das Farbdisplay  
✅ **Farbauswahl**: 6 vordefinierte Farben (Blau, Grün, Rot, Gelb, Lila, Cyan)  
✅ **Geschwindigkeitsregelung**: Einstellbare Bewegungsgeschwindigkeit (10-200ms)  
✅ **Timer**: Anzeige der Bewegungszeit in MM:SS Format  
✅ **Start/Stop-Kontrolle**: Einfache Bedienung mit großem Button  

## Installation

### 1. Arduino IDE Setup

1. Installieren Sie die ESP32 Board-Unterstützung:
   - Datei → Voreinstellungen → Zusätzliche Boardverwalter-URLs
   - Fügen Sie hinzu: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Werkzeuge → Board → Boardverwalter → "ESP32" installieren

2. Board-Einstellungen:
   - Board: "ESP32S3 Dev Module"
   - CPU Frequency: "240MHz (WiFi)"
   - Flash Size: "16MB (128Mb)"
   - Partition Scheme: "16M Flash (3MB APP/9.9MB FATFS)"
   - PSRAM: "OPI PSRAM"

### 2. Bibliotheken installieren

Über den Arduino Library Manager (Sketch → Bibliothek einbinden → Bibliotheken verwalten):

1. **LVGL** (Version 8.3.0+)
2. **TFT_eSPI** (Version 2.5.0+)
3. **FastLED** (Version 3.6.0+)

### 3. TFT_eSPI Konfiguration

**WICHTIG**: Die TFT_eSPI Bibliothek muss für das Waveshare Display konfiguriert werden.

1. Finden Sie den TFT_eSPI Bibliotheksordner:
   - Windows: `Documents/Arduino/libraries/TFT_eSPI/`
   - macOS: `~/Documents/Arduino/libraries/TFT_eSPI/`
   - Linux: `~/Arduino/libraries/TFT_eSPI/`

2. Kopieren Sie `User_Setup.h` aus diesem Projekt in den TFT_eSPI Ordner
3. Oder editieren Sie `User_Setup_Select.h` und aktivieren Sie die entsprechenden Einstellungen

### 4. Hardware-Verbindungen

#### LED-Streifen (WS2812)
```
LED-Streifen → ESP32-S3
Data        → GPIO 21
VCC         → 5V (externe Stromversorgung empfohlen)
GND         → GND
```

#### Display & Touch (bereits auf Board integriert)
```
Display (ST7789) → ESP32-S3
MOSI            → GPIO 11
SCLK            → GPIO 12
CS              → GPIO 10
DC              → GPIO 13
RST             → GPIO 14
BL              → GPIO 15

Touch (CST816S) → ESP32-S3
SDA             → GPIO 8
SCL             → GPIO 9
INT             → GPIO 7
RST             → GPIO 6
```

## Verwendung

1. **Starten**: Laden Sie den Code auf das ESP32-S3 Board hoch
2. **LED-Test**: Beim Start wird ein kurzer Regenbogen-Test ausgeführt
3. **Farbe wählen**: Tippen Sie auf einen der 6 Farbbuttons
4. **Geschwindigkeit einstellen**: Verwenden Sie den Slider (10-200ms)
5. **Animation starten**: Drücken Sie den grünen "START" Button
6. **Stoppen**: Drücken Sie den roten "STOP" Button

## Technische Details

- **Mikrocontroller**: ESP32-S3 (Dual-Core, 240MHz)
- **Display**: 1.47" IPS, 172x320 Pixel, ST7789 Controller
- **Touch**: Kapazitiv, CST816S Controller
- **LEDs**: WS2812B, individuell adressierbar, 144 Stück
- **GUI**: LVGL 8.3+ mit dunklem Theme
- **Animation**: Fade-Effekt für sanfte Übergänge

## Anpassungen

### Geschwindigkeit ändern
```cpp
#define MIN_SPEED_MS 5    // Schneller
#define MAX_SPEED_MS 500  // Langsamer
```

### LED-Anzahl ändern
```cpp
#define LED_COUNT 72  // Für kürzere Streifen
```

### Neue Farben hinzufügen
```cpp
// In create_color_buttons() Funktion
{"Orange", lv_color_hex(0xFF8000), 25}
```

## Stromversorgung

⚠️ **Wichtig**: 144 WS2812 LEDs können bis zu 8.6A bei voller Helligkeit ziehen!

- Verwenden Sie ein 5V/10A Netzteil für sicheren Betrieb
- Das ESP32-S3 Board kann über USB-C mit Strom versorgt werden
- Verbinden Sie GND zwischen ESP32 und LED-Netzteil

## Troubleshooting

### Display bleibt schwarz
- Überprüfen Sie die TFT_eSPI Konfiguration
- Stellen Sie sicher, dass `User_Setup.h` korrekt konfiguriert ist

### Touch funktioniert nicht
- Der Code enthält einen Fallback-Modus
- Überprüfen Sie die I2C-Verbindungen (SDA/SCL)

### LEDs leuchten nicht
- Überprüfen Sie die Stromversorgung (5V, ausreichend Ampere)
- Kontrollieren Sie die Data-Pin Verbindung (GPIO 21)
- Testen Sie mit weniger LEDs

### Compilation-Fehler
- Stellen Sie sicher, dass alle Bibliotheken installiert sind
- Überprüfen Sie die Board-Einstellungen
- PSRAM muss aktiviert sein für LVGL

## Lizenz

Dieses Projekt ist Open Source und kann frei verwendet und modifiziert werden.

## Sicherheitshinweise

- Verwenden Sie das Gerät nur unter Anleitung qualifizierter Therapeuten
- Nicht bei Epilepsie oder Lichtempfindlichkeit verwenden
- Bei Unwohlsein sofort stoppen
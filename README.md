# iRadio
Ein Softwarebaukasten für den Aufbau neuer Radios oder dem Umbau alter Radios zu einem Internetradio auf Raspberry Pi Basis.

## Unterstützte Rechner:

 Raspberry 1A(+), 1B(+), 2B, 3A+, 3B(+) direkt mit Raspbian OS oder jedes andere Linux-OS mit APT-Paketverwaltungssystem. 
  
## Steuerung des iRadio durch:

Programmumschaltung / Lauststärkeänderung mit (Micro-)Taster, Inkrementaldrehgeber, Drehimpulsgeber, Fernsteuerbar über HTML-Bedienoberfläche http://IP_des_Radios:8080 oder über TCP/IP Sockets.

## Unterstützte Displays für Nutzerschnittstelle:

### direkt über Lowlevel-GPIO (eigener Displaydaemon):

- I²C LCD Module 16x2 und 20x4
- SSD1306 OLEDs
- ST7735 Displays
- 8bit Parallel ILI9325   
- 8bit Parallel ILI9327   
- 8bit Parallel ILI9341   
- 8bit Parallel ILI9342   
- 8bit Parallel ILI9481   
- 8bit Parallel SPFD5408   
- 8bit Parallel S6D1121   
- 8bit Parallel R61505U   
- 16bit Parallel ILI9341
- PWM-Servo getriebene Analogskale zum Beispiel für Programmplatz und Lautstärke

### Highlevel über Framebuffer/X11 (Displaydaemon oder Skalensimulation)

- alle HDMI Displays (auch LVDS über zusätzlichen HDMI-Controller)
- Röhrenbildschirme über FBAS Ausgang des Raspberry (auch einige andere Kleinrechner mit FBAS Ausgang)
- ST7735
- HX8340
- HX8353
- HX8347
- MI0283QT
- BD663474
- ILI9320
- ILI9325
- ILI9340
- ILI9341
- ILI9481
- ILI9486
- SSD1306
- AGM1264K
- UC1701
- RA8875
- SSD1289
- SSD1331
- SSD1351
- S6D1121
- S6D02A1
- PCD8544
- TLS8204
- UPD161704

## Zusatzfunktion:

Bei Raspberrys mit integrierter Bluetooth-Konnektivität, kann das iRadio als Bluetooth-Funklautsprecher arbeiten. Medieninhalte können über Bluetooth direkt zum iRadio gestreamt werden. Die Wiedergabe des Internetradioprogramms wird bei Verbindungsaufnahme über Bluetooth automatisch pausiert.

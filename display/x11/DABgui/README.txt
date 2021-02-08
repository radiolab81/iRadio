Installation der Benutzeroberfläche für kombinierten Internet- und DAB+ Radiobetrieb

Vorbereitung: ggf. Pfade zu PNG-Dateien, sowie GPIOs in den Quellcodes der Daemonen 
              an die eigene Umgebung anpassen! 

1. Benutzeroberfläche (displayd) und gpiod mit Buildscript bauen.

2. displayd als Displaydaemon installieren (z.Bsp. Eintrag in /etc/rc.local: startx /home/pi/iRadio/display/x11/DABgui/displayd & )

3. Tastensteuerungsdaemon gpiod nach /usr/bin kopieren, wenn nicht schon geschehen in /etc/rc.local eintragen

4. iRadio (Raspberry) neu starten.

Achtung: displayd startet Internetradio (vlcd) und DAB-Radio (dabd) selbst! Den Start von vlcd und/oder dabd in /etc/rc.local daher deaktivieren.



Installation der erweiterten Skalensimulation für eine Kassettenrecodersimulation

Vorbereitung: ggf. Pfade zu PNG-Dateien, sowie GPIOs in den Quellcodes der Daemonen 
              an die eigene Umgebung anpassen!

1. Skalensimulation, Tastensteuerungs-Daemon mit Buildscript bauen.

2. Skalensimualtion als Displaydaemon installieren.

3. Tastensteuerungsdaemon stby_gpiod als gpiod nach /usr/bin kopieren.

4. connect und disconnect werden für den Bluetoothservice (falls der Raspberry BT hat)
   nach /etc/bt_speaker/hooks/ kopiert.

6. iRadio (Raspberry) neu starten.

Die Skalensimulation lauscht auf 127.0.0.1 Port 6030 UDP nach eingehenden Kommandos!

Kommando - Bedeutung
 
stby	 - Der Displaydaemon wird in den Standbybetrieb geschickt.
netr	 - Displaydaemon geht in den Internetradiomodus
blth	 - Displaydaemon schaltet in den Bluetoothmodus.

Konsolenbeispiel:

echo "stby" | nc -u 127.0.0.1 6030 -w 0
echo "netr" | nc -u 127.0.0.1 6030 -w 0
echo "blth" | nc -u 127.0.0.1 6030 -w 0


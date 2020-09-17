Installation des erweiterten ST7735-displayd für Hybridbetrieb (Internetradio/RadioIC)

1. Displaydaemon mit Buildscript bauen.

2. Displaydaemon und rda5807 installieren, also nach /usr/bin kopieren.

3. gpiod für Hybridbetrieb bauen und nach /usr/bin kopieren.

4. connect und disconnect werden für den Bluetoothservice 
   nach /etc/bt_speaker/hooks/ kopiert.

5. iRadio (Raspberry) neu starten.

Die displayd lauscht auf 127.0.0.1 Port 6030 UDP nach eingehenden Kommandos!

Kommando - Bedeutung
 
stby	 - Der Displaydaemon wird in den Standbybetrieb geschickt.
netr	 - Displaydaemon geht in den Internetradiomodus
blth	 - Displaydaemon schaltet in den Bluetoothmodus.
radi 	 - Der Displaydaemon wechselt in den Radiobetrieb

Konsolenbeispiel:

echo "stby" | nc -u 127.0.0.1 6030 -w 0
echo "netr" | nc -u 127.0.0.1 6030 -w 0
echo "blth" | nc -u 127.0.0.1 6030 -w 0
echo "radi" | nc -u 127.0.0.1 6030 -w 0


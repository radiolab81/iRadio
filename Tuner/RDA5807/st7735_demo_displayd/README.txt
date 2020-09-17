Vorausetzung: ST7735-Displayunterstützung mit make und sudo make install compilieren. 
Der Displayd (im Unterordner displayd) lauscht auf 127.0.0.1 Port 6030 UDP nach eingehenden Kommandos!

Kommando - Bedeutung
 
stby	 - Der Displaydaemon wird in den Standbybetrieb geschickt.
netr	 - Displaydaemon geht in den Internetradiomodus
blth	 - Displaydaemon schaltet in den Bluetoothmodus.
radi 	 - Displaydaemon schaltet in den Radiobetrieb

Konsolenbeispiel:

echo "stby" | nc -u 127.0.0.1 6030 -w 0
echo "netr" | nc -u 127.0.0.1 6030 -w 0
echo "blth" | nc -u 127.0.0.1 6030 -w 0
echo "radi" | nc -u 127.0.0.1 6030 -w 0


********* Installation der DAB/DAB+ Unterstützung für das iRadio *********

1. Wenn nicht bereits erfolgt, muss eine iRadio-Grundinstallation durchgeführt werden (install.sh im iRadio-Basisordner aufrufen), da die Audiodekodierung des DAB-Signals über vlc geschieht.

2. Mit dem Aufruf des Scripts install_dabservices.sh wird die grundliegende DAB/DAB+ Unterstützung compiliert und im iRadio-System installiert. 

3. Mit dem Aufruf des Scripts build.sh wird der DAB-Steuerdaemon dabd compiliert. 
Er ist das Bindeglied der Benutzeroberfläche (gpiod/displayd) des iRadio mit dem DAB-SDR-Backend.

4. Mit Aufruf von dabscan.sh wird ein Sendersuchlauf mit der installierten Empfangshardware angestoßen. 
Der Sendersuchlauf ersteckt sich von Kanal 5A bis 13F und kann einige Minuten dauern, es werden danach automatisch 
die Senderlisten (label.list, mux.list, url.list) für den DAB-Steuerdaemon angelegt. Bitte bearbeiten Sie diese
Dateien niemals händisch! 

Der Aufruf von dabscan.sh kann später durch einen gpiod getriggert werden oder durch einen cronjob zum Beispiel 1xtäglich 
erfolgen um die Senderlisten automatisch aktuell zu halten.

5. Um die DAB-Unterstützung bei jedem Start des iRadios einzuschalten, nehmen die den Steuerdaemon dabd in ein Startscript, zum Beispiel /etc/rc.local auf. Dies geht analog der dort bereits aufgeführten Daemonen (zum Beispiel gpiod, displayd). 

Der DAB-Steuerdaemon lauscht auf 127.0.0.1 Port 9914 UDP nach eingehenden Kommandos

Kommando - Bedeutung
 
dab0	 - schaltet den DAB-Empfang ab
dab1     - schaltet den DAB-Empfang ein
next	 - wechselt zum nächsten Programm in den Senderlisten
prev	 - wechselt zum vorherigen Programm in den Senderlisten

Konsolenbeispiel:

echo "dab1" | nc -u 127.0.0.1 9914 -w 0
echo "next" | nc -u 127.0.0.1 9914 -w 0
echo "prev" | nc -u 127.0.0.1 9914 -w 0


Anmerkung: Bei der Verwendung von preisgünstigen RTLSDR-USB-Empfängersticks ist ggf. das Modul dvb_usb_rtl28xxu zu blacklisten, d.h. in der Datei /etc/modprobe.d/blacklist ist der Eintrag "blacklist dvb_usb_rtl28xxu" vorzunehmen, danach reboot. Dies ist nur nötig, wenn auf den USB-Empfängerstick nicht zugeriffen werden kann und somit keine Stationen wärend des Sendersuchlaufs gefunden werden!

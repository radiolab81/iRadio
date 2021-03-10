# iRadio
Ein Softwarebaukasten für den Aufbau neuer Radios oder dem Umbau alter Radios zu einem Digitalradio (Internet und/oder DAB+) auf Raspberry Pi Basis.


#### ITT Viola 350 wird zum Digialradio...

![viola0](https://www.radio-bastler.de/forum/attachment.php?thumbnail=54713)
![viola1](https://www.radio-bastler.de/forum/attachment.php?thumbnail=54715)
![viola2](https://www.radio-bastler.de/forum/attachment.php?thumbnail=54563)

#### Ein Philips 634 "Digital" ...

![oldie2](https://www.radio-bastler.de/forum/attachment.php?thumbnail=55132)
![oldie3](https://www.radio-bastler.de/forum/attachment.php?thumbnail=55229)

## Unterstützte Rechner:

 Raspberry 1A(+), 1B(+), 2B, 3A+, 3B(+), 4 direkt mit Raspbian OS (https://downloads.raspberrypi.org/raspbian/images/). Das neue Raspberry Pi OS wird zur Zeit möglicherweise nicht vollständig out-of-box unterstützt. Das iRadio setzt eine funktionierende Audiokonfiguration auf dem Raspberry voraus!

***************************************************************************************************************************************
### Neu: iRL - iRadio on Linux, eine Linuxdistribution mit vorinstallierten und vorkonfiguriertem iRadio. Als Image für eine SD-Karte hier downloadbar: https://github.com/BM45/iRL

***************************************************************************************************************************************
  
## Steuerung des iRadio durch:

Programmumschaltung / Lautstärkeänderung mit (Micro-)Taster, Inkrementaldrehgeber, Drehimpulsgeber, fernsteuerbar über HTML-Bedienoberfläche http://IP_des_Radios:8080 oder 
über TCP/IP Sockets. 

Die Steuerung wird als Daemon/Prozess "gpiod" realisiert.

## Unterstützte Displays für Nutzerschnittstelle:

### direkt über Lowlevel-GPIO (eigener Displaydaemon "displayd"):

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
- eInk-Displays verschiedener Größen (1.5inch - 7.5inch) in Monochrom oder Tri-Color

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

#### ACHTUNG: fbtft (Linux Framebuffer drivers for small TFT LCD display modules) zur Zeit nur bis Kernelversion 5.3, siehe https://github.com/notro/fbtft/wiki , benötigen Sie diese Art der Displayansteuerung, nehmen Sie Rasbian Versionen vor 02/2020 als Betriebssystemgrundlage https://downloads.raspberrypi.org/raspbian/images/ oder Linuxdistributionen mit einem Kernel < V5.4.

## Zusatzfunktion:

Bei Raspberrys mit integrierter Bluetooth-Konnektivität, kann das iRadio als Bluetooth-Funklautsprecher arbeiten. Medieninhalte können über Bluetooth direkt zum iRadio gestreamt werden. Die Wiedergabe des Internetradioprogramms wird bei Verbindungsaufnahme über Bluetooth automatisch pausiert.

# Installation:

In /home/pi wird das Repository geklont.

`cd /home/pi/`

`git clone https://github.com/BM45/iRadio`

Wird das Repository als zip-Datei heruntergeladen, dann wird das heruntergeladene zip-Archiv mit `unzip` in /home/pi entpackt, so dass es in /home/pi/iRadio zu liegen kommt.

Die Basisinstallation des iRadios erfolgt durch Aufruf von install.sh mit Root-Rechten. Zur Basisinstallation wird eine Internetverbindung benötigt, da noch weitere Programmpakete installiert werden müssen.

`cd /home/pi/iRadio`

`sudo ./install.sh`

Nach dem Durchlauf des Installers und anschließendem Reboot des Raspberry, ist die Basisinstallation des Internetradios abgeschlossen. Zu diesem Zeitpunkt wird bereits die erste Internetradiostation der inkludierten Standardsenderliste abgespielt. Die Programmumschaltung kann durch Taster gegen Masse an den Pins 11 (GPIO17) und 12 (GPIO18) erfolgen.

Die Änderung der Pinbelegung oder Erweiterung der Tastensteuerung kann durch editieren von /home/pi/iRadio/gpiod.c erfolgen. Die Änderungen werden durch folgende Befehle übernommen.

`cd /home/pi/iRadio`

`sudo ./install_Tastensteuerung.sh`

`sudo reboot`

Anstelle von Tastern, kann auch ein Drehimpulsgeber zur Programmumschaltung genutzt werden.

![rotary](https://www.radio-bastler.de/forum/attachment.php?thumbnail=56549)

Hierzu muss ein neuer Steuerdaemon (gpiod) gebaut werden. Dies geht automatisch durch folgende Befehle.


`cd /home/pi/iRadio`

`sudo ./install_Drehencoder.sh`

`sudo reboot`

Nach dem Reboot des Raspberry kann man die Internetradioprogramme durch einen Drehimpulsgeber an den Pins 11 (GPIO17) und 12 (GPIO18) umschalten.

Eine Änderung der Pinbelegung oder Erweiterung der Drehimpulsgebersteuerung (zum Beispiel für die Verstellung der Lautstärke über einen zweiten Drehimpulsgeber) kann durch editieren von /home/pi/iRadio/rotary.c erreicht werden. Die Änderungen werden durch folgende Befehle übernommen.


`cd /home/pi/iRadio`

`sudo ./install_Drehencoder.sh`

`sudo reboot`


An das iRadio kann man verschiedene Displays anschließen (siehe oben). Diese dienen u.a. zur Darstellung des Sendernamens
der aktuell eingestellten Radiostation oder des aktuell gespielten Titels. 


![lcd](https://www.radio-bastler.de/forum/attachment.php?thumbnail=56759)


Die Ansteuerung des Displays übernimmt der Prozess displayd im iRadio. 
Um einen solchen displayd für ein LCD-Modul zu bauen, geben wir folgende Befehle ein.

`cd /home/pi/iRadio`

`sudo ./install_I2CDisplay20x4.sh`

`sudo reboot`

Ist nun ein LCD-Modul (mit Adresse 0x27) am I2C-Bus des Raspberry angeschlossen, sollte sich nach dem Neustart oben gezeigtes Szenario darstellen.

Sollte Ihr LCD-Modul eine andere I2C-Adresse besitzen oder ein anderes Format haben, so können Sie Anpassungen in 
der Datei /home/pi/iRadio/display/lcd/displayd.cpp vornehmen. 

Nach einem erneuten Aufruf von `sudo ./install_I2CDisplay20x4.sh` und einem Neustart des Raspberry sind die Änderungen aktiv.


Weitere Displaytypen werden im iRadio direkt unterstützt, für Sie gibt es bereits unterschiedliche Installer in /home/pi/iRadio . 

Hier ein SSD1306-OLED:

![ssd1306](https://www.radio-bastler.de/forum/attachment.php?thumbnail=56780)

oder ein ST7335-TFT: 

![st7335](https://www.radio-bastler.de/forum/attachment.php?thumbnail=56840)

Die passenden Installer erkennt man leicht am entsprechenden Namen. Der passende Code für diese Displaytypen bzw. für den Prozess displayd liegt in `/home/pi/iRadio/display` . Hier darf und soll(!) der Nutzer ausdrücklich seine eigenen Anpassungen vornehmen, um so zu einer individuellen Lösung zu kommen! Das iRadio soll hier nur grob den Rahmen für eigenen Konstruktionen skizzieren.

Ein ganz besonderer "Displaytyp" ist der PWM-Servo. Der Code dafür ist in `/home/pi/iRadio/display/servo` zu finden. Installiert wird er mit:

`cd /home/pi/iRadio`

`sudo ./install_ServoD.sh`

`sudo reboot`

Das iRadio mit einem PWM-Servo ist in der Lage, bestimmte Zustände, zum Beispiel Programmspeicherplätze oder Lautstärkeeinstellungen mit einem richtigen Zeiger oder einer richtigen Skale darzustellen. 

Ein von mir sehr geschätzter Radiobauer aus dem Radio-Bastler-Forum, hat mit dem iRadio einen Volksempfänger zum Internetradio umgebaut. Die alte Frequenzskale, nun angetrieben von einem Servo, zeigt den aktuell eingestellten 
Programmplatz.

![servo1](https://www.radio-bastler.de/forum/attachment.php?thumbnail=60455)
![servo2](https://www.radio-bastler.de/forum/attachment.php?thumbnail=61100)
![servo3](https://www.radio-bastler.de/forum/attachment.php?thumbnail=61097)

In Aktion: (Klick führt zu Youtube)

[![servovideo](http://img.youtube.com/vi/fL3GbyHzpOE/0.jpg)](http://www.youtube.com/watch?v=fL3GbyHzpOE "")


Mit der "Highlevel-Unterstützung" durch X11/Framebuffer sind natürlich auch andere Benutzerschnittstellen mit dem iRadio möglich, so zum Beispiel eine touch-sensitive Bedienung über eine FLTK-GUI.

![fltk](https://www.radio-bastler.de/forum/attachment.php?thumbnail=55538)

Ein besonderes Highlight ist die Simulation photorealistischer Nachbildungen von Senderskalen alter Radios.

![sim1](https://www.radio-bastler.de/forum/attachment.php?thumbnail=58390)
![sim2](https://www.radio-bastler.de/forum/attachment.php?thumbnail=58393)
![sim3](https://www.radio-bastler.de/forum/attachment.php?thumbnail=58950)

Umbau eines alten Metz-Baby zum iRadio-Internetradio.

[![](http://img.youtube.com/vi/cSQKa7eCEfE/0.jpg)](http://www.youtube.com/watch?v=cSQKa7eCEfE "")

Über spezielle "Stretch-HDMI/LVDS-Panels" lassen sich auch richtig große Dampfradios zu modernen Internetradios umbauen! Eigene Skalen lassen sich als PNG-Datei mit Transparenz erstellen. Der Code für die Skalensimulationen liegt in /home/pi/iRadio/display/x11 . Auch hier kann und soll ausdrücklich jeder Nutzer seine eigene Kreativität ausleben! Ziel ist was Spaß macht und das iRadio gibt nur den Grundrahmen vor.


## Zusatzfunktion:

Wie oben geschrieben kann man mit dem iRadio nicht nur Internetradios aufbauen. Beim Einsatz von Raspberry Pi's mit Bluetooth-Schnittstelle ist auch die Möglichkeit gegeben, das iRadio als Funklautsprecher zu nutzen. 

Zur Installation der Bluetoothfunktionalität gibt man folgendes ein:

`cd /home/pi/iRadio/bt-speaker`

`sudo ./install.sh`

`sudo reboot`

Nach dem Neustart des Raspberry präsentiert sich der Kleinrechner mit seinem vergebenen Hostnamen in der Bluetooth-Suche 
von Mediengeräten.

![bluetooth](https://www.radio-bastler.de/forum/attachment.php?thumbnail=59874)

Koppelt man sich mit dem iRadio, dann wird die Wiedergabe des Internetradios automatisch unterbrochen und das iRadio wird zum Funklautsprecher. Entkoppelt man die Bluetoothverbindung, so beginnt das iRadio mit der Wiedergabe der zuletzt eingestellten Internetradiostation.


## Änderung der WiFi-Zugangsdaten und der Senderliste:

WiFi: Es kann eine neue wpa_supplicant.conf oder wlan.txt (mit gleichem Inhalt wie wpa_supplicant.conf)
in /boot oder im Rootverzeichnis eines mit FAT32-formatierten USB-Sticks abgelegt werden. Nach einem Neustart werden die WiFi-Zugangsdaten automatisch aktualisiert.

Neue Senderliste:

Es wird eine Datei playlist.m3u in /boot oder im Rootverzeichnis eines mit FAT32-formatierten USB-Sticks abgelegt. 
Nach einem Neustart wird die Senderliste des Internetradios automatisch aktualisiert.
Achtung: In der playlist.m3u darf pro Zeile nur die URL einer Internetradiostation stehen. Keine M3U-Metadaten verwenden!
Bei der Verwendung einer Skalensimulation oder Servo-PWM sind Internetradiostationen mit URL-Umleitung zu vermeiden und durch fixe URLs zu ersetzen. 


## Unterstützung des UKW-Rebroadcast einer Internetradiostation mit dem iRadio:
Mit iRadio-Version vom 20.05.2019, wird das FM-Transmittermodul MMR-70 zur Aussendung des empfangenen Internetradioprogramms oder Bluetoothstreams unterstützt. Vielen Dank an Tobias Mädel (https://github.com/Manawyrm) für die Bereitstellung des passenden Treibers. 

Wie richtet man eine "Internetradio-UKW-Relaisstation" mit dem iRadio ein?

Nach der Grundinstallation des iRadios und des Anschlusses des Sendemoduls (Achtung: es sind ggf. GPIO-Ressourcenkonflikte zwischen Drehencoder/Tastensteuerung und MMR70 aufzulösen!), wechselt man in das Verzeichnis /home/pi/iRadio/Transmitter/MMR70

`cd /home/pi/iRadio/Transmitter/MMR70`

Die Treiberunterstützung wird mit 

`sudo apt-get install libconfuse-dev`

`make`

`sudo make install` 

gebaut und installiert. Danach muss der iRadio-interne VLCD-Daemon noch aktualisiert werden. Man kopiert die Datei vlcd
aus dem MMR70-Verzeichnis nach /usr/bin/ . 

`sudo cp ./vlcd /usr/bin/`

Ist das erfolgt, startet das iRadio beim nächsten Neustart mit der Unterstützung für den MMR70-Sender. Die Konfiguration des Senders (Frequenz, Sendeleistung und weitere Parameter) kann über eine Datei mit dem Namen FM.txt erfolgen. Eine Beispieldatei befindet sich im MMR70-Verzeichnis des iRadio. Diese Datei muss sich beim Neustart des iRadios entweder in /boot auf der SD-Karte oder im Wurzelverzeichnes eines FAT32-formatierten USB-Sticks befinden. 


## Update vom 25.07.2019: Unterstützung des UKW-Rebroadcast einer Internetradiostation mit dem Raspberry als Sendehardware:
Mit dem Update vom 25.07.19. wird zur Aussendung im UKW-Bereich kein MMR70-Sender mehr benötigt (dieser wird aber weiter unterstützt!). Der Raspberry erzeugt das UKW-Signal direkt über seine GPIO-Hardware an GPIO4 (Pin 7). An diesen Pin muss ein Reko-/Bandpassfilter geschaltet werden, damit die ebenfalls vorhandenen Oberwellen keine anderen Funkdienste beeinträchtigen! Ebenso muss beim Anschluß einer Antenne auf die im Betriebsland maximal zulässige Sendeleistung geachtet werden. Bei voller Leistung sind bis zu 75 mW(!) an Sendeleistung zu erwarten. 

### Installation:

1. iRadio Grundinstallation ausführen
2. Rekonstruktions/Ausgangsfilter an GPIO4 (Pin 7) anschließen!
3. Sendedaemon installieren

       cd /home/pi/iRadio/Transmitter/PiFmAdv/
       sudo ./install.sh

4. Auf einen FAT32-formatierten USB-Stick die Konfigurationsdatei FM.txt kopieren und
   ggf. anpassen.

Neustart des iRadios mit gestecktem USB-Stick! Das iRadio aktualisiert sich 
automatisch und der Sender wird entsprechend der in FM.txt angegebenen Konfiguration
arbeiten.

## Update vom 04.09.2019: Unterstützung mehrerer e-Paper / e-Ink Displays
Mit der heutigen Version wird die Unterstützung mehrer e-Ink Displays in das iRadio aufgenommen. 

![eink1](https://www.radio-bastler.de/forum/attachment.php?thumbnail=66672)
![eink1](https://www.radio-bastler.de/forum/attachment.php?thumbnail=66673)

## Update vom 30.04.2020: Simulation eines Abstimmgeräusches
Mit dem Update vom 30.04.2020 ist es möglich, zwischen dem Umschalten zweier Internetradiosender ein Abstimmgeräusch einzuspielen. Damit kann das Verhalten eines echten Radios noch besser simuliert werden. Um das zu ermöglichen, wurde ein neuer Daemon/Prozess im iRadio eingeführt, der noised .
Der Soucecode für diesen Prozess liegt im Verzeichnis "noise_generator" in der Datei noised.cxx . Über das Buildscript build.sh im gleichen Ordner kann der Daemon gebaut werden. Der weitere Installationsweg des noised steht in der install.txt .

Es werden zwei vorbereitete Abstimmgeräusche in den Dateien noise.mp3 und tuning.wav mitgeliefert. Eigene Abstimmgeräusche sind natürlich ebenso möglich, siehe install.txt

Die Verwendung des noised ist nicht(!) an eine Skalensimulation gebunden, eignet sich für diese aber besonders gut, siehe nachfolgendes Demovideo:

(Klick führt zu Youtube)

[![](https://i9.ytimg.com/vi/XSdv0t-ksyM/mq1.jpg?sqp=CIDrrPUF&rs=AOn4CLCSTFvaBg3scu_x6ZkzKT1ORTavNg)](https://youtu.be/XSdv0t-ksyM "")

## Update vom 26.07.2020: Weitere SDL2-Skalensimulationen als Beispielcode aufgenommen

Hier wird ein alter Kassettenrecoder zu einem Internetradio. In das Kassettenfach wurde ein Display eingebaut, auf dem diesmal eine besondere Skalensimulation läuft, nämlich eine in Form einer alten Musikkassette.

![cass1](https://github.com/BM45/iRadio/blob/master/pics4www/Cassettensimulation.jpg)

Während des Internetradiobetriebs drehen sich die animierten Cassettenspulen und der Sendername und Interpret werden in das Namensfeld der Kassette eingetragen.

![cass2](https://github.com/BM45/iRadio/blob/master/pics4www/Cassettensimulation2.jpg)

Danke Hans für diese tolle Zuarbeit zum iRadio. Sein Projekt zu diesem Radio kann man hier nachlesen: https://www.radio-bastler.de/forum/showthread.php?tid=15127

Als weitere Skalensimulation haben wir eine Skale eines Lorenz C2 aufgebaut!

![lorenz1](https://github.com/BM45/iRadio/blob/master/pics4www/lorenzc2_3.jpg)

Im Internetradiobetrieb wird beim Umschalten ganz normal die Frequenznadel entsprechend der eingelesenen Senderliste bewegt, zusätzlich wird der Stationsname eingeblendet.

![lorenz2](https://github.com/BM45/iRadio/blob/master/pics4www/lorenzc2_1.jpg)

Wird das Radio aber in den Standbymodus versetzt, so zeigt die Frequenznadel die aktuelle Uhrzeit an. Zusätzlich lässt sich eine Weckzeit programmieren (roter Zeiger) an dem das Radio automatisch in den Internetradiobetrieb geht! Natürlich lassen sich auch andere Nachtdesigns implementieren, denn wie immer ist das iRadio nur eine Ideensammlung und ein Rahmen für Euer eigenes Internetradio. 

![lorenz3](https://github.com/BM45/iRadio/blob/master/pics4www/lorenzc2_2.jpg)

## Update vom 31.08.2020: Bedienung des iRadios mittels Gestenerkennung mit dem PAJ7620U2-Sensor

![geste1](https://github.com/BM45/iRadio/blob/master/pics4www/Gestenerkennung.jpeg)

Neben der iRadio-Bedienung über Tasten/Drehencoder (gpiod) ist nun auch die berührungslose Bedienung mittels Gestensensor möglich!
Mit dem PAJ7620U2-Sensor versteht das iRadio 9 verschiedene Gesten und kann darauf mit bestimmten Aktionen (Programmumschaltung, Lautstärkeänderung, An/Aus, ...) reagieren. Mit dem PAJ7620U2 werden folgende Gesten erkannt:

- Hand nach links
- Hand nach rechts
- Hand nach oben
- Hand nach unten 
- Hand auf Sensor zubewegen
- Hand von Sensor wegbewegen
- Hand vor Sensor im Uhrzeigersinn
- Hand vor Sensor gegen Uhrzeigersinn
- Wellenförmige Bewegung vor dem Sensor

Die Erkennungsreichweite des Sensors beträgt zwischen 5 und 20 cm, hinter Skalenglas oder abgedunkelter Rauchglasskale wird die Reichweite etwas gedämpft. Die Erkennung erfolgt mittels IR-Licht und einem Sensorarray (ähnlich einer kleinen Kamera). Der Erfassungsbereich liegt bei 60°, die erkannte Geste wird per I2C-Bus vom Sensor ausgelesen. 

### Anschluß des PAJ7620U2.

Der PAJ7620U2 wird wie alle I2C-Komponenten des Radios an den I2C-Bus (SDA, SCL) des Raspberry gehangen. Vcc = 3.3 V und GND, die Internruptleitung kann freibleiben, Sie wird im Democode nicht benötigt.

Der Sensor kann neben der herkömmlichen Bedienung über Tasten, Drehencoder oder Touchscreen benutzt werden, die Funktionalität ist in einem eigenen Daemon (gestured) implementiert. Der Quellcode liegt in iRadio/Gesture/PAJ7620U2 und dort in der Datei PAJ7620U2.c . In der Switch-Anweisung können nach dem Vorbild der dortigen Beispielaktionen auch eigene Reaktionen codiert werden. Gebaut und im Radiosystem installiert wird die Gestenerkennung durch das Installationsscript install_gestenerkennung.sh im iRadio-Projektverzeichnis. Bitte den Ausgaben des Installationsscriptes folgen!

## Update vom 09.09.2020: Das bisherige Projekt "WorldRadio" wurde ins iRadio integriert.

![rtlsdr](https://github.com/BM45/iRadio/blob/master/pics4www/iRadio_rtlsdr.jpg)

Mit diesem Funktionsupdate wollen wir unserem Internetradio neue Empfangswege beibringen. Ein mit iRadio umgebautes/neu aufgebautes Internetradio kann ab sofort auch ohne Internetanschluß Rundfunk empfangen. Zunächst können RTLSDR USB-Sticks aus dem Osmocom rtl-sdr Projekt (https://osmocom.org/projects/rtl-sdr/wiki/Rtl-sdr) genutzt werden, um zum Beispiel UKW-Rundfunk, Flugfunk, (SSB-) Amateurfunk und vieles mehr zu empfangen. Der experimentelle Beispielcode für eine FLTK-Benutzeroberfläche, die Internetradio und UKW-Radio/AM-Flugfunk unter einen Hut bringt, liegt in iRadio/Tuner/rtlsdr. Dieser Beispielcode kann und soll wie beim iRadio üblich, eine Vorlage, Schablone oder Denkanstoß für eigene Umsetzungen sein. Die Installation der rtlsdr-Unterstützung kann über das Installerscript install.sh in iRadio/Tuner/rtlsdr/ erfolgen. 

Die FLTK-GUI wird mit dem Script build_gui.sh compiliert. Die übersetzte GUI ist danach wie eine Skalensimuation in den Systemstart einzubinden. Natürlich ist man nicht auf eine Bedienung per Touchscreen beschränkt! Es lassen sich auch Bedienkonzepte mit allen mitgelieferten Displayarten und gpiod-Steuerdaemonen realisieren.

## Update vom 17.09.2020: Unterstützung für UKW-Radio-IC RDA5807 integriert.

Mit diesem Funktionsupdate wird es möglich neben Internetradio auch UKW-Radio über den IC RDA5807 anzubieten. Der RDA5807 ist ein ungefähr 3x3mm großer Chip in dem ein komplettes Software-Defined-Radio (SDR) für 50-108 MHz steckt. Er kann somit das europäische/amerikanische UKW-Band genauso abdecken wie das japanische oder russische Frequenzband. 

![RDA5807a](https://github.com/BM45/iRadio/blob/master/pics4www/RDA5807_System.JPG)

Zusätzlich zu diesem breiten Frequenzband ist auch die Möglichkeit des RDS-Empfang gegeben. Die komplette MPX-Dekodierung wird rein in Software auf dem im Chip integrierten Basisband-Signalprozessor erledigt.

Den RDA5807 gibt es im deutschen Handel in der Regel in Form eines Breakoutboards, die Pinbelegung ist nachfolgend zu sehen:

![RDA5807b](https://github.com/BM45/iRadio/blob/master/pics4www/Pinout.jpg)

Im iRadio gibt es zwei Möglichkeiten der Installation der RDA5807-Unterstützung.

1. Über den Installer build_rda5807_support_only.sh wird allein der Steuerdaemon für den RDA5807 compiliert und nach /usr/bin kopiert.

Die Steuerung des RDA5807-Chips erfolgt dann direkt über diesen Daemon:

![RDA5807c](https://github.com/BM45/iRadio/blob/master/pics4www/rda5807daemon.jpg)

2. Über das Installscript build_rda5807_st7735demo.sh wird neben dem Steuerdaemon für den RDA5807 auch eine Demoskale (displayd) für ST7735-Displays und ein passender gpiod für einen Rotaryencoder compiliert und installiert.

![RDA5807d](https://github.com/BM45/iRadio/blob/master/pics4www/RDA5807_Menues.JPG)

In dieser Demonstration ist der Betrieb von Internetradio und UKW-Radio über einen(!) Drehregler mit Drucktaster möglich. Die Umschaltung des Empfangsmodus erfolgt durch kurzen Druck, Wechsel von und in den Standbybetrieb ist durch einen langen Druck auf den Taster möglich. Der Quellcode für RDA5807-Steuerdaemon, sowie Demo-displayd und gpiod liegt im Ordner Tuner/RDA5807 des iRadio-Basisordners. Dieser Beispielcode kann und soll wie beim iRadio üblich, eine Vorlage, Schablone oder Denkanstoß für eigene Umsetzungen sein und natürlich sind die dort gezeigten Ansteuerungswege auch auf andere Displays und Bedienkonzepte übertragbar.

## Update vom 09.12.2020: Update der bcm2835- und wiringpi- Bibliotheken auf Version 1.68 bzw. 2.52 zur Verbesserung der lowlevel-Unterstützung des Raspberry 4 

## Update vom 13.01.2021: Simulation von Anzeigeröhren (magische Augen, Bänder)

### Demoanwendung - Internetradio mit Skalensimulation und Anzeigenröhre über zwei HDMI-Displays
![magicEye1](https://github.com/BM45/iRadio/blob/master/pics4www/simmagiceyes.jpg)

Auf Basis von SDL2/X11 können nun Anzeigeröhren simuliert werden. Im Ordner "magicEye" gibt es dazu zwei generische Simulationen vom Typ EM34 und EM84, die zur Anzeige der WLAN-Signalstärke dienen. Der eigentliche Simulationscode der Anzeigenröhre ist in der Datei tube.cxx enthalten, die Ermittlung der WLAN-Signalstärke wird in der Datei signal.cxx umgesetzt. Die PNG-Dateien enthalten Fotografien richtiger Röhren und dienen als Ebenen der fotorealistischen Darstellung der Simulation. Durch Codeänderungen können durch die Anzeigenröhren auch andere numerische Werte repräsentiert werden, das iRadio soll hier wie immer nur einen Rahmen vorgeben und zu eigenen Umsetzungen anregen! Die Simulation der Anzeigenröhre auf Basis von SDL2 orientieren sich stark an der Skalensimulation, entsprechend ähnlich ist die Installation vorzunehmen. Zunächst wird durch das build-Script build.sh die Simulation der Anzeigenröhre gebaut, wodurch ein neuer Daemon namens magicEyed entsteht. Für den Autostart ist dieser Deamon wie bei den Skalensimulationen in /etc/rc.local aufzunehmen. Das darstellende Display für diese Simulationen kann ein HDMI oder GPIO-Display sein. Mischbetrieb ist zulässig, ebenso Dual-Displaybetrieb zur Anzeige von Senderinfo/Skalensimulation und(!) Simulation der Anzeigenröhre und beschränkt sich nicht nur auf die beiden HDMI-Ausgänge eines Raspberry PI 4. Wie bei fotorealistischen Simulationen im iRadio üblich, sollten wenn immer möglich beschleunigte Grafiktreiber eingesetzt werden.

## Update vom 02.02.2021: Das iRadio erhält vollumfängliche DAB+ Unterstützung (RPi3/4+)

Mit dem Funktionsupdate vom 02.02.2021 wird das iRadio zu einem modularen Softwarebaukasten für den Neuaufbau von Digitalradios (früher nur Internetradio) oder zum Umbau alter Radios in ein Internet- und/oder DAB+ Radio. 
![dab1](https://github.com/BM45/iRadio/blob/master/pics4www/iRadioDAB.jpg)

Als Empfänger für DAB+ kann beim iRadio eine Vielzahl verschiedener Hardware dienen. Angefangen vom einfachen RTLSDR-USB-Stick für 10 Euro, über Mittelklasse-SDRs wie dem AIRSpy (R2/mini) oder HackRF One, bis zu hochwertige RFspace-SDRs. Die Empfangshardware kann dabei zentral am Raspberry, also im Radiogehäuse untergebracht sein, als auch über eine TCP/IP Verbindung dezentral angeschlossen werden. Ein solcher dezentraler SDR-Empfänger kann an empfangsgünstigen Orten (Dachboden, Radioshack, Tower) untergebracht sein und mehrere iRadio-Instanzen mit DAB+ Empfang versorgen.

![dab1](https://github.com/BM45/iRadio/blob/master/pics4www/iRadioDABHardware.jpg)

Zur Installation der DAB+ Unterstützung gibt man folgendes ein:

`cd /home/pi/iRadio/Tuner/DABplus`

`sudo ./install_dabservices.sh`

`./build.sh`

Durch das Installerscript werden noch benötigte Softwarekomponenten nachinstalliert und die DAB+ Basisunterstützung compiliert und im System installiert.
Detailierte Installationsinformationen sind in der Datei README.txt im DABplus-Ordner zu finden!
Nachdem die SDR-Empfängerhardware mit dem Raspberry verbunden wurde, kann mit dem Script  

`./dabscan.sh` 

ein Sendersuchlauf über den DAB+ Frequenzbereich VHF-BAND 3 (174 - 230 MHz, bzw. Kanal 5A-13F) durchgeführt werden. Dieser Suchlauf kann einige Minuten in Anspruch nehmen, hierbei werden auch alle Senderlisten für den DAB-Steuerdaemon dabd generiert. Die gefunde Anzahl an Sendern wird auf der Konsole ausgegeben. 
Sind die Senderlisten erstellt, kann der DAB-Steuerdaemon dabd mittels

`./dabd &`  

gestartet werden. 
Der DAB-Steuerdaemon hört auf 127.0.0.1 den Port 9914/UDP nach Kommandos von der Benutzeroberfläche (gpiod/displayd) ab. Folgende Kommandos sind zur Zeit implementiert:

#### Kommando - Bedeutung
 
dab0	 - schaltet den DAB-Empfang ab

dab1  - schaltet den DAB-Empfang ein

next	 - wechselt zum nächsten Programm in den Senderlisten

prev	 - wechselt zum vorherigen Programm in den Senderlisten


Da zur Audiowiedergabe von DAB+ Stationen, dass bereits im Internetradiomodus bekannte vlc genutzt wird, können Rückinformationen wie Programmtitel weiterhin über UDP/9294 empfangen werden. Somit sind die bereits vorhandenen Displaydaemonen (displayd) schnell und meist ohne komplexe Codeänderungen bereit für den DAB+ Empfang. Nachfolgend ein Bild einer im iRadio als Demo vorhanden Skalensimulation beim DAB+ Empfang.

![dab1](https://github.com/BM45/iRadio/blob/master/pics4www/iRadioDABSkalensim.jpg)

Da sowohl Skalensimulation, wie auch SDR/DAB+ Empfang rechenintensive Prozesse sind, sollten solche Neu-/Umbauen modernen Raspberry Pi 4 (oder wenigstens späteren 3er Modellen) vorbehalten sein um gute Systemantwortzeiten zu erreichen, wobei 1/2 GB RAM-Systeme vollkommend ausreichend sind.

Ebenso ist es möglich, wie auch schon beim Internetradio, die empfangene DAB+ Station direkt über UKW (via Sony MMR70, GPIO-TX) wieder auszusenden. 

## Update vom 08.02.2021: X11-Benutzeroberfläche für moderne Internet-/DAB+ -Radios hinzugefügt

![dab2](https://github.com/BM45/iRadio/blob/master/pics4www/DABGui.jpg)

Mit dem Update vom 08.02.2021 wird Democode (displayd und gpiod) für eine Benutzeroberfläche für moderne Multinormradios (Internet/DAB+) hinzugefügt. Der Code befindet sich in display/x11/DABgui, die beiden nötigen Daemonen werden über das Buildscript compiliert. Die Installation der GUI erfolgt dann analog einer fotorealistischen Skalensimulation. Weitere Infos dazu findet man in der im DABgui-Ordner befindlichen README.txt . Der SDL2-Democode kann und soll beliebig für eigene Radios und Displays angepasst werden, wie immer soll das iRadio hier nur ein Rahmenwerk für eigene Umsetzungen sein. Der gpiod nutzt hier einen Drehencoder/Taster zur Ansteuerung des displayd. Für reine Touchbedienung stellt die SDL2-Bibliothek auch eine entsprechende Eventbehandlung bereit, auch das Hinzufügen einer dritten Empfangsart wie UKW ist durch die bereits im iRadio integrierte Unterstützung (zum Beispiel aus der Integration des WorldRadio-Projekts vom 09.09.2020) einfach möglich.


## Update vom 13.02.2021: iRadio als DAB(+) Sender, Wiederaussenden einer Internetradiostation im DAB(+) Format 

![dab3](https://github.com/BM45/iRadio/blob/master/pics4www/iRadioDABTx.jpg)

Mit dem Funktionsupdate vom 13.02.2021 ist es möglich, eine aus dem Internet empfangenen Radiostation über einen eigenen DAB oder DAB+ Sender áuszusenden. Von dieser Funktion profitieren vor allem alte Digitalradios, die nur über DAB verfügen und somit praktisch wertlos sind. Mit dem iRadio können diese Radios nun ohne Eingriff zu einem Internetradio umgerüstet werden. Zusätzlich ist es möglich mit dem iRadio das bereits bestehende DAB+ Senderangebot um Internetradiostationen zu erweitern.

Zur Generierung eines DAB(+)-konformen Signals, werden im iRadio drei Prozesse benötigt: ein Audioencoder, ein Multiplexer und ein Modulator. 

![dab4](https://github.com/BM45/iRadio/blob/master/pics4www/iRadioDABTx2.jpg)

Diese drei Prozesse findet man im Ordner /Transmitter/ODR des iRadios. Sie müssen nacheinander aus den Quellen compiliert werden. Informationen zum Compilieren dieser Programme findet man in den Dateien README.md und INSTALL.md in den jeweiligen Unterordnern. In der Datei mmbtools.pdf wird der Aufbau der DAB(+) Sendekette und das Zusammenspiel aller drei Prozesse detailiert beschrieben. Eine Muster-vlcd kann entsprechend der gewählten Interprozesskommunikation aufgebaut  und nach /usr/bin kopiert werden. Dadurch kann das iRadio fallweise (mittels DAB.txt Datei auf USB-Stick) in den DAB(+) Sendemodus versetzt werden, analog der bereits bestehenden UKW-Sendefunktion.

Zusätzlich zur Integration der DAB(+) Sendefunktion, bringt das iRadio nun Unterstützung für weitere SDR-Sendehardware mit. Es wurde Treiberunterstützung für den Adalm Pluto (https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/adalm-pluto.html) und für den preiswerten FL2K SDR-Sender (https://osmocom.org/projects/osmo-fl2k/wiki) integriert.

## Update vom 10.03.2021: automatische Mediaplayerfunktion hinzugefügt 

![dab3](https://github.com/BM45/iRadio/blob/master/pics4www/mediaplayer.jpg)

Wird ein mit Mediendateien gefüllter USB-Stick in das iRadio eingesteckt, so wird dieser automatisch erkannt, die Internetradio- oder Internetfernsehwiedergabe gestopt und die Wiedergabe der Musik-/Filmsammlung von USB gestartet. Zieht man den USB-Stick wieder ab, geht das iRadio danach automatisch wieder in den Internetradio- bzw. Internetfernsehbetrieb über.

Um diese automatische Mediaplayerfunktion zu aktivieren, ruft man das Buildscript im Ordner mediaplayerd auf. Der Mediaplayer-Daemon wird compiliert und alle nötigen Dateien werden an die richtige Stelle im iRadio-System kopiert. Zuletzt muss nur noch der Mediaplayer-Daemon "mediaplayerd" in einer Startdatei (zum Beispiel /etc/rc.local) eingetragen werden. Nach einem Neustart des iRadios ist die Funktion aktiviert.

Standardmäßig wird auf dem Root-Verzeichnis des USB-Sticks nach Mediendateien mit den Endungen mp3, aac, wav und mp4 gesucht. Weitere Dateitypen können jederzeit in der Datei mkplaylist.sh unter dem Punkt

FILES=(*.mp3 *.mp4 *.aac *.wav)

hinzugefügt werden. Nach einem Neuaufruf des Buildscriptes und einem Neustart wird dann auch nach den neu hinzugefügten Dateiendungen gesucht wenn ein USB-Stick eingesteckt wird. Standardmäßig gibt der mediaplayerd die Mediendateien in der Reihe des Auffindens wieder. Möchte man eine Zufallswiedergabe, so kann man dies in der Datei mpvlcd durch Hinzufügen der --random Aufrufoption zu vlc bewirken. 

Natürlich lässt sich der Mediaplayer durch Anpassung der Quellcodedateien beliebig in der Funktion anpassen und in aufwenigere Benutzerkonpzepte, zum Beispiel bei Skalensimulation mit Umschaltung zwischen mehreren Empfangsarten (Internetradio/DAB/UKW), leicht integrieren.

_____________________________________________________________________________________
Weiterer Support im Radio-Bastler-Forum unter: https://www.radio-bastler.de 
Bitte beachtet auch den Blog von meinem Bastlerkollegen Franz-Josef Haffner: https://radiobasteleien.blogspot.com/search/label/iRadio
Der Franz zeigt auf seinen Seiten unzählige Umbauten und Modernisierungen alter Radios, nicht nur mit dem iRadio! Er hat für 
das iRadio auch eine sehr umfangreiche FAQ angelegt!
Seine Werke sieht man hier: https://radiobasteleien.blogspot.com/search/label/Internetradio

Bei Fragen meldet Euch einfach im Radio-Bastler-Forum an.

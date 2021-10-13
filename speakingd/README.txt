1. Installation der Sprachausgabe durch Aufruf des Installerscripts install.sh mit Administratorrechten.

2. Eintragen von /usr/bin/speakingd.sh & in die /etc/rc.local falls die Sprachausgabe bei jedem Neustart aktiv sein soll.

3. Neustart des iRadios.

Nach dem Neustart kann eine Sprachausgabe wie folgt generiert werden:

pico2wave --lang=de-DE -w /tmp/pico.wav "Hier den Text eingeben der ausgesprochen werden soll." | aplay

Ueber den Parameter --lang= koennen verschiedene Sprachen und Dialekte ausgewaehlt werden.

de-DE > Deutsch
en-GB > britisches Englisch
en-US > amerikanisches Englisch
es-ES > Spanisch
fr-FR > Franzoesisch
it-IT > Italienisch

...


3. Es gibt 2 Demoanwendungen mit Beispielcode.

- gpiod > Ein sprechender Drehencoder fuer die Programmumschaltung, Installation mit sudo ./install_Speaking_Rotary.sh .
  Code in rotary.c .

- displayd > Ein sprechender Displaydaemon, der die aktuelle vom Radiosender uebertragene Kennung vorliest. Installation mit sudo ./install_Speaking_Displayd.sh .
  Code in displayd.cpp .






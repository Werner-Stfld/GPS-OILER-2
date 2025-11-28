# GPS-Oiler 3.0 #

Zur Version 3

## WEB-Server ##

Der Webinhalt nutzt vueJs wie hier beschrieben. [How to use VueJs with ESP8266 or ESP32](https://www.luisllamas.es/en/esp8266-vuejs/)
Die Dateien des Web-Servers sind als "LittleFs" Dateisystem auf dem Target hinterlegt.

Das Dateisystem muss erstellt und anschießend auf das Gerät geladen werden. Der Inhalt des Dateisystems wird aus dem data Verzeichnis genommen.

### Erzeugen und Hochladen des Dateisystems mit platform.io ###

Wichtig: platformio.ini benötigt einen Eintrag in platformio.ini damit das korrekte Dateisystem-Format erstellt wird.

```
board_build.filesystem = littlefs
```

Ich habe die Projekt-Task von platform.io verwendet: 
"Upload Filesystem Image".

## WEB-Oberfläche ##

- Die WEB-Oberfläche ist in dem statischen Dateisystem des Geräts hinterlegt. Die aktuellen Einstellungen werden per REST-API ausgetauscht.
- Die Größe der Oberfläche ist an das Endgerät angepasst. 
- Alle Einstellungen werden über eine Schaltfläche in das Gerät übertragen. 
- Die Aktualisierung erfolgt ebenfalls über eine Schaltfläche.

## Anzeige ##

-  Der erste Bildschirm enthält den QR-Code mit der URL der WEB-Seite.
- Die Anzeige des Sattelitenempfangs wird mit einer grafischen Intensitätsanzeige gemacht. Grundlage ist die Anzahl empfagener Satteliten. 
-  Den Abstand zu der nächsten Ölung wird über einen Balken dargestellt. Der Balken wird während der Ölanzeige mit der Ölkanne überblendet.
-  Die gefahrenen Kilometer seit Rücksetzen des Tanks wurde ersetzt durch die Zeitanzeige.

## Öler ##

Als erfahrener Anwender eines Ölers bin ich der Meinung, dass ein Ölen aufgrund des Neustarts nicht notwendig ist. Es sollte jedoch sichergestellt werden Ölen nach einem Regen nicht zu "vergessen". Moped im Regen abgestellt. Bei Weiterfahrt mit schönem Wetter sollte die Kette nachgeölt werden.

## [Beschreibung](docs/Beschreibung/README.md)

## [Nachbau](docs/Nachbau/README.md)

## [Vorkenntnisse](docs/Vorkenntnisse/README.md)

## [Firmware Version 2.7](docs/Firmware_2.7/README.md)

## [Mainboard Rev. 5](docs/Mainboard_Rev5/README.md)

## [Bin Datei aufspielen](docs/Firmware_BIN/README.md)

## [Einbauvorschlag](docs/Tracer_900GT/README.md)

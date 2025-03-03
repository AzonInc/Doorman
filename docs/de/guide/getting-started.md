# Erste Schritte

Ist das dein erster Besuch hier? Keine Sorge, wir haben alles für dich vorbereitet!
Unten findest du eine ausführliche Anleitung zur Ersteinrichtung deines Doormans.

Bitte beachte, dass diese Anweisungen auf dem bereits geflashten Doorman-PCB basieren welches ich verkaufe.
Sofern du selbst ein PCB produzieren lassen hast, musst du zuerst die Firmware flashen. Solltest du dem nicht nachgehen, kannst du lange warten, dass irgendwas von dem unten beschriebenen passiert. 😄

**Vielen Dank, dass du Doorman verwendest! ❤️**

## Verkabelung
Öffne als Erstes das Gehäuse deiner Innenstation. In den meisten Fällen findest du dort eine Schraubklemme mit den Bezeichnungen `a`, `b`, `E` und `P`.

::: danger VORSICHT
**SCHLIEßE AUF KEINEN FALL EINE EXTERNE STROMQUELLE AN DIE TC:BUS-TERMINALE AN.  
DAS KÖNNTE DAS RICHTIG SCHADEN ANRICHTEN!**
:::

Schließe die `b`-Leitung (Ground) an einen der TC:BUS-Anschlüsse deines Doorman an und die `a`-Leitung (24V Bus) an den anderen TC:BUS-Anschluss deines Doorman. Doorman ist wie jedes andere Gerät am Bus und wird **parallel angeschlossen**.

::: warning HINWEIS
Standardmäßig versende ich Revision 1.5 mit einer Jumper-Kappe auf `BUS PWR`. Bitte entferne diese, sofern du Doorman nicht nach dem Schema `2-Draht-Modus über die Innenstation` anschließt.
:::

### Stromversorgungsoptionen:
::: details 3-Draht-Modus über die Innenstation <Badge type="tip" text="Empfohlen" />
Verbinde die `P`-Leitung (+24V) mit dem `P`-Terminal an deinem Doorman.

> [!WARNING]
> Die Stromversorgung der Gegensprechanlage muss mindestens 60mA liefern.

Beispiel:
![3-wire](./images/3wire.png){width=300px}
:::

::: details 2-Draht-Modus über externe Stromversorgung
Schließe die externe Stromversorgung an deinen Doorman an. Nutze dazu die `P` (+5V bis +30V) und `G` (Ground) Schraubklemmen an deinem Doorman.

Beispiel:
![2-wire external via screw terminal](./images/2wire_power_screwterminal.png){width=300px}
:::

::: details 2-Draht-Modus über externe USB-C-Stromversorgung
Schließe die externe Stromversorgung über den USB-C-Anschluss an.

Beispiel:
![2-wire external via usb](./images/2wire_power_usb_c.png){width=300px}
:::

::: details 2-Draht-Modus über die Innenstation <Badge type="danger" text="Hardware Revision 1.5 und neuer" />
> [!DANGER] Wichtig
> Die Nutzung der `a`-Bus-Leitung als Stromquelle bei älteren Hardware Revisionen als `1.5` führt zu einem lauten Piepton. Dieses Problem tritt wahrscheinlich aufgrund der Hochfrequenz-Schaltstromversorgung auf.
>
> Ab Hardware Revision `1.5` erzeugt diese Methode nur noch ein leises, aber dennoch wahrnehmbares Rauschen im Lautsprecher.

Nachdem du die `a` und `b` Leitung angeschlossen hast musst du noch `BUS PWR` mittels Jumper Kappe verbinden.

Beispiel:
![2-wire via intercom jumper](./images/2wire_intercom.png){width=300px}
:::


## Einrichtung

### Schritt 1: Mit Wi-Fi verbinden
Wenn du deinen Doorman-S3 zum ersten Mal einschaltest, wird er schnell (orange) blinken und einen neuen Access Point namens `Doorman-S3 Setup` erstellen. Du kannst deine WiFi-Zugangsdaten über den Access Point, Improv Serial oder Improv Bluetooth einrichten.

Das Passwort für den Access Point lautet `open-sesame`.

Sobald du mit dem Access Point verbunden bist, sollte sich die Web-Oberfläche automatisch öffnen (siehe auch Netzwerk-Benachrichtigungen).\
Falls nicht, kannst du manuell zu http://192.168.4.1/ in deinem Browser navigieren.

::: tip
Wenn dein Home Assistant Bluetooth aktiviert hat, kannst du die automatische Einrichtung nutzen.

Dank mDNS-Unterstützung wird Home Assistant deinen Doorman automatisch entdecken, sobald er mit dem WiFi verbunden ist.
:::
![Home Assistant Discovery](./images/discovery.png)

### Schritt 2: Mit Home Assistant verbinden
Nachdem du deinen Doorman mit deinem Netzwerk verbunden hast, wird die RGB Status LED langsam (blau) blinken. Er sollte automatisch von Home Assistant entdeckt werden. Klicke einfach auf `Konfigurieren`, um das neu entdeckte ESPHome-Gerät hinzuzufügen.

Falls nicht, kannst du ihn manuell per IP Addresse hinzufügen.

Nach erfolgreicher Verbindung leuchtet er für 3 sekunden lang durchgehend blau.

### Schritt 3: Interaktive Einrichtung
::: tip
Wenn du deinen Doorman zum ersten Mal mit Home Assistant verbindest,\
befindet er sich bereits im `Setup Mode` für die interaktive Einrichtung.

Du musst den Modus nicht manuell aktivieren; er wird bei jedem Neustart automatisch gestartet, solange der Einrichtungsprozess nicht abgeschlossen oder abgebrochen wurde.
:::

1. **Zugriff auf die Einstellungen:**
   Öffne die Einstellungen entweder über die Weboberfläche deines Doormans oder besuche die [ESPHome Integrationsseite](https://my.home-assistant.io/redirect/integration/?domain=esphome) und wähle das Doorman S3-Gerät aus.

2. **Aktiviere den Setup-Modus:**  
   Geh zum Bereich `Konfiguration` und schalte den `Setup-Modus` ein, um mit der interaktiven Einrichtung zu beginnen.  
   Sobald der Setup-Prozess beginnt, wird die RGB-Status-LED grün-türkis pulsieren.

::: warning BEVOR DU WEITERMACHST
Deine Innenstation muss angeschlossen und das Gehäuse verschlossen sein, damit die Einrichtung abgeschlossen werden kann.
:::

3. **Warte, bis du auf die Klingel drückst:**  
   Wenn du den Klingelknopf an deiner Wohnung oder am Eingang drückst, speichert das System die Seriennummer deiner Innenstation und versucht, das Modell zu erkennen.

   Sobald das Modell erfolgreich erkannt wurde, wird der Speicher deiner Innenstation ausgelesen.  
   **Hinweis:** Der gesamte Prozess kann **bis zu 30 Sekunden** dauern.

   Wenn das Modell erfolgreich erkannt wurde oder die Zeit abläuft, wird die Einrichtung abgeschlossen.

4. **Einrichtung abgeschlossen:**  
   Die LED leuchtet 3 Sekunden lang grün-türkis und geht dann aus, der Setup-Modus wird deaktiviert. Die Einrichtung ist abgeschlossen.

Wenn du mehrere Außenstationen hast, wird die Firmware versuchen, die zusätzliche Station automatisch zu erkennen.
Um die Erkennung der zweiten Türklingel und das Öffnen der zweiten Tür zu ermöglichen, musst du die zweite Türklingel einmal betätigen oder den physischen Entsperrknopf der zweiten Tür mindestens einmal betätigen, damit die Adresse gespeichert wird.

::: tip MEHRERE INNENSTATIONEN
Wenn du mehrere Innenstationen hast, wird's etwas tricky. Du musst dann eine eigene YAML-Konfiguration erstellen, damit alle zusammen funktionieren. Die Standard-Firmware kann nämlich nur mit einer Innenstation umgehen.
:::

## ESPHome adoption
Wenn du die Firmware deines Doorman anpassen möchtest, kannst du diesen zu deinem [ESPHome-Dashboard](https://my.home-assistant.io/redirect/supervisor_ingress/?addon=5c53de3b_esphome) hinzufügen und deine angepasste [Stock](firmware/stock-firmware.md) oder [Nuki Bridge](firmware/nuki-bridge-firmware.md) Firmware flashen.

Nach dem Hinzufügen wird deine Konfiguration ungefähr so aussehen:
::: details Beispiel
> [!NOTE]
> Hier kannst du neue Komponenten hinzufügen und bestehende ändern. Weitere Informationen zu den Möglichkeiten findest du im Abschnitt [Beispiele](firmware/stock-firmware#beispiele).
>
> Wenn du die Firmware vollständig anpassen möchtest, solltest du die `base.yaml`-Datei aus dem Repository verwenden.

```yaml
<!--@include: ./firmware/minimal.example.yaml-->
```
:::
# Erste Schritte

Hier ist eine ausführliche Anleitung, wie du deinen Doorman zum ersten Mal einrichtest. Bitte beachte, dass diese Anweisungen auf dem bereits geflashten Doorman-PCB basieren welches ich verkaufe.

Danke, dass du Doorman verwendest! ❤️

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
Nachdem du den Doorman mit deinem Netzwerk verbunden hast, wird er langsam (blau) blinken und sollte automatisch von Home Assistant entdeckt werden. Klicke einfach auf `Konfigurieren`, um das neu entdeckte ESPHome-Gerät hinzuzufügen.

### Schritt 3: Bus Befehle setzen
::: warning Wichtig
Du musst den Doorman zuerst mit deiner Gegensprechanlage verbinden. Sieh dir hierfür den Bereich [Verkabelung](#verkabelung) an.
:::

Du kannst die [ESPHome-Integrationsseite](https://my.home-assistant.io/redirect/integration/?domain=esphome) besuchen und auf den neuen Eintrag deines Doorman S3 klicken. Dort findest du einen Last Bus Command-Textsensor, der den zuletzt empfangenen Busbefehl im hexadezimalen Format anzeigt.

Zusätzlich werden alle empfangenen Befehle im ESPHome-Console-Log (Debug-Log) protokolliert und als Home Assistant-Ereignis veröffentlicht.

**Wichtige Befehle:**
- Eingangstür-Klingel
- Wohnungstür-Klingel
- Eingangstür öffnen

Drücke die Tasten an deinem Etagen-Telefon, um die Codes aufzuzeichnen, und kopiere sie dann in die entsprechenden Konfigurations-Textfelder.

## ESPHome adoption
Wenn du die Firmware deines Doorman anpassen möchtest, kannst du diesen zu deinem [ESPHome-Dashboard](https://my.home-assistant.io/redirect/supervisor_ingress/?addon=5c53de3b_esphome) hinzufügen und deine angepasste [Stock](firmware/stock-firmware.md) oder [Nuki Bridge](firmware/nuki-bridge-firmware.md) Firmware flashen.

Nach dem Hinzufügen wird deine Konfiguration ungefähr so aussehen:
::: details Beispiel
> [!NOTE]
> Hier kannst du neue Komponenten hinzufügen und bestehende ändern. Weitere Informationen zu den Möglichkeiten findest du im Abschnitt [Beispiele](firmware/stock-firmware#beispiele).
>
> Wenn du die Firmware vollständig anpassen möchtest, solltest du die `base.yaml`-Datei aus dem Repository verwenden.

```yaml
substitutions:
  name: doorman-s3
  friendly_name: Doorman S3

packages:
  AzonInc.Doorman: github://AzonInc/doorman/firmware/doorman-stock.yaml@master

esphome:
  name: ${name}
  name_add_mac_suffix: false
  friendly_name: ${friendly_name}

api:
  encryption:
    key: ...

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
```
:::


## Verkabelung
Öffne als Erstes das Gehäuse deiner Gegensprechanlage. In den meisten Fällen findest du dort eine Schraubklemme mit den Bezeichnungen `a`, `b`, `E` und `P`.

Schließe die `b`-Leitung (Ground) an einen der TCS:BUS-Anschlüsse deines Doorman an und die `a`-Leitung (24V Bus) an den anderen TCS:BUS-Anschluss deines Doorman.

### Stromversorgungsoptionen:
::: details 3-Draht-Modus über die Gegensprechanlage <Badge type="tip" text="Empfohlen" />
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

::: details 2-Draht-Modus über die Gegensprechanlage <Badge type="danger" text="Nicht möglich" />
> [!DANGER] Das ist leider nicht möglich!
> Die Nutzung der `a`-Bus-Leitung als Stromquelle führt zu einem lauten Piepton. Dieses Problem tritt wahrscheinlich aufgrund der Hochfrequenz-Schaltstromversorgung auf und könnte in zukünftigen Hardware-Versionen behoben werden.

Beispiel:
![2-wire external via usb](./images/2wire_power_a_terminal.png){width=300px}
:::
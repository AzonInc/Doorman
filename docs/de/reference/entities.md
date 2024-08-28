# Entitäten
Die Doorman-Firmware stellt standardmäßig viele Entitäten zur Verfügung. Auf dieser Seite wird erklärt, wozu jede einzelne dient.

## Sensoren

### Last Bus Command  
Meldet jeden veröffentlichten Befehl auf dem Bus.

### Uptime  
Meldet die Betriebszeit in Sekunden.

### Nuki Battery Level  
Meldet den Akkustand des verbundenen Nuki Smart Locks.


## Binäre Sensoren

### Entrance Doorbell  
Wird aktiviert, wenn der Befehl `entrance_doorbell_command` empfangen wird.

### Apartment Doorbell  
Wird aktiviert, wenn der Befehl `apartment_doorbell_command` empfangen wird.

### Pick up phone  
Wird aktiviert, wenn der Befehl `pick_up_phone_command` empfangen wird.

### Hang up phone  
Wird aktiviert, wenn der Befehl `hang_up_phone_command` empfangen wird.

### Ring To Open: Toggle  
Wird aktiviert, wenn der Befehl `ring_to_open_toggle_command` empfangen wird.

### Turn on light: Trigger  
Wird aktiviert, wenn der Befehl `turn_on_light_command` empfangen wird.

### External Button  
Wird aktiviert, wenn der externe Button gedrückt wird.

### Nuki Connected  
Wird aktiviert, wenn das gekoppelte Nuki Smart Lock verbunden ist.

### Nuki Paired  
Wird aktiviert, wenn ein Nuki Smart Lock gekoppelt ist.

### Nuki Battery Critical  
Wird aktiviert, wenn das gekoppelte Nuki Smart Lock einen kritischen Akkustand hat.

### Nuki Door Sensor  
Keine Beschreibung vorhanden.

### Nuki Door Sensor State  
Meldet den Türsensorstatus des gekoppelten Nuki Smart Locks.


## Text Sensors

### Easy Setup: Status
Zeigt den aktuellen Status vom [Easy Setup](../guide/getting-started#easy-setup) Prozess an.


## Schalter

### Ring To Open  
Steuert die [Ring To Open](../guide/automation/ring-to-open) Automatisierung.

### Ring To Open: Confirmation  
Steuert die Einschaltbestätigung für die [Ring To Open](../guide/automation/ring-to-open) Automatisierung.

### Relay  
Steuert das eingebaute Relais.

### Nuki Pairing Mode  
Steuert den Nuki Kopplungsmodus.


## Buttons

### Open Entrance Door  
Öffnet die Eingangstür, indem der Befehl `open_entrance_door_command` auf dem Bus gesendet wird.

### Open Second Door  
Öffnet die zweite Tür, indem der Befehl `open_second_door_command` auf dem Bus gesendet wird.

### Turn on the light  
Schaltet das Licht ein, indem der Befehl `turn_on_light_command` auf dem Bus gesendet wird.

### Easy Setup: Start <Badge type="tip" text="doorman_easy_setup_start" />
Startet den [Easy Setup](../guide/getting-started#easy-setup) Prozess um Bus Commands einzurichten.

### Easy Setup: Cancel <Badge type="tip" text="doorman_easy_setup_cancel" />
Beendet den [Easy Setup](../guide/getting-started#easy-setup) Prozess.

### Nuki Unpair Device  
Entkoppelt dein Nuki Smart Lock.

### Restart  
Startet Doorman neu, nützlich nach Konfigurationsänderungen, die einen Neustart erfordern.

### Safe mode  
Startet Doorman im Sicherheitsmodus neu, nützlich für OTA-Updates, wenn die Einheit zu beschäftigt ist.

### Restore Factory Settings  
Startet Doorman neu und lädt die Werkseinstellungen.

::: Gefahr MIT GROẞER VORSICHT VERWENDEN
Alle Zugangsdaten, globalen Variablen, Zähler und gespeicherten Zustände, die im nichtflüchtigen Speicher gespeichert sind, gehen verloren und können nicht wiederhergestellt werden. Selbst das Auslesen des Flash-Speichers mit esptool hilft nicht, da die Daten physisch aus dem Flash-Speicher gelöscht werden.

Wenn du WiFi über das Captive Portal, Improv Serial oder Improv BLE konfiguriert hast, werden auch die WiFi-Einstellungen zurückgesetzt, wodurch solche Geräte offline gehen. Du musst das Gerät dann über einen eingebauten WiFi-Access-Point und das Captive Portal neu konfigurieren.
:::

## Eingabe für Text

### Command: Apartment Doorbell  
Legt den Code fest, der gesendet wird, wenn jemand an der Wohnungsklingel läutet.

### Command: Entrance Doorbell  
Legt den Code fest, der gesendet wird, wenn jemand an der Eingangsklingel läutet.

### Command: Open Entrance Door  
Legt den Code fest, der gesendet wird, wenn du den Knopf zum Öffnen der Eingangstür drückst.

### Command: Open Second Door  
Legt den Code fest, der gesendet wird, wenn du den Knopf zum Öffnen der zweiten Tür drückst.

### Command: Pick up phone  
Legt den Code fest, der gesendet wird, wenn du den Hörer abnimmst.

### Command: Hang up phone  
Legt den Code fest, der gesendet wird, wenn du den Hörer auflegst.

### Command: Turn on light  
Legt den Code fest, der gesendet wird, wenn du den Knopf zum Einschalten des Flurlichts drückst.

### Command: Ring To Open Toggle  
Legt den Code fest, der verwendet wird, um die [Ring To Open](../guide/automation/ring-to-open) Automatisierung umzuschalten. Der Funktionstaster wird oft für diesen Zweck verwendet, da er üblicherweise nicht genutzt wird.

## Eingabe für Zahlenwert

### Ring To Open: Delay  
Legt die Verzögerung für die [Ring To Open](../guide/automation/ring-to-open) Automatisierung fest.

::: tip
Das Einstellen der Verzögerung auf das Maximum (60 Sekunden) führt dazu, dass die Automatisierung jedes Mal eine zufällige Verzögerung zwischen 5 und 15 Sekunden generiert, wenn sie ausgelöst wird.
:::

## Dropdowns

### Ring To Open: Timeout  
Legt den Timeout-Modus für die [Ring To Open](../guide/automation/ring-to-open) Automatisierung fest.

##### Modi:
- Einmal klingeln
- 5 bis 60 Minuten
- Nie

## Schlösser

### Nuki Lock  
Repräsentiert die Schlosseinheit für dein gekoppeltes Nuki Smart Lock.

## Ereignisse

### Doorbell Pattern  
Wird ausgelöst, wenn ein Klingelmuster erkannt wird. Erfahre mehr über Musterereignisse [hier](../guide/automation/pattern-events).

##### Ereignistypen
- apartment_single
- apartment_double
- apartment_triple
- entrance_single
- entrance_double
- entrance_triple

### Phone pick up Pattern  
Wird ausgelöst, wenn ein Abhebe-Muster des Telefons erkannt wird. Erfahre mehr über Musterereignisse [hier](../guide/automation/pattern-events).

##### Ereignistypen
- single
- double
- triple

## Updates

### Firmware  
Zeigt an, ob ein Update im stabilen Zweig verfügbar ist, und bietet eine Installation über das HTTP OTA-Updateverfahren an.

### Firmware  
Zeigt an, ob ein Update im Entwicklungszweig verfügbar ist, und bietet eine Installation über das HTTP OTA-Updateverfahren an.

## Lichter

### Status LED  
Eine kleine LED auf der Doorman-Platine, die vom ESPHome [Status LED](https://esphome.io/components/light/status_led.html) gesteuert wird. Standardmäßig bleibt diese LED eingeschaltet, wenn alles ordnungsgemäß funktioniert, blinkt jedoch bei Problemen.

Du kannst die LED ausschalten, wenn sie störend wirkt. Selbst im ausgeschalteten Zustand behält sie ihre Funktionalität und blinkt, um auf Probleme hinzuweisen.

### Status LED  
Eine kleine WS2812B RGB-LED auf der Doorman-Platine, die verwendet wird, um bestimmte Ereignisse anzuzeigen. Die LED kann verschiedene Farben und Muster anzeigen, um visuelles Feedback zu unterschiedlichen Zuständen und Aktionen zu geben.

##### Ereignistypen
- Warten auf WiFi / AP aktiviert <Badge type="tip" text="Fast orange pulse" />
- Warten auf Home Assistant-Verbindung <Badge type="tip" text="Slow blue pulse" />
- Home Assistant verbunden <Badge type="tip" text="Solid blue light (for 3 seconds)" />
- Ring To Open ist aktiv <Badge type="tip" text="Slow yellow pulse" />
- Nuki Kopplungsmodus ist aktiv <Badge type="tip" text="Slow purple pulse" />
- Nuki erfolgreich gekoppelt <Badge type="tip" text="Solid purple light (for 3 seconds)" />
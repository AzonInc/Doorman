# Entitäten
Die Doorman-Firmware stellt standardmäßig viele Entitäten zur Verfügung. Auf dieser Seite wird erklärt, wozu jede einzelne dient.

## Sensoren

### Last Bus Command <Badge type="tip" text="last_bus_command" />
Meldet jeden veröffentlichten Befehl auf dem Bus.

### Uptime <Badge type="tip" text="doorman_uptime" />
Meldet die Betriebszeit in Sekunden.

### Nuki Battery Level <Badge type="tip" text="nuki_battery_level" /> <Badge type="info" text="Nuki Bridge only" />
Meldet den Akkustand des verbundenen Nuki Smart Locks.


## Binäre Sensoren

### Entrance Doorbell <Badge type="tip" text="entrance_doorbell" />
Wird aktiviert, wenn der Befehl `entrance_doorbell_command` empfangen wird.

### Apartment Doorbell <Badge type="tip" text="apartment_doorbell" />
Wird aktiviert, wenn der Befehl `apartment_doorbell_command` empfangen wird.

### Pick up phone <Badge type="tip" text="pick_up_phone" /> <Badge type="info" text="Disabled by default" />
Wird aktiviert, wenn der Befehl `pick_up_phone_command` empfangen wird.

### Hang up phone <Badge type="tip" text="hang_up_phone" /> <Badge type="info" text="Disabled by default" />
Wird aktiviert, wenn der Befehl `hang_up_phone_command` empfangen wird.

### Function Button <Badge type="tip" text="function_button" />
Wird aktiviert, wenn der Befehl `function_button_command` empfangen wird.

### Light Button <Badge type="tip" text="light_button" />
Wird aktiviert, wenn der Befehl `light_button_command` empfangen wird.

### Boot Button <Badge type="tip" text="doorman_boot_button" /> <Badge type="info" text="Disabled by default" /> <Badge type="info" text="Internal" />
Wird aktiviert, wenn der Boot Button gedrückt wird.

### External Button <Badge type="tip" text="doorman_external_button" /> <Badge type="info" text="Disabled by default" />
Wird aktiviert, wenn der externe Button gedrückt wird.

### Nuki Connected <Badge type="tip" text="nuki_connected" /> <Badge type="info" text="Nuki Bridge only" />
Wird aktiviert, wenn das gekoppelte Nuki Smart Lock verbunden ist.

### Nuki Paired <Badge type="tip" text="nuki_paired" /> <Badge type="info" text="Nuki Bridge only" />
Wird aktiviert, wenn ein Nuki Smart Lock gekoppelt ist.

### Nuki Battery Critical <Badge type="tip" text="nuki_battery_critical" /> <Badge type="info" text="Nuki Bridge only" />
Wird aktiviert, wenn das gekoppelte Nuki Smart Lock einen kritischen Akkustand hat.

### Nuki Door Sensor <Badge type="tip" text="nuki_door_sensor" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Keine Beschreibung vorhanden.

### Nuki Door Sensor State <Badge type="tip" text="nuki_door_sensor_state" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Meldet den Türsensorstatus des gekoppelten Nuki Smart Locks.


## Text Sensors

### Interactive Setup: Status <Badge type="tip" text="doorman_interactive_setup_status" />
Zeigt den aktuellen Status vom [Interactive Setup](../guide/getting-started#interactive-setup) Prozess an.


## Schalter

### Ring To Open <Badge type="tip" text="doorman_ring_to_open" />
Steuert die [Ring To Open](../guide/automation/ring-to-open) Automatisierung.

### Ring To Open: Confirmation <Badge type="tip" text="doorman_ring_to_open_confirmation" /> <Badge type="info" text="Disabled by default" />
Steuert die Einschaltbestätigung für die [Ring To Open](../guide/automation/ring-to-open) Automatisierung.

### Relay <Badge type="tip" text="doorman_relay" /> <Badge type="info" text="Disabled by default" />
Steuert das eingebaute Relais.

### Nuki Pairing Mode <Badge type="tip" text="nuki_pairing_mode" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Steuert den Nuki Kopplungsmodus.


## Buttons

### Open Entrance Door <Badge type="tip" text="open_entrance_door" />
Öffnet die Eingangstür, indem der Befehl `open_entrance_door_command` auf dem Bus gesendet wird.

### Open Second Door <Badge type="tip" text="open_second_door" /> <Badge type="info" text="Disabled by default" />
Öffnet die zweite Tür, indem der Befehl `open_second_door_command` auf dem Bus gesendet wird.

### Turn on the light <Badge type="tip" text="turn_on_light" /> <Badge type="info" text="Disabled by default" />
Schaltet das Licht ein, indem der Befehl `light_button_command` auf dem Bus gesendet wird.

### Interactive Setup: Start <Badge type="tip" text="doorman_interactive_setup_start" />
Startet den [Interactive Setup](../guide/getting-started#interactive-setup) Prozess um Bus Commands einzurichten.

### Interactive Setup: Cancel <Badge type="tip" text="doorman_interactive_setup_cancel" />
Beendet den [Interactive Setup](../guide/getting-started#interactive-setup) Prozess.

### Nuki Unpair Device <Badge type="tip" text="nuki_unpair_device" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Entkoppelt dein Nuki Smart Lock.

### Restart <Badge type="tip" text="doorman_restart" /> <Badge type="info" text="Disabled by default" />
Startet Doorman neu, nützlich nach Konfigurationsänderungen, die einen Neustart erfordern.

### Safe mode <Badge type="tip" text="doorman_safe_mode" /> <Badge type="info" text="Disabled by default" />
Startet Doorman im Sicherheitsmodus neu, nützlich für OTA-Updates, wenn die Einheit zu beschäftigt ist.

### Restore Factory Settings <Badge type="tip" text="doorman_factory_reset" /> <Badge type="info" text="Disabled by default" />
Startet Doorman neu und lädt die Werkseinstellungen.

::: Gefahr MIT GROẞER VORSICHT VERWENDEN
Alle Zugangsdaten, globalen Variablen, Zähler und gespeicherten Zustände, die im nichtflüchtigen Speicher gespeichert sind, gehen verloren und können nicht wiederhergestellt werden. Selbst das Auslesen des Flash-Speichers mit esptool hilft nicht, da die Daten physisch aus dem Flash-Speicher gelöscht werden.

Wenn du WiFi über das Captive Portal, Improv Serial oder Improv BLE konfiguriert hast, werden auch die WiFi-Einstellungen zurückgesetzt, wodurch solche Geräte offline gehen. Du musst das Gerät dann über einen eingebauten WiFi-Access-Point und das Captive Portal neu konfigurieren.
:::

## Eingabe für Text

### Command: Apartment Doorbell <Badge type="tip" text="apartment_doorbell_command_input" />
Legt den Code fest, der gesendet wird, wenn jemand an der Wohnungsklingel läutet.

### Command: Entrance Doorbell <Badge type="tip" text="entrance_doorbell_command_input" />
Legt den Code fest, der gesendet wird, wenn jemand an der Eingangsklingel läutet.

### Command: Open Entrance Door <Badge type="tip" text="open_entrance_door_command_input" />
Legt den Code fest, der gesendet wird, wenn du den Knopf zum Öffnen der Eingangstür drückst.

### Command: Open Second Door <Badge type="tip" text="open_second_door_command_input" /> <Badge type="info" text="Disabled by default" />
Legt den Code fest, der gesendet wird, wenn du den Knopf zum Öffnen der zweiten Tür drückst.

### Command: Pick up phone <Badge type="tip" text="pick_up_phone_command_input" /> <Badge type="info" text="Disabled by default" />
Legt den Code fest, der gesendet wird, wenn du den Hörer abnimmst.

### Command: Hang up phone <Badge type="tip" text="hang_up_phone_command_input" /> <Badge type="info" text="Disabled by default" />
Legt den Code fest, der gesendet wird, wenn du den Hörer auflegst.

### Command: Light Button <Badge type="tip" text="light_button_command_input" /> <Badge type="info" text="Disabled by default" />
Legt den Code fest, der gesendet wird, wenn du den Knopf zum Einschalten des Flurlichts drückst.

### Command: Function Button <Badge type="tip" text="function_button_command_input" /> <Badge type="info" text="Disabled by default" />
Legt den Code fest, der gesendet wird, wenn du den Funktionstaster (Kreis) drückst.\
Der Funktionstaster wird üblicherweise nicht genutzt.\
Aus diesem Grund wird er verwendet, um die [Ring To Open](../guide/automation/ring-to-open) Automatisierung umzuschalten. 

## Eingabe für Zahlenwert

### Ring To Open: Delay <Badge type="tip" text="doorman_ring_to_open_delay" /> <Badge type="info" text="Disabled by default" />
Legt die Verzögerung für die [Ring To Open](../guide/automation/ring-to-open) Automatisierung fest.

::: tip
Das Einstellen der Verzögerung auf das Maximum (60 Sekunden) führt dazu, dass die Automatisierung jedes Mal eine zufällige Verzögerung zwischen 5 und 15 Sekunden generiert, wenn sie ausgelöst wird.
:::

## Dropdowns

### Ring To Open: Timeout <Badge type="tip" text="doorman_ring_to_open_timeout_mode" /> <Badge type="info" text="Disabled by default" /> 
Legt den Timeout-Modus für die [Ring To Open](../guide/automation/ring-to-open) Automatisierung fest.

##### Modi:
- Einmal klingeln
- 5 bis 60 Minuten
- Nie

## Schlösser

### Nuki Lock <Badge type="tip" text="nuki_smart_lock" /> <Badge type="info" text="Nuki Bridge only" />
Repräsentiert die Schlosseinheit für dein gekoppeltes Nuki Smart Lock.

## Ereignisse

### Doorbell Pattern <Badge type="tip" text="doorbell_pattern" />
Wird ausgelöst, wenn ein Klingelmuster erkannt wird. Erfahre mehr über Musterereignisse [hier](../guide/automation/pattern-events).

##### Ereignistypen
- apartment_single
- apartment_double
- apartment_triple
- entrance_single
- entrance_double
- entrance_triple

### Phone pick up Pattern <Badge type="tip" text="phone_pick_up_pattern" />
Wird ausgelöst, wenn ein Abhebe-Muster des Telefons erkannt wird. Erfahre mehr über Musterereignisse [hier](../guide/automation/pattern-events).

##### Ereignistypen
- single
- double
- triple

## Updates

### Firmware <Badge type="info" text="Stable" /> <Badge type="tip" text="update_http_request_stable" />
Zeigt an, ob ein Update im stabilen Zweig verfügbar ist, und bietet eine Installation über das HTTP OTA-Updateverfahren an.

### Firmware <Badge type="info" text="Development" /> <Badge type="tip" text="update_http_request_dev" />
Zeigt an, ob ein Update im Entwicklungszweig verfügbar ist, und bietet eine Installation über das HTTP OTA-Updateverfahren an.

## Lichter

### Status LED <Badge type="danger" text="Red" /> <Badge type="tip" text="doorman_status_led" />
Eine kleine LED auf der Doorman-Platine, die vom ESPHome [Status LED](https://esphome.io/components/light/status_led.html) gesteuert wird. Standardmäßig bleibt diese LED eingeschaltet, wenn alles ordnungsgemäß funktioniert, blinkt jedoch bei Problemen.

Du kannst die LED ausschalten, wenn sie störend wirkt. Selbst im ausgeschalteten Zustand behält sie ihre Funktionalität und blinkt, um auf Probleme hinzuweisen.

### Status LED <Badge type="warning" text="RGB" /> <Badge type="tip" text="doorman_rgb_status_led" />
Eine kleine WS2812B RGB-LED auf der Doorman-Platine, die verwendet wird, um bestimmte Ereignisse anzuzeigen. Die LED kann verschiedene Farben und Muster anzeigen, um visuelles Feedback zu unterschiedlichen Zuständen und Aktionen zu geben.

##### Ereignistypen
- Warten auf WiFi / AP aktiviert <Badge type="tip" text="Fast orange pulse" />
- Warten auf Home Assistant-Verbindung <Badge type="tip" text="Slow blue pulse" />
- Home Assistant verbunden <Badge type="tip" text="Solid blue light (for 3 seconds)" />
- Ring To Open ist aktiv <Badge type="tip" text="Slow yellow pulse" />
- Nuki Kopplungsmodus ist aktiv <Badge type="tip" text="Slow purple pulse" />
- Nuki erfolgreich gekoppelt <Badge type="tip" text="Solid purple light (for 3 seconds)" />
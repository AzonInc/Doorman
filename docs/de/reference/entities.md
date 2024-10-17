# Entitäten
Die Doorman-Firmware stellt standardmäßig viele Entitäten zur Verfügung. Auf dieser Seite wird erklärt, wozu jede einzelne dient.

## Sensoren

### Last Bus Command <Badge type="tip" text="last_bus_command" />
Meldet jeden veröffentlichten Befehl auf dem Bus.

### Uptime <Badge type="tip" text="doorman_uptime" /> <Badge type="info" text="Deaktiviert" />
Meldet die Betriebszeit in Sekunden.

### Nuki Battery Level <Badge type="tip" text="nuki_battery_level" /> <Badge type="info" text="Nuki Bridge only" />
Meldet den Akkustand des verbundenen Nuki Smart Locks.


## Binäre Sensoren

### Entrance Doorbell <Badge type="tip" text="entrance_doorbell" />
Wird aktiviert, wenn der Befehl vom Typ `door_call` empfangen wird.

### Second Entrance Doorbell <Badge type="tip" text="second_entrance_doorbell" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `door_call` mit zweiter Außenstation `address` empfangen wird.

### Apartment Doorbell <Badge type="tip" text="apartment_doorbell" />
Wird aktiviert, wenn der Befehl vom Typ `floor_call` empfangen wird.

### Pick up phone <Badge type="tip" text="pick_up_phone" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `start_talking_door_station` empfangen wird.

### Hang up phone <Badge type="tip" text="hang_up_phone" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `stop_talking_door_station` empfangen wird.

### Function Button <Badge type="tip" text="function_button" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `control_function` empfangen wird.

### Light Button <Badge type="tip" text="light_button" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `light` empfangen wird.

### Door readiness <Badge type="tip" text="door_readiness" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn die Türbereitschaft aktiv ist.

### Flash Button <Badge type="tip" text="doorman_boot_button" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der `FLASH` bzw. `PRG` Button auf deinem Doorman gedrückt wird.

### External Button <Badge type="tip" text="doorman_external_button" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der externe Button gedrückt wird.

### Nuki Connected <Badge type="tip" text="nuki_connected" /> <Badge type="info" text="Nuki Bridge only" />
Wird aktiviert, wenn das gekoppelte Nuki Smart Lock verbunden ist.

### Nuki Paired <Badge type="tip" text="nuki_paired" /> <Badge type="info" text="Nuki Bridge only" />
Wird aktiviert, wenn ein Nuki Smart Lock gekoppelt ist.

### Nuki Battery Critical <Badge type="tip" text="nuki_battery_critical" /> <Badge type="info" text="Nuki Bridge only" />
Wird aktiviert, wenn das gekoppelte Nuki Smart Lock einen kritischen Akkustand hat.

### Nuki Door Sensor <Badge type="tip" text="nuki_door_sensor" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Deaktiviert" />
Keine Beschreibung vorhanden.

### Nuki Door Sensor State <Badge type="tip" text="nuki_door_sensor_state" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Deaktiviert" />
Meldet den Türsensorstatus des gekoppelten Nuki Smart Locks.


## Schalter

### Ring To Open <Badge type="tip" text="doorman_ring_to_open" />
Steuert die [Ring To Open](../guide/automation/ring-to-open) Automatisierung.

### Ring To Open: Confirmation <Badge type="tip" text="doorman_ring_to_open_confirmation" /> <Badge type="info" text="Deaktiviert" />
Steuert die Einschaltbestätigung für die [Ring To Open](../guide/automation/ring-to-open) Automatisierung.

### Relay <Badge type="tip" text="doorman_relay" /> <Badge type="info" text="Deaktiviert" />
Steuert das eingebaute Relais.

### Setup Mode <Badge type="tip" text="doorman_setup_mode" />
Aktiviert oder deaktiviert den Modus für die [interaktive Einrichtung](../guide/getting-started#schritt-3-interaktive-einrichtung).

### Nuki Pairing Mode <Badge type="tip" text="nuki_pairing_mode" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Deaktiviert" />
Steuert den Nuki Kopplungsmodus.


## Buttons

### Open Entrance Door <Badge type="tip" text="open_entrance_door" />
Öffnet die Eingangstür, indem der Befehlvom Typ `open_door` auf dem Bus gesendet wird.

### Open Second Entrance Door <Badge type="tip" text="open_second_door" /> <Badge type="info" text="Deaktiviert" />
Öffnet die zweite Eingangstür, indem der Befehl vom Typ `open_door` mit `address` der zweiten Außenstelle auf dem Bus gesendet wird.

### Turn on the Light <Badge type="tip" text="turn_on_light" /> <Badge type="info" text="Deaktiviert" />
Schaltet das Licht ein, indem der Befehl `light_button_command` auf dem Bus gesendet wird.

### Read Memory <Badge type="tip" text="read_memory" /> <Badge type="info" text="Deaktiviert" />
Liest den internen speicher deines Gegensprechanlagen Telefons mit der angegebenen Seriennummer aus.

### Nuki Unpair Device <Badge type="tip" text="nuki_unpair_device" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Deaktiviert" />
Entkoppelt dein Nuki Smart Lock.

### Restart <Badge type="tip" text="doorman_restart" /> <Badge type="info" text="Deaktiviert" />
Startet Doorman neu, nützlich nach Konfigurationsänderungen, die einen Neustart erfordern.

### Safe mode <Badge type="tip" text="doorman_safe_mode" /> <Badge type="info" text="Deaktiviert" />
Startet Doorman im Sicherheitsmodus neu, nützlich für OTA-Updates, wenn die Einheit zu beschäftigt ist.

### Restore Factory Settings <Badge type="tip" text="doorman_factory_reset" /> <Badge type="info" text="Deaktiviert" />
Startet Doorman neu und lädt die Werkseinstellungen.

::: Gefahr MIT GROẞER VORSICHT VERWENDEN
Alle Zugangsdaten, globalen Variablen, Zähler und gespeicherten Zustände, die im nichtflüchtigen Speicher gespeichert sind, gehen verloren und können nicht wiederhergestellt werden. Selbst das Auslesen des Flash-Speichers mit esptool hilft nicht, da die Daten physisch aus dem Flash-Speicher gelöscht werden.

Wenn du WiFi über das Captive Portal, Improv Serial oder Improv BLE konfiguriert hast, werden auch die WiFi-Einstellungen zurückgesetzt, wodurch solche Geräte offline gehen. Du musst das Gerät dann über einen eingebauten WiFi-Access-Point und das Captive Portal neu konfigurieren.
:::


## Eingabe für Zahlenwert

### Ring To Open: Delay <Badge type="tip" text="doorman_ring_to_open_delay" /> <Badge type="info" text="Deaktiviert" />
Legt die Verzögerung für die [Ring To Open](../guide/automation/ring-to-open) Automatisierung fest.

::: tip
Das Einstellen der Verzögerung auf das Maximum (60 Sekunden) führt dazu, dass die Automatisierung jedes Mal eine zufällige Verzögerung zwischen 5 und 15 Sekunden generiert, wenn sie ausgelöst wird.
:::

### Volume: Ringtone <Badge type="tip" text="intercom_volume_ringtone" /> <Badge type="info" text="Disabled by default" />
Legt die Lautstärke der Klingeltöne deines Gegensprechanlagen Telefons fest.

### Volume: Handset <Badge type="tip" text="intercom_volume_handset" /> <Badge type="info" text="Disabled by default" />
Legt die Lautstärke des Hörers deines Gegensprechanlagen Telefons fest.


## Dropdowns

### Ring To Open: Timeout <Badge type="tip" text="doorman_ring_to_open_timeout_mode" />
Legt den Timeout-Modus für die [Ring To Open](../guide/automation/ring-to-open) Automation fest.

##### Modi:
- Ring once
- 5...60 Minutes
- Never

### Ring To Open: Door Station <Badge type="tip" text="doorman_ring_to_open_door_trigger" />
Legt die auslösende Außenstelle für die [Ring To Open](../guide/automation/ring-to-open) Automation fest.

##### Optionen:
- Entrance
- Second Entrance
- Any

### Ringtone: Door Call <Badge type="tip" text="intercom_ringtone_door_call" /> <Badge type="info" text="Disabled by default" />
Legt den Klingelton für Tür-Rufe deines Gegensprechanlagen Telefons fest.

##### Options:
- Ringtone 1
...
- Ringtone 13

### Ringtone: Floor Call <Badge type="tip" text="intercom_ringtone_floor_call" /> <Badge type="info" text="Disabled by default" />
Legt den Klingelton für Etagen-Rufe deines Gegensprechanlagen Telefons fest.

##### Options:
- Ringtone 1
...
- Ringtone 13

### Ringtone: Internal Call <Badge type="tip" text="intercom_ringtone_internal_call" /> <Badge type="info" text="Disabled by default" />
Legt den Klingelton für Intern-Rufe deines Gegensprechanlagen Telefons fest.

##### Options:
- Ringtone 1
...
- Ringtone 13


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
- second_entrance_single
- second_entrance_double
- second_entrance_triple

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
# Entitäten
Die Doorman-Firmware stellt standardmäßig viele Entitäten zur Verfügung.
Unten findest du eine Liste der verfügbaren Sensoren und Steuerungen sowie eine Erklärung, wie du sie konfigurieren kannst.

## Anzeigen der Entitäten in Home Assistant

1. Öffne die Benutzeroberfläche von Home Assistant und gehe zu [ESPHome](https://my.home-assistant.io/redirect/integration/?domain=esphome).
2. Wähle dein Doorman-Gerät aus.
3. Es erscheint ein Dashboard, das alle verfügbaren Sensoren und Steuerungen für Automatisierungen auflistet.

## Sensoren

### Uptime <Badge type="tip" text="doorman_uptime" /> <Badge type="info" text="Deaktiviert" />
Meldet die Betriebszeit in Sekunden.

### WiFi Signal <Badge type="tip" text="doorman_wifi_signal" /> <Badge type="info" text="Deaktiviert" />
Meldet die WLAN Signalstärke in Prozent.

### Nuki Battery Level <Badge type="tip" text="nuki_battery_level" /> <Badge type="info" text="Nuki Bridge" />
Meldet den Akkustand des verbundenen Nuki Smart Locks.

### Nuki Bluetooth Signal Strength <Badge type="tip" text="nuki_bluetooth_signal_strength" /> <Badge type="info" text="Nuki Bridge" />
Meldet die Bluetooth Signalstärke des verbundenen Nuki Smart Locks.

## Binäre Sensoren

### Entrance Doorbell <Badge type="tip" text="entrance_doorbell" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `door_call` empfangen wird.

### Second Entrance Doorbell <Badge type="tip" text="second_entrance_doorbell" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `door_call` mit zweiter Außenstation `address` empfangen wird.

### Apartment Doorbell <Badge type="tip" text="apartment_doorbell" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `floor_call` empfangen wird.

### Pick up phone <Badge type="tip" text="pick_up_phone" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `start_talking` empfangen wird.

### Pick up phone (door call) <Badge type="tip" text="pick_up_phone_door_call" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `start_talking_door_call` empfangen wird.

### Hang up phone <Badge type="tip" text="hang_up_phone" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `stop_talking` empfangen wird.

### Hang up phone (door call) <Badge type="tip" text="hang_up_phone_door_call" /> <Badge type="info" text="Deaktiviert" />
Wird aktiviert, wenn der Befehl vom Typ `stop_talking_door_call` empfangen wird.

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

### Nuki Connected <Badge type="tip" text="nuki_connected" /> <Badge type="info" text="Nuki Bridge" />
Wird aktiviert, wenn das gekoppelte Nuki Smart Lock verbunden ist.

### Nuki Paired <Badge type="tip" text="nuki_paired" /> <Badge type="info" text="Nuki Bridge" />
Wird aktiviert, wenn ein Nuki Smart Lock gekoppelt ist.

### Nuki Battery Critical <Badge type="tip" text="nuki_battery_critical" /> <Badge type="info" text="Nuki Bridge" />
Wird aktiviert, wenn das gekoppelte Nuki Smart Lock einen kritischen Akkustand hat.

### Nuki Door Sensor <Badge type="tip" text="nuki_door_sensor" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Keine Beschreibung vorhanden.


## Text Sensoren

### Last Bus Telegram <Badge type="tip" text="last_bus_telegram" />
Meldet jeden veröffentlichten Befehl auf dem Bus.

### Nuki Door Sensor State <Badge type="tip" text="nuki_door_sensor_state" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Meldet den Türsensorstatus des gekoppelten Nuki Smart Locks.

### Nuki Last Unlock User <Badge type="tip" text="nuki_last_unlock_user" /> <Badge type="info" text="Nuki Bridge" />
Reports the last user who locked or unlocked the Nuki smart lock.

### Nuki Last Lock Action <Badge type="tip" text="nuki_last_lock_action" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Reports the last lock action of the Nuki smart lock.

### Nuki Last Lock Action Trigger <Badge type="tip" text="nuki_last_lock_action_trigger" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Reports the last log action trigger of the Nuki smart lock.

### Nuki Security Pin Status <Badge type="tip" text="nuki_pin_status" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Meldet den Pin Status der Nuki Bridge Komponente.

### ESPHome Version <Badge type="tip" text="esphome_version" /> <Badge type="info" text="Deaktiviert" />
Zeigt die installierte ESPHome Basisversion an.

### Doorman Firmware Version <Badge type="tip" text="doorman_firmware_version" /> <Badge type="info" text="Deaktiviert" />
Zeigt die installierte Doorman Firmware Version und Buildzeit an.

### Doorman Hardware <Badge type="tip" text="doorman_hardware_version" /> <Badge type="info" text="Deaktiviert" />
Zeigt die Doorman Hardware-Revision an.


## Schalter

### Ring To Open <Badge type="tip" text="rto_central" />
Schaltet die [Ring To Open](../guide/features/ring-to-open) Automatisierung für die festgelegten Türen ein/aus.

::: tip
Du kannst die Türen mit dem [RTO: Central Toggle - Door Selection](#rto-central-toggle-door-selection)-Dropdown festlegen.
:::

### RTO: Entrance Door <Badge type="tip" text="rto_entrance_door" /> <Badge type="info" text="Deaktiviert" />
Steuert die [Ring To Open](../guide/features/ring-to-open) Automatisierung der Eingangstür.

### RTO: Second Entrance Door <Badge type="tip" text="rto_second_entrance_door" /> <Badge type="info" text="Deaktiviert" />
Steuert die [Ring To Open](../guide/features/ring-to-open) Automatisierung der zweiten Eingangstür.

### RTO: Apartment Door <Badge type="tip" text="rto_apartment_door" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Steuert die [Ring To Open](../guide/features/ring-to-open) Automatisierung der Wohnungstür.

### RTO: Confirmation <Badge type="tip" text="rto_confirmation" /> <Badge type="info" text="Deaktiviert" />
Steuert die Einschaltbestätigung für die [Ring To Open](../guide/features/ring-to-open) Automatisierung.

### RTO: Display Status <Badge type="tip" text="rto_led_status" /> <Badge type="info" text="Deaktiviert" />
Steuert die Status LED anzeige für die [Ring To Open](../guide/features/ring-to-open) Automatisierung.

### Use 32-Bit Door Protocol <Badge type="tip" text="use_32_open_door_protocol" /> <Badge type="info" text="Deaktiviert" />
Dies erzwingt den langen Türöffner-Befehl und stellt sicher, dass dem kurzen Türöffner-Befehl stets eine Seriennummer hinzugefügt wird.

### Status LED: Show Bus Activity <Badge type="tip" text="doorman_status_led_bus_activity" /> <Badge type="info" text="Deaktiviert" />
Wenn diese Option aktiviert ist, blinkt die Status-LED kurz bei Aktivität auf dem Bus – praktisch zur Fehlersuche oder einfach, um die Kommunikation im Blick zu behalten.

### Relay <Badge type="tip" text="doorman_relay" /> <Badge type="info" text="Deaktiviert" />
Steuert das eingebaute Relais.

### Setup Mode <Badge type="tip" text="doorman_setup_mode" />
Aktiviert oder deaktiviert den Modus für die [interaktive Einrichtung](../guide/getting-started#interaktive-einrichtung).

### Experimental Firmware <Badge type="tip" text="dev_firmware" />
Schaltet experimentelle Updates frei, damit du ganz einfach zwischen dem Master- und Dev-Zweig wechseln und die neuesten Änderungen ausprobieren kannst.

### Nuki Pairing Mode <Badge type="tip" text="nuki_pairing_mode" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Steuert den Nuki Kopplungsmodus.

### Nuki Auto unlatch <Badge type="tip" text="nuki_auto_unlatch" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Auto unlatch setting.

### Nuki Button: Locking operations <Badge type="tip" text="nuki_button_locking_operations" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Button locking operations setting.

### Nuki LED: Signal <Badge type="tip" text="nuki_led_enabled" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock LED Signal setting.

### Nuki Night Mode <Badge type="tip" text="nuki_night_mode" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Night Mode setting.

### Nuki Night Mode: Auto Lock <Badge type="tip" text="nuki_night_mode_auto_lock_enabled" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Night Mode Auto Lock setting.

### Nuki Night Mode: Reject Auto Unlock <Badge type="tip" text="nuki_night_mode_auto_unlock_disabled" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Night Mode Reject Auto Unlock setting.

### Nuki Night Mode: Lock at Start Time <Badge type="tip" text="nuki_night_mode_immediate_lock_on_start" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Night Mode Lock at Start Time setting.

### Nuki Auto Lock <Badge type="tip" text="nuki_auto_lock" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Auto Lock setting.

### Nuki Auto Lock: Immediately <Badge type="tip" text="nuki_immediate_auto_lock" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Auto Lock Immediately setting.

### Nuki Auto Unlock: Disable <Badge type="tip" text="nuki_auto_unlock_disabled" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Auto Unlock Disable setting.

### Nuki Single Lock <Badge type="tip" text="nuki_single_lock" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Single Lock setting.


## Buttons

### Hallway Light <Badge type="tip" text="hallway_light" /> <Badge type="info" text="Deaktiviert" />
Schaltet das Licht ein, indem der Befehl `light` auf dem Bus gesendet wird.

### Identify Indoor Station <Badge type="tip" text="identify_indoor_station" /> <Badge type="info" text="Deaktiviert" />
Ermittelt das Modell der Innenstation und speichert es.
::: note Hinweis
Es werden nicht alle Modelle unterstützt, da ältere Modelle diese Funktion ggfs. nicht unterstützen.
:::

### Read Memory <Badge type="tip" text="read_memory" /> <Badge type="info" text="Deaktiviert" />
Liest den internen speicher deiner Innenstation mit der angegebenen Seriennummer aus.

### Reset HomeKit Pairing <Badge type="tip" text="homekit_reset_pairing" /> <Badge type="info" text="HomeKit Integration" />
Entkoppelt alle Geräte die per HomeKit gekoppelt sind.

### Nuki Unpair Device <Badge type="tip" text="nuki_unpair_device" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Entkoppelt dein Nuki Smart Lock.

### Save MQTT Configuration <Badge type="tip" text="mqtt_save_settings" /> <Badge type="info" text="MQTT Integration" />
Saves the MQTT credentials and reconnect.

### Install Update <Badge type="tip" text="update_install" />
Installiert die neuste Doorman Firmware Version.

### Restart <Badge type="tip" text="doorman_restart" /> <Badge type="info" text="Deaktiviert" />
Startet Doorman neu, nützlich nach Konfigurationsänderungen, die einen Neustart erfordern.

### Safe mode <Badge type="tip" text="doorman_safe_mode" /> <Badge type="info" text="Deaktiviert" />
Startet Doorman im Sicherheitsmodus neu, nützlich für OTA-Updates, wenn die Einheit zu beschäftigt ist.

### Restore Factory Settings <Badge type="tip" text="doorman_factory_reset" /> <Badge type="info" text="Deaktiviert" />
Startet Doorman neu und lädt die Werkseinstellungen.

::: danger MIT GROẞER VORSICHT VERWENDEN
Alle Zugangsdaten, globalen Variablen, Zähler und gespeicherten Zustände, die im nichtflüchtigen Speicher gespeichert sind, gehen verloren und können nicht wiederhergestellt werden. Selbst das Auslesen des Flash-Speichers mit esptool hilft nicht, da die Daten physisch aus dem Flash-Speicher gelöscht werden.

Wenn du WiFi über das Captive Portal, Improv Serial oder Improv BLE konfiguriert hast, werden auch die WiFi-Einstellungen zurückgesetzt, wodurch solche Geräte offline gehen. Du musst das Gerät dann über einen eingebauten WiFi-Access-Point und das Captive Portal neu konfigurieren.
:::


## Eingabe für Zahlenwert

### Serial Number <Badge type="tip" text="indoor_station_serial_number" /> <Badge type="info" text="Deaktiviert" />
Legt die Seriennummer der Innenstation für den Telegrambuilder/parser fest.

### Entrance Door Station ID <Badge type="tip" text="entrance_door_station_id" /> <Badge type="info" text="Deaktiviert" />
Legt die ID der Außenstelle am Eingang fest.

### Second Entrance Door Station ID <Badge type="tip" text="second_entrance_door_station_id" /> <Badge type="info" text="Deaktiviert" />
Legt die ID der zweiten Außenstelle fest.

### Volume: Ringtone <Badge type="tip" text="indoor_station_volume_ringtone" /> <Badge type="info" text="Deaktiviert" />
Legt die Lautstärke der Klingeltöne deiner Innenstation fest.

### Volume: Handset Door Call <Badge type="tip" text="indoor_station_volume_handset_door_call" /> <Badge type="info" text="Deaktiviert" />
Legt die Lautstärke des Hörers deiner Innenstation bei Türrufen fest.

### Volume: Handset Internal Call <Badge type="tip" text="indoor_station_volume_handset_internal_call" /> <Badge type="info" text="Deaktiviert" />
Legt die Lautstärke des Hörers deiner Innenstation bei Internrufen fest.

### Nuki LED: Brightness <Badge type="tip" text="nuki_led_brightness" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock LED Brightness setting.

### Nuki Security Pin <Badge type="tip" text="nuki_security_pin" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Sets the Nuki Bridge Security Pin to authenticate against the Nuki Smart Lock.

### MQTT Broker Port <Badge type="tip" text="mqtt_broker_port" /> <Badge type="info" text="MQTT Integration" /> <Badge type="info" text="Deaktiviert" />
Sets the MQTT Broker Port.


## Eingabe für Text

### MQTT Broker Address <Badge type="tip" text="mqtt_broker_address" /> <Badge type="info" text="MQTT Integration" />
Sets the MQTT Broker Address.

### MQTT Broker Username <Badge type="tip" text="mqtt_broker_username" /> <Badge type="info" text="MQTT Integration" /> <Badge type="info" text="Deaktiviert" />
Sets the MQTT Broker Username.

### MQTT Broker Password <Badge type="tip" text="mqtt_broker_password" /> <Badge type="info" text="MQTT Integration" /> <Badge type="info" text="Deaktiviert" />
Sets the MQTT Broker Password.

### Entrance Door Pre-Open-Telegrams <Badge type="tip" text="entrance_door_before_open_cmds" /> <Badge type="info" text="Deaktiviert" />
Gib eine Liste von Befehlen an (durch Semikolons getrennt), die auf den Bus gesendet werden, bevor der Befehl `open_door` ausgeführt wird.

### Second Entrance Door Pre-Open-Telegrams <Badge type="tip" text="second_entrance_door_before_open_cmds" /> <Badge type="info" text="Deaktiviert" />
Gib eine Liste von Befehlen an (durch Semikolons getrennt), die auf den Bus gesendet werden, bevor der Befehl `open_door` ausgeführt wird.


## Dropdowns

### Door Opener Mode <Badge type="tip" text="door_opener_mode" /> <Badge type="info" text="Deaktiviert" />
Konfiguriere, wie deine Eingangstür geöffnet werden soll — entweder durch das Senden eines BUS-Befehls oder durch das Schalten des internen Doorman-Relais.

##### Modi:
- **Bus Telegram**
- **Internal Relay**

### RTO: Central Toggle - Door Selection <Badge type="tip" text="rto_central_doors" />
Legt die Türen fest, bei denen der zentrale `Ring To Open`-Schalter die [Ring To Open](../guide/features/ring-to-open) Automation umschaltet.

##### Modi:
- **Entrance**
- **Second Entrance**
- **Any Entrance**
- **Apartment**
- **Any Entrance & Apartment**

### RTO: Entrance Door - Delay <Badge type="tip" text="rto_entrance_door_delay" /> <Badge type="info" text="Deaktiviert" />
Dieser Wert legt die **Verzögerung zum Türöffnen** fest, die von der [Ring To Open](../guide/features/ring-to-open)-Automation verwendet wird.
Um Störungen bei der Erkennung von Klingelmustern zu vermeiden, beginnt die Verzögerung standardmäßig bei **2 Sekunden** – außer du verwendest das **`single`**-Klingelmuster, das auch kürzere Zeiten erlaubt.

##### Verfügbare Optionen:
* **0–1 seconds** → nur gültig mit dem **`single`**-Klingelmuster
* **2–25 seconds**
* **Random** → wählt automatisch einen gültigen Wert innerhalb des Bereichs

### RTO: Second Entrance Door - Delay <Badge type="tip" text="rto_second_entrance_door_delay" /> <Badge type="info" text="Deaktiviert" />
Dieser Wert legt die **Verzögerung zum Türöffnen** fest, die von der [Ring To Open](../guide/features/ring-to-open)-Automation verwendet wird.
Um Störungen bei der Erkennung von Klingelmustern zu vermeiden, beginnt die Verzögerung standardmäßig bei **2 Sekunden** – außer du verwendest das **`single`**-Klingelmuster, das auch kürzere Zeiten erlaubt.

##### Verfügbare Optionen:
* **0–1 seconds** → nur gültig mit dem **`single`**-Klingelmuster
* **2–25 seconds**
* **Random** → wählt automatisch einen gültigen Wert innerhalb des Bereichs

### RTO: Apartment Door - Delay <Badge type="tip" text="rto_apartment_door_delay" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Dieser Wert legt die **Verzögerung zum Türöffnen** fest, die von der [Ring To Open](../guide/features/ring-to-open)-Automation verwendet wird.
Um Störungen bei der Erkennung von Klingelmustern zu vermeiden, beginnt die Verzögerung standardmäßig bei **2 Sekunden** – außer du verwendest das **`single`**-Klingelmuster, das auch kürzere Zeiten erlaubt.

##### Verfügbare Optionen:
* **0–1 seconds** → nur gültig mit dem **`single`**-Klingelmuster
* **2–25 seconds**
* **Random** → wählt automatisch einen gültigen Wert innerhalb des Bereichs

### RTO: Entrance Door - Timeout <Badge type="tip" text="rto_entrance_door_timeout_mode" />
Legt den Timeout-Modus für die [Ring To Open](../guide/features/ring-to-open) Automation fest.

##### Modi:
- **Ring once**
- **5...60 Minutes**
- **Never**

### RTO: Second Entrance Door - Timeout <Badge type="tip" text="rto_second_entrance_door_timeout_mode" /> <Badge type="info" text="Deaktiviert" />
Legt den Timeout-Modus für die [Ring To Open](../guide/features/ring-to-open) Automation fest.

##### Modi:
- **Ring once**
- **5...60 Minutes**
- **Never**

### RTO: Apartment Door - Timeout <Badge type="tip" text="rto_apartment_door_timeout_mode" /> <Badge type="info" text="Nuki Bridge" />
Legt den Timeout-Modus für die [Ring To Open](../guide/features/ring-to-open) Automation fest.

##### Modi:
- **Ring once**
- **5...60 Minutes**
- **Never**

### RTO: Entrance Door - Pattern Condition <Badge type="tip" text="rto_entrance_door_pattern_condition" /> <Badge type="info" text="Deaktiviert" />
Legt das Muster fest, welches Bedingung zum auslösen der [Ring To Open](../guide/features/ring-to-open) automation fest.

##### Options:
- **single**
- **double**
- **triple**
- **quadruple**

### RTO: Second Entrance Door - Pattern Condition <Badge type="tip" text="rto_second_entrance_door_pattern_condition" /> <Badge type="info" text="Deaktiviert" />
Legt das Muster fest, welches Bedingung zum auslösen der [Ring To Open](../guide/features/ring-to-open) automation fest.

##### Options:
- **single**
- **double**
- **triple**
- **quadruple**

### RTO: Apartment Door - Pattern Condition <Badge type="tip" text="rto_apartment_door_pattern_condition" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Legt das Muster fest, welches Bedingung zum auslösen der [Ring To Open](../guide/features/ring-to-open) automation fest.

##### Options:
- **single**
- **double**
- **triple**
- **quadruple**

### RTO: Central Toggle - Trigger <Badge type="tip" text="rto_toggle_trigger" /> <Badge type="info" text="Deaktiviert" />
Legt den Auslöser zum umschalten für die [Ring To Open](../guide/features/ring-to-open) automation fest.

##### Optionen:
- **Manual**
- **Function Button**
- **External Button**

### Indoor Station Model <Badge type="tip" text="indoor_station_model" /> <Badge type="info" text="Deaktiviert" />
Legt das Modell deiner Innenstation fest. Finde mehr über die [unterstützten Modelle und Einstellungen](esphome-component#model-setting-availability) heraus.

### Ringtone: Entrance Door Call <Badge type="tip" text="indoor_station_ringtone_entrance_door_call" /> <Badge type="info" text="Deaktiviert" />
Legt den Klingelton für Tür-Rufe (Eingang) deiner Innenstation fest.

##### Optionen:
- **Ringtone 1 ... 13**

### Ringtone: Second Entrance Door Call <Badge type="tip" text="indoor_station_ringtone_second_entrance_door_call" /> <Badge type="info" text="Deaktiviert" />
Legt den Klingelton für Tür-Rufe (Zweiter Eingang) deiner Innenstation fest.

##### Optionen:
- **Ringtone 1 ... 13**

### Ringtone: Floor Call <Badge type="tip" text="indoor_station_ringtone_floor_call" /> <Badge type="info" text="Deaktiviert" />
Legt den Klingelton für Etagen-Rufe deiner Innenstation fest.

##### Optionen:
- **Ringtone 1 ... 13**

### Ringtone: Internal Call <Badge type="tip" text="indoor_station_ringtone_internal_call" /> <Badge type="info" text="Deaktiviert" />
Legt den Klingelton für Intern-Rufe deiner Innenstation fest.

##### Optionen:
- **Ringtone 1 ... 13**

### Nuki Button: Single Press Action <Badge type="tip" text="nuki_single_button_press_action" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Single Button Press Action setting.

##### Optionen:
- **No Action**
- **Intelligent**
- **Unlock**
- **Lock**
- **Unlatch**
- **Lock n Go**
- **Show Status**

### Nuki Button: Double Press Action <Badge type="tip" text="nuki_double_button_press_action" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Double Button Press Action setting.

##### Optionen:
- **No Action**
- **Intelligent**
- **Unlock**
- **Lock**
- **Unlatch**
- **Lock n Go**
- **Show Status**

### Nuki Fob: Action 1 <Badge type="tip" text="nuki_fob_action_1" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Fob Action 1 setting.

##### Optionen:
- **No Action**
- **Unlock**
- **Lock**
- **Lock n Go**
- **Intelligent**

### Nuki Fob: Action 2 <Badge type="tip" text="nuki_fob_action_2" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Fob Action 2 setting.

##### Optionen:
- **No Action**
- **Unlock**
- **Lock**
- **Lock n Go**
- **Intelligent**

### Nuki Fob: Action 3 <Badge type="tip" text="nuki_fob_action_3" /> <Badge type="info" text="Nuki Bridge" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Fob Action 3 setting.

##### Optionen:
- **No Action**
- **Unlock**
- **Lock**
- **Lock n Go**
- **Intelligent**

### Nuki Advertising Mode <Badge type="tip" text="nuki_advertising_mode" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Deaktiviert" />
Controls the Nuki Smart Lock Advertising Mode setting.

##### Optionen:
- **Automatic**
- **Normal**
- **Slow**
- **Slowest**


## Schlösser

### Entrance Door <Badge type="tip" text="entrance_door" />
Repräsentiert die Eingangstür von dem Gebäude. Es werden nur die `Öffnen`- und `Aufschließen`-Aktionen unterstützt.

### Second Entrance Door <Badge type="tip" text="second_entrance_door" /> <Badge type="info" text="Deaktiviert" />
Repräsentiert die zweite Eingangstür von dem Gebäude. Es werden nur die `Öffnen`- und `Aufschließen`-Aktionen unterstützt.

### Apartment Door <Badge type="tip" text="apartment_door" /> <Badge type="info" text="Nuki Bridge" />
Repräsentiert das gekoppelte Nuki Smart Lock deiner Wohnungs- oder Haustür.


## Ereignisse

### Entrance Doorbell <Badge type="tip" text="entrance_doorbell_pattern" />
Wird ausgelöst, wenn ein Klingelmuster an der Eingangstür erkannt wird. Erfahre mehr über Musterereignisse [hier](../guide/features/pattern-events).

##### Ereignistypen
- **single**
- **double**
- **triple**
- **quadruple**

### Second Entrance Doorbell <Badge type="tip" text="second_entrance_doorbell_pattern" />
Wird ausgelöst, wenn ein Klingelmuster an der zweiten Eingangstür erkannt wird. Erfahre mehr über Musterereignisse [hier](../guide/features/pattern-events).

##### Ereignistypen
- **single**
- **double**
- **triple**
- **quadruple**

### Apartment Doorbell <Badge type="tip" text="apartment_doorbell_pattern" />
Wird ausgelöst, wenn ein Klingelmuster an der Wohnungstür erkannt wird. Erfahre mehr über Musterereignisse [hier](../guide/features/pattern-events).

##### Ereignistypen
- **single**
- **double**
- **triple**
- **quadruple**

### Phone pick up <Badge type="tip" text="phone_pick_up_pattern" />
Wird ausgelöst, wenn ein Abhebe-Muster des Telefons der Innenstation erkannt wird. Erfahre mehr über Musterereignisse [hier](../guide/features/pattern-events).

##### Ereignistypen
- **single**
- **double**
- **triple**
- **quadruple**

## Updates

### Doorman Firmware Update <Badge type="tip" text="update_http_request" />
Zeigt an, ob ein Doorman Firmware Update verfügbar ist, und bietet eine Installation über das HTTP OTA-Updateverfahren an.


## Lichter

### Status LED <Badge type="danger" text="Red" /> <Badge type="tip" text="doorman_status_led" />
Eine kleine LED auf der Doorman-Platine, die vom ESPHome [Status LED](https://esphome.io/components/light/status_led.html) gesteuert wird. Standardmäßig bleibt diese LED eingeschaltet, wenn alles ordnungsgemäß funktioniert, blinkt jedoch bei Problemen.

Du kannst die LED ausschalten, wenn sie störend wirkt. Selbst im ausgeschalteten Zustand behält sie ihre Funktionalität und blinkt, um auf Probleme hinzuweisen.

### Status LED <Badge type="warning" text="RGB" /> <Badge type="tip" text="doorman_rgb_status_led" />
Eine kleine WS2812B RGB-LED auf der Doorman-Platine, die verwendet wird, um bestimmte Ereignisse anzuzeigen. Die LED kann verschiedene Farben und Muster anzeigen, um visuelles Feedback zu unterschiedlichen Zuständen und Aktionen zu geben.

##### Ereignistypen
- **Warten auf WiFi / AP aktiviert** <Badge type="tip" text="Blinkt schnell orange" />
- **Warten auf Home Assistant / MQTT / HomeKit Verbindung** <Badge type="tip" text="Pulsiert langsam blau" />
- **Home Assistant / MQTT / HomeKit verbunden** <Badge type="tip" text="Durchgängig blau (für 3 sekunden)" />
- **MQTT Einrichtung ausstehend** <Badge type="tip" text="Pulsiert rot-blau" /> <Badge type="info" text="MQTT Integration" />
- **HomeKit Kopplung ausstehend** <Badge type="tip" text="Pulsiert rot-blau" /> <Badge type="info" text="HomeKit Integration" />
- **Setup Mode ist aktiv** <Badge type="tip" text="Blinkt schnell grün-türkis" />
- **Setup abgeschlossen** <Badge type="tip" text="Durchgängig grün-türkis (für 3 sekunden)" />
- **Ring To Open ist aktiv** <Badge type="tip" text="Pulsiert langsam gelb" />
- **Nuki Kopplungsmodus ist aktiv** <Badge type="tip" text="Pulsiert langsam lila" /> <Badge type="info" text="Nuki Bridge" />
- **Nuki erfolgreich gekoppelt** <Badge type="tip" text="Durchgängig lila (für 3 sekunden)" /> <Badge type="info" text="Nuki Bridge" />
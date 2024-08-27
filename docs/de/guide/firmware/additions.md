## Beispiele

::: details Einfachen TCS-Befehl-Binärsensor erstellen
Neben den bereits vordefinierten kannst du ganz leicht zusätzliche Binärsensoren für jeden TCS-Befehl hinzufügen.
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++] // [!code focus]
  - platform: tcs_intercom // [!code ++] // [!code focus]
    name: "Benutzerdefinierter Befehl" // [!code ++] // [!code focus]
    command: 0x3b8f9a00 // [!code ++] // [!code focus]
```
:::

::: details Internes RGB-Status-LED steuern
Wenn du die RGB-LED am Doorman beispielsweise mit einem Button steuern möchtest, verwende einfach die Light ID: `doorman_rgb_status_led`.
```yaml
<!--@include: minimal.example.yaml-->

button: // [!code ++] // [!code focus]
  - platform: template // [!code ++] // [!code focus]
    name: "Status RGB-LED auf Rot setzen" // [!code ++] // [!code focus]
    on_press: // [!code ++] // [!code focus]
      - light.turn_on: // [!code ++] // [!code focus]
          id: doorman_rgb_status_led // [!code ++] // [!code focus]
          red: 100% // [!code ++] // [!code focus]
          green: 0% // [!code ++] // [!code focus]
          blue: 0% // [!code ++] // [!code focus]
```
:::

::: details Externen Hardware-Button verwenden
Wenn du den externen Button nutzen möchtest, um Automationen auszulösen, kannst du deine YAML-Konfiguration einfach erweitern.
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++] // [!code focus]
  - id: !extend doorman_external_button // [!code ++] // [!code focus]
    on_press: // [!code ++] // [!code focus]
      - logger.log: "Externer Button gedrückt!" // [!code ++] // [!code focus]
```
:::

::: details I²C-Sensoren nutzen
Falls du Sensoren über den I²C-Bus hinzufügen möchtest, kannst du die beiden verfügbaren GPIO-Pins nutzen und die I²C-Komponente in deine Konfiguration einbinden. Dann kannst du deine Sensoren an diese beiden I²C-GPIO-Pins anschließen.
```yaml
<!--@include: minimal.example.yaml-->

i2c: // [!code ++] // [!code focus]
  sda: GPIO40 // [!code ++] // [!code focus]
  scl: GPIO48 // [!code ++] // [!code focus]
  scan: true // [!code ++] // [!code focus]
  id: i2c_bus // [!code ++] // [!code focus]
```
:::


## Fortgeschrittene Beispiele
### Home Assistant
::: details Bus-Commands senden
Mit Home Assistant kannst du Aktionen nutzen, um Commands über den Bus zu senden.
> [!INFO]
> Denk daran, das führende `0x` beim Senden eines HEX-Kommandos einzufügen. Wenn du es weglässt, musst du den HEX-Befehl in eine Dezimalzahl umwandeln.

```yaml
service: esphome.doorman_s3_send_tcs_command
data:
  command: 0x3a001100
```
:::

::: details Auf ESPHome-Ereignisse hören
Der Doorman sendet `esphome.doorman`-Ereignisse an Home Assistant, jedes Mal wenn ein Befehl empfangen wird.

Jedes Ereignis sieht so aus:
```yaml
event_type: esphome.doorman
data:
  device_id: 373c62d6788cf81d322763235513310e
  command: "00001100"
origin: LOCAL
time_fired: "2024-08-12T12:34:13.718317+00:00"
context:
  id: 01J5399Y2PP2XS2VRYKBT3H3AV
  parent_id: null
  user_id: null
```

Beispiel für eine Home Assistant-Automation:
```yaml
alias: Bei Doorman TCS Türöffnungsbefehl auslösen
description: ""
trigger:
  - platform: event
    event_type: esphome.doorman
    event_data:
      command: "00001100"
condition: []
action: []
mode: single
```
:::

### ESPHome
::: details Einen zur Laufzeit konfigurierbaren TCS-Befehl-Binärsensor erstellen
Du kannst zusätzliche konfigurierbare Befehl-Binärsensoren hinzufügen, indem du Lambda, Globals und Texteingaben verwendest.

```yaml
<!--@include: minimal.example.yaml-->

globals: // [!code ++] // [!code focus]
  - id: custom_command // [!code ++] // [!code focus]
    type: int // [!code ++] // [!code focus]
    restore_value: true // [!code ++] // [!code focus]
    initial_value: '0x3b8f9a00' // [!code ++] // [!code focus]

text: // [!code ++] // [!code focus]
  - platform: template // [!code ++] // [!code focus]
    id: custom_command_input // [!code ++] // [!code focus]
    name: Benutzerdefinierter Befehl // [!code ++] // [!code focus]
    entity_category: CONFIG // [!code ++] // [!code focus]
    icon: "mdi:console-network" // [!code ++] // [!code focus]
    mode: text // [!code ++] // [!code focus]
    lambda: |- // [!code ++] // [!code focus]
      unsigned long number = id(custom_command); // [!code ++] // [!code focus]
      return str_upper_case(format_hex(number)); // [!code ++] // [!code focus]
    set_action: // [!code ++] // [!code focus]
      then: // [!code ++] // [!code focus]
        - lambda: |- // [!code ++] // [!code focus]
            x.erase(std::remove_if(x.begin(), x.end(), [](char c) { return !std::isxdigit(c); }), x.end()); // [!code ++] // [!code focus]
            x.erase(0, x.find_first_not_of('0')); // [!code ++] // [!code focus]
            x.resize(8); // [!code ++] // [!code focus]
            unsigned long number = 0; // [!code ++] // [!code focus]
            if(std::string(x.c_str()) != "") { // [!code ++] // [!code focus]
              number = std::stoul(x.c_str(), nullptr, 16); // [!code ++] // [!code focus]
            } // [!code ++] // [!code focus]
            id(custom_command) = number; // [!code ++] // [!code focus]
            id(custom_command_input)->publish_state(str_upper_case(format_hex(number))); // [!code ++] // [!code focus]

binary_sensor: // [!code ++] // [!code focus]
  - platform: tcs_intercom // [!code ++] // [!code focus]
    name: "Benutzerdefinierter Befehl" // [!code ++] // [!code focus]
    lambda: !lambda "return id(custom_command);" // [!code ++] // [!code focus]
```
:::

::: details Einen Bus-Spannungssensor erstellen
Du kannst einen Bus-Spannungssensor für ältere Gegensprechanlagen hinzufügen, die mit 14-24V DC arbeiten.\
Es könnte in Zukunft auch möglich sein, andere Protokolle zu implementieren.
```yaml
<!--@include: minimal.example.yaml-->

# Neuer ADC-Spannungssensor // [!code ++] // [!code focus]
sensor: // [!code ++] // [!code focus]
  - platform: adc // [!code ++] // [!code focus]
    id: bus_voltage // [!code ++] // [!code focus]
    name: Bus-Spannung // [!code ++] // [!code focus]
    pin: // [!code ++] // [!code focus]
      number: GPIO9 // [!code ++] // [!code focus]
      allow_other_uses: true // [!code ++] // [!code focus]
    update_interval: 500ms // [!code ++] // [!code focus]
    attenuation: 12dB // [!code ++] // [!code focus]

# TCS-Intercom-Komponente erweitern // [!code ++] // [!code focus]
# RX-Pin auch für andere Zwecke nutzen // [!code ++] // [!code focus]
tcs_intercom: // [!code ++] // [!code focus]
  rx_pin: // [!code ++] // [!code focus]
    number: GPIO9 // [!code

 ++] // [!code focus]
    allow_other_uses: true // [!code ++] // [!code focus]
```
:::

::: details Eigene Klingel-Muster erstellen
Wenn du ein benutzerdefiniertes Klingelmuster erstellen möchtest, kannst du die bestehenden Klingel-Entities einfach erweitern. Weitere Informationen zu Mustern findest du in der [ESPHome Dokumentation](https://esphome.io/components/binary_sensor/index.html#on-multi-click).
```yaml
<!--@include: minimal.example.yaml-->

# Türglocken-Muster-Event-Entity erweitern // [!code ++] // [!code focus]
# Neues apartment_special-Eventtyp hinzufügen // [!code ++] // [!code focus]
event: // [!code ++] // [!code focus]
  - id: !extend doorbell_pattern // [!code ++] // [!code focus]
    event_types: // [!code ++] // [!code focus]
      - "apartment_special" // [!code ++] // [!code focus]

# Die apartment_doorbell / entrance_doorbell-Entity erweitern // [!code ++] // [!code focus]
# und das neue spezielle Muster hinzufügen // [!code ++] // [!code focus]
binary_sensor: // [!code ++] // [!code focus]
  - id: !extend apartment_doorbell // [!code ++] // [!code focus]
    on_multi_click: // [!code ++] // [!code focus]
      # Besonderes Muster // [!code ++] // [!code focus]
      - timing: // [!code ++] // [!code focus]
          # Zwei Mal drücken, mit maximal einer Sekunde zwischen den Drücken. // [!code ++] // [!code focus]
          - ON für maximal 0.5s // [!code ++] // [!code focus]
          - OFF für maximal 1s // [!code ++] // [!code focus]
          - ON für maximal 0.5s // [!code ++] // [!code focus]
          - OFF für mindestens 2s // [!code ++] // [!code focus]
        then: // [!code ++] // [!code focus]
          - logger.log: "Besonderes Muster erkannt!" // [!code ++] // [!code focus]
          - event.trigger: // [!code ++] // [!code focus]
              id: doorbell_pattern // [!code ++] // [!code focus]
              # Den vorher definierten neuen Eventtyp hier verwenden // [!code ++] // [!code focus]
              event_type: apartment_special // [!code ++] // [!code focus]
```
:::

::: details Licht einschalten, wenn jemand an der Eingangstür klingelt
Du kannst das Licht einschalten lassen, wenn jemand an der Eingangstür klingelt.
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++] // [!code focus]
  - id: !extend entrance_doorbell // [!code ++] // [!code focus]
    on_press: // [!code ++] // [!code focus]
      - tcs_intercom.send: // [!code ++] // [!code focus]
          command: !lambda "return id(turn_on_light_command);" // [!code ++] // [!code focus]
```

Wenn du auch die Sonnenhöhe berücksichtigen möchtest, kannst du das entsprechend anpassen.
```yaml
<!--@include: minimal.example.yaml-->

# Home Assistant Sonnenhöhe-Sensor importieren // [!code ++] // [!code focus]
sensor: // [!code ++] // [!code focus]
  - platform: homeassistant // [!code ++] // [!code focus]
    id: sun_elevation // [!code ++] // [!code focus]
    entity_id: sun.sun // [!code ++] // [!code focus]
    attribute: elevation // [!code ++] // [!code focus]

# Eingangstür-Sensor erweitern // [!code ++] // [!code focus]
binary_sensor: // [!code ++] // [!code focus]
  - id: !extend entrance_doorbell // [!code ++] // [!code focus]
    on_press: // [!code ++] // [!code focus]
      # Sonnenhöhe <= 0 (dunkel) // [!code ++] // [!code focus]
      - if: // [!code ++] // [!code focus]
          condition: // [!code ++] // [!code focus]
            sensor.in_range: // [!code ++] // [!code focus]
              id: sun_elevation // [!code ++] // [!code focus]
              below: 1 // [!code ++] // [!code focus]
          then: // [!code ++] // [!code focus]
            # Licht einschalten // [!code ++] // [!code focus]
            - tcs_intercom.send: // [!code ++] // [!code focus]
                command: !lambda "return id(turn_on_light_command);" // [!code ++] // [!code focus]
```
:::
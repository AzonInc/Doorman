## Beispiele

::: details Einfachen TC-Befehl-Binärsensor erstellen
Neben den bereits vordefinierten kannst du ganz leicht zusätzliche Binärsensoren für jeden TC-Befehl hinzufügen.

Command Builder:
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++] // [!code focus]
  - platform: tc_bus // [!code ++] // [!code focus]
    name: "Benutzerdefinierter Befehl" // [!code ++] // [!code focus]
    type: open_door // [!code ++] // [!code focus]
    address: 0 // [!code ++] // [!code focus]
    web_server: // [!code ++] // [!code focus]
      sorting_group_id: sorting_group_listeners // [!code ++] // [!code focus]
```

32-Bit Command:
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++] // [!code focus]
  - platform: tc_bus // [!code ++] // [!code focus]
    name: "Benutzerdefinierter Befehl" // [!code ++] // [!code focus]
    command: 0x00001100 // [!code ++] // [!code focus]
    web_server: // [!code ++] // [!code focus]
      sorting_group_id: sorting_group_listeners // [!code ++] // [!code focus]
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

::: details Sending Bus commands
Mit Home Assistant kannst du Aktionen nutzen, um Commands über den Bus zu senden.
Benutze entweder `command` für reine 32 Bit Befehle oder `type`, `address`, `payload` und `serial_number` um Befehle über den Command Builder zu senden.

> [!INFO]
> Denk an das führende `0x` beim Senden eines Befehls mit der `command` Eigenschaft. Wenn du es weglässt, musst du den HEX-Befehl zuerst in eine Dezimalzahl umwandeln.

Command Builder:
```yaml
service: esphome.doorman_s3_send_tc_command
data:
  type: open_door
  address: 0
  payload: 0
  serial_number: 0
```

32-Bit Befehle via `command`:
```yaml
service: esphome.doorman_s3_send_tc_command_raw
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
  type: "open_door"
  address: "0"
  payload: "0"
  serial_number: "0"
origin: LOCAL
time_fired: "2024-08-12T12:34:13.718317+00:00"
context:
  id: 01J5399Y2PP2XS2VRYKBT3H3AV
  parent_id: null
  user_id: null
```

Beispiel für eine Home Assistant-Automation (HEX Command):
```yaml
alias: Bei Doorman TC Türöffnungsbefehl auslösen
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

Beispiel für eine Home Assistant-Automation (Command Builder):
```yaml
alias: Bei Doorman TC Türöffnungsbefehl auslösen
description: ""
trigger:
  - platform: event
    event_type: esphome.doorman
    event_data:
      type: "open_door"
condition: []
action: []
mode: single
```
:::

### ESPHome
::: details Eigene Klingel-Muster erstellen
Wenn du ein benutzerdefiniertes Klingelmuster erstellen möchtest, kannst du die bestehenden Klingel-Entities einfach erweitern. Weitere Informationen zu Mustern findest du in der [ESPHome Dokumentation](https://esphome.io/components/binary_sensor/index.html#on-multi-click).
```yaml
<!--@include: minimal.example.yaml-->

# Türglocken-Muster-Event-Entity erweitern // [!code ++] // [!code focus]
# Neues apartment_special-Eventtyp hinzufügen // [!code ++] // [!code focus]
event: // [!code ++] // [!code focus]
  - id: !extend doorbell_apartment_pattern // [!code ++] // [!code focus]
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
      - tc_bus.send: // [!code ++] // [!code focus]
          type: "light" // [!code ++] // [!code focus]
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
            - tc_bus.send: // [!code ++] // [!code focus]
                type: "light" // [!code ++] // [!code focus]
```
:::

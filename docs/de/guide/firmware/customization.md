# Anpassungen

Entdecke verschiedene Beispiele, um die Doorman-Firmware anzupassen und zu erweitern. Vom Hinzufügen benutzerdefinierter Sensoren bis hin zum Anpassen von Automatisierungseinstellungen helfen dir diese Beispiele, das Gerät nach deinen individuellen Bedürfnissen zu gestalten. Folge den Anleitungen um das Beste aus deiner Konfiguration herauszuholen.

## Erstelle einen einfachen TC Telegram Binary Sensor
Du kannst problemlos benutzerdefinierte Binärsensoren für jeden TC-Befehl hinzufügen, zusätzlich zu den bereits vorkonfigurierten. Damit kannst du auf jeden anderen Busbefehl hören und darauf reagieren, um die Funktionalität deines Systems zu erweitern.

::: code-group
```yaml [Telegram Builder]
<!--@include: minimal.example.yaml-->

binary_sensor: # [!code ++] [!code focus]
  - platform: tc_bus # [!code ++] [!code focus]
    name: "Custom Telegram" # [!code ++] [!code focus]
    type: open_door # [!code ++] [!code focus]
    address: 0 # [!code ++] [!code focus]
    web_server: # [!code ++] [!code focus]
      sorting_group_id: sorting_group_listeners # [!code ++] [!code focus]
```
```yaml [32-Bit Telegrams]
<!--@include: minimal.example.yaml-->

binary_sensor: # [!code ++] [!code focus]
  - platform: tc_bus # [!code ++] [!code focus]
    name: "Custom Telegram" # [!code ++] [!code focus]
    telegram: 0x00001100 # [!code ++] [!code focus]
    web_server: # [!code ++] [!code focus]
      sorting_group_id: sorting_group_listeners # [!code ++] [!code focus]
```
:::

## Steuere die interne RGB-Status-LED
Um die Onboard-RGB-LED mit einem Button (zum Beispiel) zu steuern, kannst du einfach die Licht-Entität mit der internen ID: `doorman_rgb_status_led` referenzieren.

```yaml
<!--@include: minimal.example.yaml-->

button: # [!code ++] [!code focus]
  - platform: template # [!code ++] [!code focus]
    name: "Turn on Status RGB LED to red" # [!code ++] [!code focus]
    on_press: # [!code ++] [!code focus]
      - light.turn_on: # [!code ++] [!code focus]
          id: doorman_rgb_status_led # [!code ++] [!code focus]
          red: 100% # [!code ++] [!code focus]
          green: 0% # [!code ++] [!code focus]
          blue: 0% # [!code ++] [!code focus]
```

## Verwende den externen Hardware-Button
Wenn du den externen Button verwenden möchtest, um Automatisierungen auszulösen, erweitere den Block `on_press` und füge deine eigenen Aktionen hinzu.

```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: # [!code ++] [!code focus]
  - id: !extend doorman_external_button # [!code ++] [!code focus]
    on_press: # [!code ++] [!code focus]
      - logger.log: "External button pressed!" # [!code ++] [!code focus]
```

## Verwende I²C-Sensoren
Falls du Sensoren über den I²C-Bus hinzufügen möchtest, kannst du die beiden verfügbaren GPIO-Pins verwenden und die I²C-Komponente zu deiner Konfiguration hinzufügen. Du kannst dann deine Sensoren an diese beiden I²C-GPIO-Pins anschließen.

```yaml
<!--@include: minimal.example.yaml-->

i2c: # [!code ++] [!code focus]
  sda: GPIO40 # [!code ++] [!code focus]
  scl: GPIO48 # [!code ++] [!code focus]
  scan: true # [!code ++] [!code focus]
  id: i2c_bus # [!code ++] [!code focus]
```


## Erstelle dein eigenes Klingelmuster
Wenn du ein benutzerdefiniertes Klingelmuster erstellen möchtest, kannst du die vorhandenen Klingel-Entities ganz einfach erweitern. Weitere Informationen zu Mustern findest du in der [ESPHome-Dokumentation](https://esphome.io/components/binary_sensor/index.html#on-multi-click).

```yaml
<!--@include: minimal.example.yaml-->

# Erweitere die Klingelmuster-Event-Entität // [!code ++] [!code focus]
# Füge einen neuen speziellen Ereignistyp hinzu // [!code ++] [!code focus]
event: # [!code ++] [!code focus]
  - id: !extend apartment_doorbell_pattern # [!code ++] [!code focus]
    event_types: # [!code ++] [!code focus]
      - "special" # [!code ++] [!code focus]

# Erweitere die Ring-To-Open-Musterbedingungen // [!code ++] [!code focus]
# Du musst jede Türmuster-Bedingung erweitern, bei der du das Muster verwenden möchtest // [!code ++] [!code focus]
select: # [!code ++] [!code focus]
  - id: !extend rto_apartment_door_pattern_condition # [!code ++] [!code focus]
    options: # [!code ++] [!code focus]
      - "special" # [!code ++] [!code focus]

# Erweitere die apartment_doorbell / entrance_doorbell-Entity // [!code ++] [!code focus]
# und füge dein neues spezielles Muster hinzu // [!code ++] [!code focus]
binary_sensor: # [!code ++] [!code focus]
  - id: !extend apartment_doorbell # [!code ++] [!code focus]
    on_multi_click: # [!code ++] [!code focus]
      # Spezielles Muster // [!code ++] [!code focus]
      - timing: # [!code ++] [!code focus]
          # Drücke zweimal schnell mit höchstens einer halben Sekunde dazwischen. // [!code ++] [!code focus]
          - ON for at most 0.3s # [!code ++] [!code focus]
          - OFF for at most 0.5s # [!code ++] [!code focus]
          - ON for at most 0.3s # [!code ++] [!code focus]
          - OFF for 1s to 1.5s # [!code ++] [!code focus]
          # Drücke einmal langsam mit höchstens 1,5 Sekunden dazwischen. // [!code ++] [!code focus]
          - ON for at most 0.3s # [!code ++] [!code focus]
          - OFF for 1s to 1.5s # [!code ++] [!code focus]
          # Drücke einmal. // [!code ++] [!code focus]
          - ON for at most 0.3s # [!code ++] [!code focus]
          - OFF for at least 2s # [!code ++] [!code focus]
        then: # [!code ++] [!code focus]
          - logger.log: "Spezielles Muster erkannt!" # [!code ++] [!code focus]
          - event.trigger: # [!code ++] [!code focus]
              id: apartment_doorbell_pattern # [!code ++] [!code focus]
              # Verwende hier den vorher definierten neuen Ereignistyp // [!code ++] [!code focus]
              event_type: special # [!code ++] [!code focus]
```

## Licht an, wenn die Eingangsklingel betätigt wird
Du kannst eine Automatisierung einrichten, die das Licht einschaltet, wenn jemand die Eingangsklingel betätigt. Dies ist nützlich, um es leichter zu machen, zu sehen, wer an der Tür ist. Konfiguriere einfach die Automatisierung, um das Licht basierend auf dem Klingelereignis einzuschalten.

::: code-group
```yaml [Basic]
<!--@include: minimal.example.yaml-->

binary_sensor: # [!code ++] [!code focus]
  - id: !extend entrance_doorbell # [!code ++] [!code focus]
    on_press: # [!code ++] [!code focus]
      - tc_bus.send: # [!code ++] [!code focus]
          type: "light" # [!code ++] [!code focus]
```
```yaml [Basierend auf der Sonnenhöhe]
<!--@include: minimal.example.yaml-->

# Importiere den Home Assistant-Sonnenhöhen-Sensor // [!code ++] [!code focus]
sensor: # [!code ++] [!code focus]
  - platform: homeassistant # [!code ++] [!code focus]
    id: sun_elevation # [!code ++] [!code focus]
    entity_id: sun.sun # [!code ++] [!code focus]
    attribute: elevation # [!code ++] [!code focus]

# Erweitere den Eingangsklingelsensor // [!code ++] [!code focus]
binary_sensor: # [!code ++] [!code focus]
  - id: !extend entrance_doorbell # [!code ++] [!code focus]
    on_press: # [!code ++] [!!code focus]
      # Sonnenhöhe <= 0 (dunkel) // [!code ++] [!code focus]
      - if: # [!code ++] [!code focus]
          condition: # [!code ++] [!code focus]
            sensor.in_range: # [!code ++] [!code focus]
              id: sun_elevation # [!code ++] [!code focus]
              below: 1 # [!code ++] [!code focus]
          then: # [!code ++] [!code focus]
            # Schalte das Licht ein // [!code ++] [!code focus]
            - tc_bus.send: # [!code ++] [!code focus]
                type: "light" # [!code ++] [!code focus]
```
:::
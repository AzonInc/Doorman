# Muster-Ereignisse

Die Doorman-Firmware enthält Muster-Ereignis-Entitäten, die du in [Home Assistant](https://www.home-assistant.io/) Automatisierungen verwenden kannst.

Jedes Muster ist wie folgt konfiguriert:
- Drücken
- Maximal 1 Sekunde Pause
- Drücken

Sieh dir die [fortgeschrittenen Beispiele](../firmware/stock-firmware#fortgeschrittene-beispiele) an, um zu lernen, wie du eigene Muster erstellen und anpassen kannst.

## Klingelmuster

### Ereignis Sensoren
- Entrance Doorbell <Badge type="tip" text="entrance_doorbell" />
- Second Entrance Doorbell <Badge type="tip" text="second_entrance_doorbell" />
- Apartment Doorbell <Badge type="tip" text="apartment_doorbell" />

### Ereignistypen
- single
- double
- triple

### Beispiel-Automatisierung
::: details Tür automatisch öffnen, wenn die Eingangsklingel zweimal in einem bestimmten Muster gedrückt wird.
Stelle sicher, dass du die `entity_id` durch deine spezifischen Doorman-Entitäten ersetzt.
```yaml
alias: Eingangstür automatisch öffnen
description: "Öffne die Eingangstür, nachdem die Eingangsklingel zweimal gedrückt wurde."
trigger:
  - platform: state
    entity_id:
      - event.doorman_s3_entrance_doorbell
    attribute: event_type
    to: double
condition: []
action:
  - service: button.press
    metadata: {}
    data: {}
    target:
        entity_id: button.doorman_s3_open_entrance_door
mode: single
```
:::

## Phone Hang up-Muster

### Ereignistypen
- single
- double
- triple

### Beispiel-Automatisierung
::: details Ring-To-Open-Modus umschalten, indem du schnell dreimal den Telefonhörer auflegst.
Stelle sicher, dass du die `entity_id` durch deine spezifischen Doorman-Entitäten ersetzt.
```yaml
alias: Ring-To-Open-Modus umschalten (Telefon-Auflegen-Muster)
description: "Schalte den Ring-To-Open-Modus um, wenn du schnell dreimal den Telefonhörer auflegst."
trigger:
  - platform: state
    entity_id:
      - event.doorman_s3_phone_pick_up
    attribute: event_type
    to: triple
condition: []
action:
  - service: switch.toggle
    metadata: {}
    data: {}
    target:
        entity_id: switch.doorman_s3_ring_to_open
mode: single
```
:::

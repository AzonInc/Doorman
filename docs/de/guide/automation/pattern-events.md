# Muster-Ereignisse

Die Doorman-Firmware enthält Muster-Ereignis-Entitäten, die du in [Home Assistant](https://www.home-assistant.io/) Automatisierungen verwenden kannst.

Jedes Muster ist wie folgt konfiguriert:
- Drücken
- Maximal 1 Sekunde Pause
- Drücken

Sieh dir die [fortgeschrittenen Beispiele](../firmware/stock-firmware#fortgeschrittene-beispiele) an, um zu lernen, wie du eigene Muster erstellen und anpassen kannst.

## Klingelmuster

### Ereignistypen
- apartment_single
- apartment_double
- apartment_triple
- entrance_single
- entrance_double
- entrance_triple
- second_entrance_single
- second_entrance_double
- second_entrance_triple

### Beispiel-Automatisierung
::: details Tür automatisch öffnen, wenn die Eingangsklingel zweimal in einem bestimmten Muster gedrückt wird.
Stelle sicher, dass du die `entity_id` durch deine spezifischen Doorman-Entitäten ersetzt.
```yaml
alias: Eingangstür automatisch öffnen
description: "Öffne die Eingangstür, nachdem die Eingangsklingel zweimal gedrückt wurde."
trigger:
  - platform: state
    entity_id:
      - event.doorman_s3_doorbell_pattern
    attribute: event_type
    to: entrance_double
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

## Telefon-Auflegen-Muster

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
      - event.doorman_s3_phone_pick_up_pattern
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
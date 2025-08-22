# Muster-Ereignisse

Die Doorman-Firmware enthält Muster-Ereignis-Entitäten, die du in [Home Assistant](https://www.home-assistant.io/) Automatisierungen verwenden kannst.

Jedes Muster ist wie folgt konfiguriert:
- Drücken
- Maximal 1 Sekunde Pause
- Drücken

Sieh dir das [Beispiel](../firmware/customization#erstelle-dein-eigenes-klingelmuster) an, um zu lernen, wie du eigene Muster erstellen und anpassen kannst.

## Klingelmuster

### Ereignis Sensoren
- Entrance Doorbell <Badge type="tip" text="entrance_doorbell_pattern" />
- Second Entrance Doorbell <Badge type="tip" text="second_entrance_doorbell_pattern" />
- Apartment Doorbell <Badge type="tip" text="apartment_doorbell_pattern" />

### Ereignistypen
- **single**
- **double**
- **triple**
- **quadruple**

### Beispiel-Automatisierung
::: details Tür automatisch öffnen, wenn die Eingangsklingel zweimal gedrückt wird.
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
  - service: lock.unlock
    target:
        entity_id: lock.doorman_s3_entrance_door
mode: single
```
:::

## Phone Hang up-Muster

### Ereignistypen
- **single**
- **double**
- **triple**
- **quadruple**

### Beispiel-Automatisierung
::: details Ring-To-Open-Modus umschalten, indem du dreimal den Telefonhörer auflegst.
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
        entity_id: switch.doorman_s3_rto_entrance_door
mode: single
```
:::

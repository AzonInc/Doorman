# Pattern Events

The Doorman Firmware includes pattern event entities which you can use in [Home Assistant](https://www.home-assistant.io/) automations.

## Doorbell Pattern

### Event Types
- apartment_single
- apartment_double
- apartment_triple
- entrance_single
- entrance_double
- entrance_triple

### Example Automation
::: details Automatically open the door when the Entrance Doorbell is pressed 2 times in a certain way.
You have to change the `entity_id` to your Doorman entities.
```yaml
alias: Automatically open the entrance door
description: "Open the entrance door after pressing the entrance doorbell two times."
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

## Phone pickup Pattern

### Event Types
- single
- double
- triple

### Example Automation
::: details Toggle Ring To Open Mode when you quickly pick up the phone 3 times.
You have to change the `entity_id` to your Doorman entities.
```yaml
alias: Toggle Ring To Open Mode (Phone pickup Pattern)
description: "Toggle Ring To Open Mode when you quickly pick up the phone 3 times."
trigger:
  - platform: state
    entity_id:
      - event.doorman_s3_phone_pickup_pattern
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
# Doorbell Events

The [Stock Doorman Firmware](../firmware/stock-firmware.md) includes a `Doorbell Pattern` event entity which offers 6 event types.

You can create automations to trigger specific actions based on each Button Push Pattern.

## Doorbell Pattern Event Types
- apartment_single
- apartment_double
- apartment_triple
- entrance_single
- entrance_double
- entrance_triple

## Example Automation
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
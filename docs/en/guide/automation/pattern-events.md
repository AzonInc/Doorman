# Pattern Events

The Doorman firmware includes pattern event entities that you can use in [Home Assistant](https://www.home-assistant.io/) automations.

Each pattern is configured as follows:
- Push
- Maximum 1-second break
- Push

Check out the [advanced examples](../firmware/stock-firmware#advanced-examples) to learn how to create and customize your own patterns.

## Doorbell Pattern

### Event (Sensors)
- doorbell_entrance_pattern
- doorbell_second_entrance_pattern
- doorbell_apartment_pattern

### Event Types
- single_press
- double_press
- triple_press

### Example Automation
::: details Automatically open the door when the Entrance Doorbell is pressed twice in a specific pattern.
Make sure to replace the `entity_id` with your specific Doorman entities.
```yaml
alias: Automatically open the entrance door
description: "Open the entrance door after pressing the entrance doorbell two times."
trigger:
  - platform: state
    entity_id:
      - event.doorman_s3_doorbell_entrance_pattern
    attribute: event_type
    to: double_press
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
- single_press
- double_press
- triple_press

### Example Automation
::: details Toggle Ring To Open Mode by quickly picking up the phone three times.
Make sure to replace the `entity_id` with your specific Doorman entities.
```yaml
alias: Toggle Ring To Open Mode (Phone pick up Pattern)
description: "Toggle Ring To Open Mode when you quickly pick up the phone 3 times."
trigger:
  - platform: state
    entity_id:
      - event.doorman_s3_phone_pick_up_pattern
    attribute: event_type
    to: triple_press
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

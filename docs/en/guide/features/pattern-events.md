# Pattern Events

The Doorman firmware includes pattern event entities that you can use in [Home Assistant](https://www.home-assistant.io/) automations.

Each pattern is configured as follows:
- Push
- Maximum 1-second break
- Push

Check out the [example](../firmware/customization#create-your-own-doorbell-pattern) to learn how to create and customize your own patterns.

## Doorbell Pattern

### Event Sensors
- Entrance Doorbell <Badge type="tip" text="entrance_doorbell_pattern" />
- Second Entrance Doorbell <Badge type="tip" text="second_entrance_doorbell_pattern" />
- Apartment Doorbell <Badge type="tip" text="apartment_doorbell_pattern" />

### Event Types
- **single**
- **double**
- **triple**
- **quadruple**

### Example Automation
::: details Automatically open the door when the Entrance Doorbell is pressed twice.
Make sure to replace the `entity_id` with your specific Doorman entities.
```yaml
alias: Automatically open the entrance door
description: "Open the entrance door after pressing the entrance doorbell two times."
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

## Phone pickup Pattern

### Event Types
- **single**
- **double**
- **triple**
- **quadruple**

### Example Automation
::: details Toggle Ring To Open Mode by quickly picking up the phone three times.
Make sure to replace the `entity_id` with your specific Doorman entities.
```yaml
alias: Toggle Ring To Open Mode (Phone pick up Pattern)
description: "Toggle Ring To Open Mode when you quickly pick up the phone 3 times."
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
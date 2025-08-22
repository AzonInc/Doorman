# Home Assistant Integration

Discover practical examples for integrating and automating Doorman with Home Assistant, from basic setups to advanced configurations.

## Add Doorman to Home Assistant
<!--@include: ./instructions/home-assistant.md-->

## Sending Bus telegrams
You can use Home Assistant actions (formerly known as services) to send telegrams on the bus.
Either use the `telegram` to send raw telegrams or `type`, `address`, `payload` and `serial_number` to send telegrams via the telegram builder.

::: tip
Remember to include the leading `0x` when calling the action with the `telegram` property. If you omit it, you'll need to convert the HEX telegram to a decimal number first.
:::

::: code-group
```yaml [Telegram Builder]
service: esphome.doorman_s3_send_tc_telegram
data:
  type: open_door
  address: 0
  payload: 0
  serial_number: 123456
```
```yaml [HEX Telegrams]
service: esphome.doorman_s3_send_tc_telegram_raw
data:
  telegram: 0x11E24080
```
:::

## Listening for ESPHome events
Doorman will send esphome.doorman events to Home Assistant every time a telegram is received.

Each event is structured as follows:
```yaml
event_type: esphome.doorman
data:
  device_id: 373c62d6788cf81d322763235513310e
  telegram: "00001100"
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

Automation Example YAML:
::: code-group
```yaml [Telegram Builder]
alias: Trigger on Doorman TC Open Door Telegram
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
```yaml [HEX Telegrams]
alias: Trigger on Doorman TC Open Door Telegram
description: ""
trigger:
  - platform: event
    event_type: esphome.doorman
    event_data:
      telegram: "1100"
condition: []
action: []
mode: single
```
:::

## Blueprint Collection

This is a collection of community-contributed blueprints.  
Feel free to contribute your own blueprints.

### Confirmable Open Door Notification

This blueprint enables you to send a confirmable notification to a device with the Home Assistant app whenever someone rings the doorbell.

The default confirmation action triggers a telegram to open the entrance door, but you can customize it to suit your needs.

[![Open your Home Assistant instance and show the blueprint import dialog with a specific blueprint pre-filled.](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2Fazoninc%2Fdoorman%2Fblob%2Fdev%2Fblueprints%2Fconfirmable_open_door_notification.yaml)

### Open Door on Doorbell Pattern

This blueprint enables you to open the door whenever someone rings the doorbell in the specified pattern.

The default action triggers a telegram to open the entrance door, but you can customize it to suit your needs.

[![Open your Home Assistant instance and show the blueprint import dialog with a specific blueprint pre-filled.](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2Fazoninc%2Fdoorman%2Fblob%2Fdev%2Fblueprints%2Fopen_door_on_pattern.yaml)
## Examples

::: details Handle TCS Commands
You can easily add more binary sensors next to the preconfigured ones for every possible TCS Command.
```yaml
binary_sensor:
  - platform: tcs_intercom
    name: "Apartment Doorbell"
    command: 0x3b8f9a00
```

You can also use Lambda to configure the command dynamically.
```yaml
binary_sensor:
  - platform: tcs_intercom
    name: "Apartment Doorbell"
    lambda: !lambda "return id(entrance_doorbell_command);"
```
:::

::: details Controlling the internal RGB Status LED
If you want to control the onboard RGB LED with a Button (Example) just use the Light ID: doorman_rgb_status_led.
```yaml
button:
  - platform: template
    name: "Turn on Status RGB LED to red"
    on_press:
      - light.turn_on:
          id: doorman_rgb_status_led
          red: 100%
          green: 0%
          blue: 0%
```
:::

## Advanced Examples
::: details Sending Bus commands from Home Assistant
You can use Home Assistant actions (formerly services) to send commands on the Bus.
> [!INFO]
> Don't forget the leading `0x` if you want to send the HEX command. Otherwise you have to convert the hex command into a decimal number.

```yaml
service: esphome.doorman_s3_send_tcs_command
data:
  command: 0x3a001100
```
:::


::: details Listening for ESPHome events
Doorman will send `esphome.doorman` events to Home Assistant everytime a command is received.

Each Event is structured like:
```yaml
event_type: esphome.doorman
data:
  device_id: 373c62d6788cf81d322763235513310e
  command: "00001100"
origin: LOCAL
time_fired: "2024-08-12T12:34:13.718317+00:00"
context:
  id: 01J5399Y2PP2XS2VRYKBT3H3AV
  parent_id: null
  user_id: null
```

Home Assistant Automation Example:
```yaml
alias: Trigger on Doorman TCS Open Door Command
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
:::
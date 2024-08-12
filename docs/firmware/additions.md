## Examples

::: details Handle static TCS Commands
You can easily add more binary sensors next to the preconfigured ones for every possible TCS Command.
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++]
  - platform: tcs_intercom // [!code ++]
    name: "Custom Command" // [!code ++]
    command: 0x3b8f9a00 // [!code ++]
```
:::

::: details Controlling the internal RGB Status LED
If you want to control the onboard RGB LED with a Button (Example) just use the Light ID: doorman_rgb_status_led.
```yaml
<!--@include: minimal.example.yaml-->

button: // [!code ++]
  - platform: template // [!code ++]
    name: "Turn on Status RGB LED to red" // [!code ++]
    on_press: // [!code ++]
      - light.turn_on: // [!code ++]
          id: doorman_rgb_status_led // [!code ++]
          red: 100% // [!code ++]
          green: 0% // [!code ++]
          blue: 0% // [!code ++]
```
:::

::: details Use the external hardware button
If you want to use the external button to trigger automations you can just extend your yaml config.
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++]
  - id: !extend doorman_external_button // [!code ++]
    on_press: // [!code ++]
      - logger.log: "External button pressed!" // [!code ++]
```
:::

## Advanced Examples
::: details Home Assistant: Sending Bus commands
You can use Home Assistant actions (formerly services) to send commands on the Bus.
> [!INFO]
> Don't forget the leading `0x` if you want to send the HEX command. Otherwise you have to convert the hex command into a decimal number.

```yaml
service: esphome.doorman_s3_send_tcs_command
data:
  command: 0x3a001100
```
:::

::: details Home Assistant: Listening for ESPHome events
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

::: details ESPHome: Handle runtime config TCS Commands
You can add more configurable command binary sensors next to the preconfigured ones using lambda, globals and text inputs.

```yaml
<!--@include: minimal.example.yaml-->

globals: // [!code ++]
  - id: custom_command // [!code ++]
    type: int // [!code ++]
    restore_value: true // [!code ++]
    initial_value: '0x3b8f9a00' // [!code ++]

text: // [!code ++]
  - platform: template // [!code ++]
    name: Custom Command // [!code ++]
    optimistic: true // [!code ++]
    mode: text // [!code ++]
    restore_value: true // [!code ++]
    initial_value: '3b8f9a00' // [!code ++]
    on_value: // [!code ++]
      then: // [!code ++]
        - lambda: |- // [!code ++]
            id(custom_command) = std::stoi(x.c_str(), nullptr, 16); // [!code ++]
    entity_category: CONFIG // [!code ++]
    icon: "mdi:console-network" // [!code ++]

binary_sensor: // [!code ++]
  - platform: tcs_intercom // [!code ++]
    name: "Custom Command" // [!code ++]
    lambda: !lambda "return id(custom_command);" // [!code ++]
```
:::

::: details ESPHome: Adding a Bus Voltage sensor
You can add a `Bus Voltage` sensor for older intercoms operating on 14-24V DC.\
It might be possible to implement other protocols as well in the future.
```yaml
<!--@include: minimal.example.yaml-->

# New ADC Voltage Sensor // [!code ++]
sensor: // [!code ++]
  - platform: adc // [!code ++]
    id: bus_voltage // [!code ++]
    name: Bus Voltage // [!code ++]
    pin: // [!code ++]
      number: GPIO9 // [!code ++]
      allow_other_uses: true // [!code ++]
    update_interval: 500ms // [!code ++]
    attenuation: 11dB // [!code ++]

# Extend tcs_intercom component // [!code ++]
# Allow RX pin to be used for other cases as well // [!code ++]
tcs_intercom: // [!code ++]
  rx_pin: // [!code ++]
    number: GPIO9 // [!code ++]
    allow_other_uses: true // [!code ++]
```
:::

::: details ESPHome: Create your own Doorbell Pattern
If you want to create a special doorbell pattern you can easily extend the existing doorbell entities.
```yaml
<!--@include: minimal.example.yaml-->

# Extend the doorbell_pattern event entity // [!code ++]
# Add a new apartment_special event type // [!code ++]
event: // [!code ++]
  - id: !extend doorbell_pattern // [!code ++]
    event_types: // [!code ++]
      - "apartment_special" // [!code ++]

# Extend the doorman_apartment_doorbell entity // [!code ++]
# and add your new special pattern // [!code ++]
binary_sensor: // [!code ++]
  - id: !extend doorman_apartment_doorbell // [!code ++]
    on_multi_click: // [!code ++]
      # Special Pattern // [!code ++]
      - timing: // [!code ++]
          - ON for at most 0.5s // [!code ++]
          - OFF for at least 2s // [!code ++]
        then: // [!code ++]
          - logger.log: "Special pattern detected!" // [!code ++]
          - event.trigger: // [!code ++]
              id: doorbell_pattern // [!code ++]
              event_type: apartment_special // [!code ++]
```
:::
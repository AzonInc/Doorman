## Examples

::: details Create a simple TCS Command Binary Sensor
You can easily add more binary sensors next to the preconfigured ones for every possible TCS Command.
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++] // [!code focus]
  - platform: tcs_intercom // [!code ++] // [!code focus]
    name: "Custom Command" // [!code ++] // [!code focus]
    command: 0x3b8f9a00 // [!code ++] // [!code focus]
```
:::

::: details Controlling the internal RGB Status LED
If you want to control the onboard RGB LED with a Button (Example) just use the Light ID: doorman_rgb_status_led.
```yaml
<!--@include: minimal.example.yaml-->

button: // [!code ++] // [!code focus]
  - platform: template // [!code ++] // [!code focus]
    name: "Turn on Status RGB LED to red" // [!code ++] // [!code focus]
    on_press: // [!code ++] // [!code focus]
      - light.turn_on: // [!code ++] // [!code focus]
          id: doorman_rgb_status_led // [!code ++] // [!code focus]
          red: 100% // [!code ++] // [!code focus]
          green: 0% // [!code ++] // [!code focus]
          blue: 0% // [!code ++] // [!code focus]
```
:::

::: details Use the external hardware button
If you want to use the external button to trigger automations you can just extend your yaml config.
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++] // [!code focus]
  - id: !extend doorman_external_button // [!code ++] // [!code focus]
    on_press: // [!code ++] // [!code focus]
      - logger.log: "External button pressed!" // [!code ++] // [!code focus]
```
:::

## Advanced Examples
### Home Assistant
::: details Sending Bus commands
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

### ESPHome
::: details Create a runtime config TCS Command Binary Sensor
You can add more configurable command binary sensors next to the preconfigured ones using lambda, globals and text inputs.

```yaml
<!--@include: minimal.example.yaml-->

globals: // [!code ++] // [!code focus]
  - id: custom_command // [!code ++] // [!code focus]
    type: int // [!code ++] // [!code focus]
    restore_value: true // [!code ++] // [!code focus]
    initial_value: '0x3b8f9a00' // [!code ++] // [!code focus]

text: // [!code ++] // [!code focus]
  - platform: template // [!code ++] // [!code focus]
    id: custom_command_input // [!code ++] // [!code focus]
    name: Custom Command // [!code ++] // [!code focus]
    entity_category: CONFIG // [!code ++] // [!code focus]
    icon: "mdi:console-network" // [!code ++] // [!code focus]
    mode: text // [!code ++] // [!code focus]
    lambda: |- // [!code ++] // [!code focus]
      unsigned long number = id(custom_command); // [!code ++] // [!code focus]
      return str_upper_case(format_hex(number)); // [!code ++] // [!code focus]
    set_action: // [!code ++] // [!code focus]
      then: // [!code ++] // [!code focus]
        - lambda: |- // [!code ++] // [!code focus]
            x.erase(std::remove_if(x.begin(), x.end(), [](char c) { return !std::isxdigit(c); }), x.end()); // [!code ++] // [!code focus]
            x.erase(0, x.find_first_not_of('0')); // [!code ++] // [!code focus]
            x.resize(8); // [!code ++] // [!code focus]
            unsigned long number = 0; // [!code ++] // [!code focus]
            if(!x.empty()) { // [!code ++] // [!code focus]
              number = std::stoul(x.c_str(), nullptr, 16); // [!code ++] // [!code focus]
            } // [!code ++] // [!code focus]
            id(custom_command) = number; // [!code ++] // [!code focus]
            id(custom_command_input)->publish_state(str_upper_case(format_hex(number))); // [!code ++] // [!code focus]

binary_sensor: // [!code ++] // [!code focus]
  - platform: tcs_intercom // [!code ++] // [!code focus]
    name: "Custom Command" // [!code ++] // [!code focus]
    lambda: !lambda "return id(custom_command);" // [!code ++] // [!code focus]
```
:::

::: details Create a Bus Voltage sensor
You can add a `Bus Voltage` sensor for older intercoms operating on 14-24V DC.\
It might be possible to implement other protocols as well in the future.
```yaml
<!--@include: minimal.example.yaml-->

# New ADC Voltage Sensor // [!code ++] // [!code focus]
sensor: // [!code ++] // [!code focus]
  - platform: adc // [!code ++] // [!code focus]
    id: bus_voltage // [!code ++] // [!code focus]
    name: Bus Voltage // [!code ++] // [!code focus]
    pin: // [!code ++] // [!code focus]
      number: GPIO9 // [!code ++] // [!code focus]
      allow_other_uses: true // [!code ++] // [!code focus]
    update_interval: 500ms // [!code ++] // [!code focus]
    attenuation: 12dB // [!code ++] // [!code focus]

# Extend tcs_intercom component // [!code ++] // [!code focus]
# Allow RX pin to be used for other cases as well // [!code ++] // [!code focus]
tcs_intercom: // [!code ++] // [!code focus]
  rx_pin: // [!code ++] // [!code focus]
    number: GPIO9 // [!code ++] // [!code focus]
    allow_other_uses: true // [!code ++] // [!code focus]
```
:::

::: details Create your own Doorbell Pattern
If you want to create a special doorbell pattern you can easily extend the existing doorbell entities.
You can find more information about the patterns in the [ESPHome Docs](https://esphome.io/components/binary_sensor/index.html#on-multi-click).
```yaml
<!--@include: minimal.example.yaml-->

# Extend the doorbell_pattern event entity // [!code ++] // [!code focus]
# Add a new apartment_special event type // [!code ++] // [!code focus]
event: // [!code ++] // [!code focus]
  - id: !extend doorbell_pattern // [!code ++] // [!code focus]
    event_types: // [!code ++] // [!code focus]
      - "apartment_special" // [!code ++] // [!code focus]

# Extend the apartment_doorbell / entrance_doorbell entity // [!code ++] // [!code focus]
# and add your new special pattern // [!code ++] // [!code focus]
binary_sensor: // [!code ++] // [!code focus]
  - id: !extend apartment_doorbell // [!code ++] // [!code focus]
    on_multi_click: // [!code ++] // [!code focus]
      # Special Pattern // [!code ++] // [!code focus]
      - timing: // [!code ++] // [!code focus]
          # Press twice with no more than one second between each press. // [!code ++] // [!code focus]
          - ON for at most 0.5s // [!code ++] // [!code focus]
          - OFF for at most 1s // [!code ++] // [!code focus]
          - ON for at most 0.5s // [!code ++] // [!code focus]
          - OFF for at least 2s // [!code ++] // [!code focus]
        then: // [!code ++] // [!code focus]
          - logger.log: "Special pattern detected!" // [!code ++] // [!code focus]
          - event.trigger: // [!code ++] // [!code focus]
              id: doorbell_pattern // [!code ++] // [!code focus]
              # Use the previously defined new event type here // [!code ++] // [!code focus]
              event_type: apartment_special // [!code ++] // [!code focus]
```
:::
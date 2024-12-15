## Examples

::: details Create a simple TC Command Binary Sensor
You can easily add additional binary sensors for any TC Command, alongside the preconfigured ones.

Command Builder:
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++] // [!code focus]
  - platform: tc_bus // [!code ++] // [!code focus]
    name: "Custom Command" // [!code ++] // [!code focus]
    type: open_door // [!code ++] // [!code focus]
    address: 0 // [!code ++] // [!code focus]
    web_server: // [!code ++] // [!code focus]
      sorting_group_id: sorting_group_listeners // [!code ++] // [!code focus]
```

32-Bit Commands:
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++] // [!code focus]
  - platform: tc_bus // [!code ++] // [!code focus]
    name: "Custom Command" // [!code ++] // [!code focus]
    command: 0x00001100 // [!code ++] // [!code focus]
    web_server: // [!code ++] // [!code focus]
      sorting_group_id: sorting_group_listeners // [!code ++] // [!code focus]
```
:::

::: details Controlling the internal RGB Status LED
If you want to control the onboard RGB LED with a button (for example), simply use the Light ID: `doorman_rgb_status_led`.
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

::: details Use the External Hardware Button
If you want to use the external button to trigger automations, you can simply extend your YAML configuration.
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++] // [!code focus]
  - id: !extend doorman_external_button // [!code ++] // [!code focus]
    on_press: // [!code ++] // [!code focus]
      - logger.log: "External button pressed!" // [!code ++] // [!code focus]
```
:::

::: details Use I²C Sensors
If you want to add sensors via the I²C bus, you can use the two available GPIO pins and add the I²C component to your configuration. You can then attach your sensors to these two I²C GPIO pins.
```yaml
<!--@include: minimal.example.yaml-->

i2c: // [!code ++] // [!code focus]
  sda: GPIO40 // [!code ++] // [!code focus]
  scl: GPIO48 // [!code ++] // [!code focus]
  scan: true // [!code ++] // [!code focus]
  id: i2c_bus // [!code ++] // [!code focus]
```
:::


## Advanced Examples
### Home Assistant
::: details Sending Bus commands
You can use Home Assistant actions (formerly known as services) to send commands on the bus.
Either use the `command` to send raw commands or `type`, `address`, `payload` and `serial_number` to send commands via the command builder.

> [!INFO]
> Remember to include the leading `0x` when calling the action with the `command` property. If you omit it, you'll need to convert the HEX command to a decimal number first.

Command Builder:
```yaml
service: esphome.doorman_s3_send_tc_command
data:
  type: open_door
  address: 0
  payload: 0
  serial_number: 123456
```

32-Bit Commands via `command`:
```yaml
service: esphome.doorman_s3_send_tc_command_raw
data:
  command: 0x11E24080
```
:::

::: details Listening for ESPHome events
Doorman will send esphome.doorman events to Home Assistant every time a command is received.

Each event is structured as follows:
```yaml
event_type: esphome.doorman
data:
  device_id: 373c62d6788cf81d322763235513310e
  command: "00001100"
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

Automation Example (HEX Command):
```yaml
alias: Trigger on Doorman TC Open Door Command
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

Automation Example (Command Builder):
```yaml
alias: Trigger on Doorman TC Open Door Command
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
:::

### ESPHome
::: details Create Your Own Doorbell Pattern
If you want to create a custom doorbell pattern, you can easily extend the existing doorbell entities. For more information about patterns, refer to the [ESPHome Docs](https://esphome.io/components/binary_sensor/index.html#on-multi-click).
```yaml
<!--@include: minimal.example.yaml-->

# Extend the doorbell_pattern event entity // [!code ++] // [!code focus]
# Add a new apartment_special event type // [!code ++] // [!code focus]
event: // [!code ++] // [!code focus]
  - id: !extend apartment_doorbell_pattern // [!code ++] // [!code focus]
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
              id: apartment_doorbell_pattern // [!code ++] // [!code focus]
              # Use the previously defined new event type here // [!code ++] // [!code focus]
              event_type: apartment_special // [!code ++] // [!code focus]
```
:::

::: details Turn on the light when someone rings the entrance doorbell
You can turn on the light when someone rings the entrance doorbell.
```yaml
<!--@include: minimal.example.yaml-->

binary_sensor: // [!code ++] // [!code focus]
  - id: !extend entrance_doorbell // [!code ++] // [!code focus]
    on_press: // [!code ++] // [!code focus]
      - tc_bus.send: // [!code ++] // [!code focus]
          type: "light" // [!code ++] // [!code focus]
```

If you want to account for the sun's elevation as well, you can adjust it accordingly.
```yaml
<!--@include: minimal.example.yaml-->

# Import the Home Assistant sun elevation sensor // [!code ++] // [!code focus]
sensor: // [!code ++] // [!code focus]
  - platform: homeassistant // [!code ++] // [!code focus]
    id: sun_elevation // [!code ++] // [!code focus]
    entity_id: sun.sun // [!code ++] // [!code focus]
    attribute: elevation // [!code ++] // [!code focus]

# Extend the entrance doorbell sensor // [!code ++] // [!code focus]
binary_sensor: // [!code ++] // [!code focus]
  - id: !extend entrance_doorbell // [!code ++] // [!code focus]
    on_press: // [!code ++] // [!code focus]
      # Sun elevation is <= 0 (dark) // [!code ++] // [!code focus]
      - if: // [!code ++] // [!code focus]
          condition: // [!code ++] // [!code focus]
            sensor.in_range: // [!code ++] // [!code focus]
              id: sun_elevation // [!code ++] // [!code focus]
              below: 1 // [!code ++] // [!code focus]
          then: // [!code ++] // [!code focus]
            # Turn on the light // [!code ++] // [!code focus]
            - tc_bus.send: // [!code ++] // [!code focus]
                type: "light" // [!code ++] // [!code focus]
```
:::

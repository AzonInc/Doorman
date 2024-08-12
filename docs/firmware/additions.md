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
```yaml
service: esphome.doorman_s3_send_tcs_command
data:
  command: 0x3a001100
```
:::
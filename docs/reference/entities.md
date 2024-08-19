# Entities
The Doorman firmware exposes many entities by default. This page describes the purpose of each.

## Sensors
### Last Bus Command <Badge type="tip" text="last_bus_command" />
This sensor reports every published command on the Bus.
### Uptime <Badge type="tip" text="doorman_uptime" />
This sensor reports the uptime in seconds.
### Nuki Battery Level <Badge type="tip" text="nuki_battery_level" />
This sensor reports the battery level of the paired Nuki smart lock.

## Binary Sensors
### Entrance Doorbell <Badge type="tip" text="entrance_doorbell" />
This sensor will turn on whenever the `entrance_doorbell_command` command is received.
### Apartment Doorbell <Badge type="tip" text="apartment_doorbell" />
This sensor will turn on whenever the `apartment_doorbell_command` command is received.
### Pick up phone <Badge type="tip" text="pick_up_phone" /> <Badge type="info" text="Disabled by default" />
This sensor will turn on whenever the `pick_up_phone_command` command is received.
### Hang up phone <Badge type="tip" text="hang_up_phone" /> <Badge type="info" text="Disabled by default" />
This sensor will turn on whenever the `hang_up_phone_command` command is received.
### Ring To Open: Toggle <Badge type="tip" text="ring_to_open_toggle" /> <Badge type="info" text="Internal" />
This sensor will turn on whenever the `ring_to_open_toggle_command` command is received.
### Turn on light: Trigger <Badge type="tip" text="turn_on_light_trigger" /> <Badge type="info" text="Internal" />
This sensor will turn on whenever the `turn_on_light_command` command is received.
### External Button <Badge type="tip" text="doorman_external_button" /> <Badge type="info" text="Disabled by default" />
This sensor will turn on when the external button is pressed.
### Nuki Connected <Badge type="tip" text="nuki_connected" />
This sensor will turn on when the paired Nuki smart lock is connected.
### Nuki Paired <Badge type="tip" text="nuki_paired" />
This sensor will turn on when a Nuki smart lock is paired.
### Nuki Battery Critical <Badge type="tip" text="nuki_battery_critical" />
This sensor will turn on when the paired Nuki smart lock has a critical battery level.
### Nuki Door Sensor <Badge type="tip" text="nuki_door_sensor" /> <Badge type="info" text="Disabled by default" />
No description yet.
### Nuki Door Sensor State <Badge type="tip" text="nuki_door_sensor_state" /> <Badge type="info" text="Disabled by default" />
This sensor reports the door sensor state of the paired Nuki smart lock.

## Switches
### Ring To Open <Badge type="tip" text="doorman_ring_to_open" />
This switch controls the [Ring To Open](../automation/ring-to-open) automation.
### Ring To Open: Confirmation <Badge type="tip" text="doorman_ring_to_open_confirmation" /> <Badge type="info" text="Disabled by default" />
This switch controls the [Ring To Open](../automation/ring-to-open) turn-on-confirmation.
### Relay <Badge type="tip" text="doorman_relay" /> <Badge type="info" text="Disabled by default" />
This switch controls the built-in relay.
### Nuki Pairing Mode <Badge type="tip" text="nuki_pairing_mode" /> <Badge type="info" text="Disabled by default" />
This switch controls the Nuki pairing mode.

## Buttons
### Open Entrance Door <Badge type="tip" text="open_entrance_door" />
This button is used to open the entrance door by sending the `open_entrance_door_command` command on the Bus.
### Open Second Door <Badge type="tip" text="open_second_door" /> <Badge type="info" text="Disabled by default" />
This button is used to open the second door by sending the `open_second_door_command` command on the Bus.
### Turn on the light <Badge type="tip" text="turn_on_light" /> <Badge type="info" text="Disabled by default" />
This button is used to turn on the light by sending the `turn_on_light_command` command on the Bus.
### Nuki Unpair Device <Badge type="tip" text="nuki_unpair_device" /> <Badge type="info" text="Disabled by default" />
This button is used to unpair your Nuki smart lock.
### Restart <Badge type="tip" text="doorman_restart" /> <Badge type="info" text="Disabled by default" />
This button is used to restart the Doorman unit, for example if you have changed a configuration that requires a restart.
### Safe mode <Badge type="tip" text="doorman_safe_mode" /> <Badge type="info" text="Disabled by default" />
This button is used to restart the Doorman unit into safe mode, for example if you want to update via OTA and your Doorman is too busy.
### Restore Factory Settings <Badge type="tip" text="doorman_factory_reset" /> <Badge type="info" text="Disabled by default" />
This button is used to restart the Doorman unit and load the factory defaults.
::: danger USE WITH GREAT CAUTION
All credentials, global variables, counters and saved states stored in non-volatile memory will be lost with no chance of recovering them. Even raw reading of flash memory with esptool will not help, since data is physically erased from flash memory.

If you configured WiFi using the captive portal, Improv Serial or Improv BLE, this will reset WiFi settings as well, thus making such devices offline. You’ll need to be in close proximity to your device to configure it again using a built-in WiFi access point and captive portal.
:::

## Texts
### Command: Apartment Doorbell <Badge type="tip" text="apartment_doorbell_command_input" />
This input sets the code which is sent when someone rings the apartment doorbell.
### Command: Entrance Doorbell <Badge type="tip" text="entrance_doorbell_command_input" />
This input sets the code which is sent when someone rings the entrance doorbell.
### Command: Open Entrance Door <Badge type="tip" text="open_entrance_door_command_input" />
This input sets the code which is sent when you press the button to open the entrance door.
### Command: Open Second Door <Badge type="tip" text="open_second_door_command_input" /> <Badge type="info" text="Disabled by default" />
This input sets the code which is sent when you press the button to open the second door.
### Command: Pick up phone <Badge type="tip" text="pick_up_phone_command_input" /> <Badge type="info" text="Disabled by default" />
This input sets the code which is sent when you pick up the phone.
### Command: Hang up phone <Badge type="tip" text="hang_up_phone_command_input" /> <Badge type="info" text="Disabled by default" />
This input sets the code which is sent when you hang up the phone.
### Command: Turn on light <Badge type="tip" text="turn_on_light_command_input" /> <Badge type="info" text="Disabled by default" />
This input sets the code which is sent when you press the button to turn on the hallway light.
### Command: Ring To Open Toggle <Badge type="tip" text="ring_to_open_toggle_command_input" /> <Badge type="info" text="Disabled by default" />
This input sets the code which is used to toggle the [Ring To Open](../automation/ring-to-open) automation. You can use the function button for that as it is usually not used.

## Numbers
### Ring To Open: Delay <Badge type="tip" text="doorman_ring_to_open_delay" /> <Badge type="info" text="Disabled by default" />
This input sets the delay for the [Ring To Open](../automation/ring-to-open) automation.
::: tip
If you set the delay to the maximum (60 seconds), the automation will generate it's own random delay between 5 and 15 seconds each time the automation is triggered.
:::

## Selects
### Ring To Open: Timeout <Badge type="tip" text="doorman_ring_to_open_timeout_mode" /> <Badge type="info" text="Disabled by default" />
This input sets the timeout mode for the [Ring To Open](../automation/ring-to-open) automation.

##### Modes:
- Ring once
- 5 to 60 minutes
- Never

## Locks
### Nuki Lock <Badge type="tip" text="nuki_smart_lock" />
This is the lock entity for your paired Nuki smart lock.

## Events
### Doorbell Pattern <Badge type="tip" text="doorbell_pattern" />
This event entity will will trigger each time a doorbell pattern is detected. Learn more about the pattern events [here](../guide/automation/pattern-events).

##### Event Types
- apartment_single
- apartment_double
- apartment_triple
- entrance_single
- entrance_double
- entrance_triple

### Phone pick up Pattern <Badge type="tip" text="phone_pick_up_pattern" />
This event entity will will trigger each time a phone pick up pattern is detected. Learn more about the pattern events [here](../guide/automation/pattern-events).

##### Event Types
- single
- double
- triple

## Updates
### Firmware (stable) <Badge type="tip" text="update_http_request_stable" />
This Update entity will show you if an updates on the stable branch is available and offers you to install the update via the HTTP OTA update mechanism.

### Firmware (dev) <Badge type="tip" text="update_http_request_dev" />
This Update entity will show you if an updates on the development branch is available and offers you to install the update via the HTTP OTA update mechanism.

## Lights
### Status LED
There is a small LED on the Doorman's PCB that is controlled by the ESPHome [Status LED](https://esphome.io/components/light/status_led.html) component.

By default the LED is on when everything is OK, but you can turn it off if it annoys you. If you turn it off, it will still retain the Status LED functionality, i.e., it will blink if something is wrong.

### RGB Status LED
There is another small WS2812B LED on the Doorman's PCB which is used to inidcate more events like AP mode, Ring To Open active, Nuki Pairing mode and more.

##### States:
- Waiting for WiFi / AP enabled <Badge type="tip" text="Blinking fast orange" />
- Waiting for Home Assistant to connect <Badge type="tip" text="Blinking slow blue" />
- Home Assistant connected <Badge type="tip" text="Solid blue for 3 seconds" />
- Ring To Open active <Badge type="tip" text="Blinking slow yellow" />
- Nuki pairing mode active <Badge type="tip" text="Blinking slow purple" />
- Nuki paired successfully <Badge type="tip" text="Solid purple for 3 seconds" />
# Entities
The Doorman firmware exposes many entities by default. This page explains the purpose of each one.

## Sensors

### Last Bus Command <Badge type="tip" text="last_bus_command" />
Reports every published command on the bus.

### Uptime <Badge type="tip" text="doorman_uptime" />
Reports the uptime in seconds.

### Nuki Battery Level <Badge type="tip" text="nuki_battery_level" /> <Badge type="info" text="Nuki Bridge only" />
Reports the battery level of the paired Nuki smart lock.


## Binary Sensors

### Entrance Doorbell <Badge type="tip" text="entrance_doorbell" />
Activates whenever the `entrance_doorbell_command` command is received.

### Apartment Doorbell <Badge type="tip" text="apartment_doorbell" />
Activates whenever the `apartment_doorbell_command` command is received.

### Pick up phone <Badge type="tip" text="pick_up_phone" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `pick_up_phone_command` command is received.

### Hang up phone <Badge type="tip" text="hang_up_phone" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `hang_up_phone_command` command is received.

### Function Button <Badge type="tip" text="function_button" />
Activates whenever the `function_button_command` command is received.

### Light Button <Badge type="tip" text="light_button" />
Activates whenever the `light_button_command` command is received.

### Boot Button <Badge type="tip" text="doorman_boot_button" /> <Badge type="info" text="Disabled by default" /> <Badge type="info" text="Internal" />
Activates when the boot button is pressed.

### External Button <Badge type="tip" text="doorman_external_button" /> <Badge type="info" text="Disabled by default" />
Activates when the external button is pressed.

### Nuki Connected <Badge type="tip" text="nuki_connected" /> <Badge type="info" text="Nuki Bridge only" />
Activates when the paired Nuki smart lock is connected.

### Nuki Paired <Badge type="tip" text="nuki_paired" /> <Badge type="info" text="Nuki Bridge only" />
Activates when a Nuki smart lock is paired.

### Nuki Battery Critical <Badge type="tip" text="nuki_battery_critical" /> <Badge type="info" text="Nuki Bridge only" />
Activates when the paired Nuki smart lock has a critical battery level.

### Nuki Door Sensor <Badge type="tip" text="nuki_door_sensor" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
No description yet.

### Nuki Door Sensor State <Badge type="tip" text="nuki_door_sensor_state" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Reports the door sensor state of the paired Nuki smart lock.


## Text Sensors

### Interactive Setup: Status <Badge type="tip" text="doorman_interactive_setup_status" />
Shows the current state of [Interactive Setup](../guide/getting-started#interactive-setup) for Commands.


## Switches

### Ring To Open <Badge type="tip" text="doorman_ring_to_open" />
Controls the [Ring To Open](../guide/automation/ring-to-open) automation.

### Ring To Open: Confirmation <Badge type="tip" text="doorman_ring_to_open_confirmation" /> <Badge type="info" text="Disabled by default" />
Controls the turn-on confirmation for the [Ring To Open](../guide/automation/ring-to-open) automation.

### Relay <Badge type="tip" text="doorman_relay" /> <Badge type="info" text="Disabled by default" />
Controls the built-in relay.

### Nuki Pairing Mode <Badge type="tip" text="nuki_pairing_mode" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki pairing mode.


## Buttons

### Open Entrance Door <Badge type="tip" text="open_entrance_door" />
Opens the entrance door by sending the `open_entrance_door_command` command on the Bus.

### Open Second Door <Badge type="tip" text="open_second_door" /> <Badge type="info" text="Disabled by default" />
Opens the second door by sending the `open_second_door_command` command on the Bus.

### Turn on the light <Badge type="tip" text="turn_on_light" /> <Badge type="info" text="Disabled by default" />
Turns on the light by sending the `light_button_command` command on the Bus.

### Interactive Setup: Start <Badge type="tip" text="doorman_interactive_setup_start" />
Starts the [Interactive Setup](../guide/getting-started#interactive-setup) Process to interactively setup bus commands.

### Interactive Setup: Cancel <Badge type="tip" text="doorman_interactive_setup_cancel" />
Cancels the [Interactive Setup](../guide/getting-started#interactive-setup) Process.

### Nuki Unpair Device <Badge type="tip" text="nuki_unpair_device" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Unpairs your Nuki smart lock.

### Restart <Badge type="tip" text="doorman_restart" /> <Badge type="info" text="Disabled by default" />
Restarts the Doorman unit, useful after configuration changes that require a restart.

### Safe mode <Badge type="tip" text="doorman_safe_mode" /> <Badge type="info" text="Disabled by default" />
Restarts the Doorman unit into safe mode, useful for OTA updates if the unit is too busy.

### Restore Factory Settings <Badge type="tip" text="doorman_factory_reset" /> <Badge type="info" text="Disabled by default" />
Restarts the Doorman unit and loads factory defaults.
::: danger USE WITH GREAT CAUTION
All credentials, global variables, counters, and saved states stored in non-volatile memory will be lost with no chance of recovery. Even raw reading of flash memory with esptool will not help, as data is physically erased from flash memory.

If you configured WiFi using the captive portal, Improv Serial, or Improv BLE, this will reset WiFi settings as well, making such devices offline. You'll need to reconfigure the device using a built-in WiFi access point and captive portal.
:::


## Texts

### Command: Apartment Doorbell <Badge type="tip" text="apartment_doorbell_command_input" />
Sets the code sent when someone rings the apartment doorbell.

### Command: Entrance Doorbell <Badge type="tip" text="entrance_doorbell_command_input" />
Sets the code sent when someone rings the entrance doorbell.

### Command: Open Entrance Door <Badge type="tip" text="open_entrance_door_command_input" />
Sets the code sent when you press the button to open the entrance door.

### Command: Open Second Door <Badge type="tip" text="open_second_door_command_input" /> <Badge type="info" text="Disabled by default" />
Sets the code sent when you press the button to open the second door.

### Command: Pick up phone <Badge type="tip" text="pick_up_phone_command_input" /> <Badge type="info" text="Disabled by default" />
Sets the code sent when you pick up the phone.

### Command: Hang up phone <Badge type="tip" text="hang_up_phone_command_input" /> <Badge type="info" text="Disabled by default" />
Sets the code sent when you hang up the phone.

### Command: Light Button <Badge type="tip" text="light_button_command_input" /> <Badge type="info" text="Disabled by default" />
Sets the code sent when you press the button to turn on the hallway light.

### Command: Function Button <Badge type="tip" text="function_button_command_input" /> <Badge type="info" text="Disabled by default" />
Sets the code that is sent when you press the function button (circle).\
The function button is usually not used.\
For this reason, it is used to toggle the [Ring To Open](../guide/automation/ring-to-open) automation.

## Numbers

### Ring To Open: Delay <Badge type="tip" text="doorman_ring_to_open_delay" /> <Badge type="info" text="Disabled by default" />
Sets the delay for the [Ring To Open](../guide/automation/ring-to-open) automation.
::: tip
Setting the delay to the maximum (60 seconds) will result in the automation generating a random delay between 5 and 15 seconds each time it is triggered. 
:::


## Selects

### Ring To Open: Timeout <Badge type="tip" text="doorman_ring_to_open_timeout_mode" /> <Badge type="info" text="Disabled by default" />
Sets the timeout mode for the [Ring To Open](../guide/automation/ring-to-open) automation.

##### Modes:
- Ring once
- 5 to 60 minutes
- Never


## Locks

### Nuki Lock <Badge type="tip" text="nuki_smart_lock" /> <Badge type="info" text="Nuki Bridge only" />
Represents the lock entity for your paired Nuki smart lock.


## Events

### Doorbell Pattern <Badge type="tip" text="doorbell_pattern" />
Triggers each time a doorbell pattern is detected. Learn more about pattern events [here](../guide/automation/pattern-events).

##### Event Types
- apartment_single
- apartment_double
- apartment_triple
- entrance_single
- entrance_double
- entrance_triple

### Phone pick up Pattern <Badge type="tip" text="phone_pick_up_pattern" />
Triggers each time a phone pick up pattern is detected. Learn more about pattern events [here](../guide/automation/pattern-events).

##### Event Types
- single
- double
- triple


## Updates

### Firmware <Badge type="info" text="Stable" /> <Badge type="tip" text="update_http_request_stable" />
Shows if an update on the stable branch is available and offers installation via the HTTP OTA update mechanism.

### Firmware <Badge type="info" text="Development" /> <Badge type="tip" text="update_http_request_dev" />
Shows if an update on the development branch is available and offers installation via the HTTP OTA update mechanism.


## Lights

### Status LED <Badge type="danger" text="Red" /> <Badge type="tip" text="doorman_status_led" />
A small LED on the Doorman's PCB controlled by the ESPHome [Status LED](https://esphome.io/components/light/status_led.html) component.  By default, this LED remains on when everything is functioning properly but will blink if there are issues.

You can turn off the LED if it becomes annoying. Even when turned off, it will still retain its functionality and blink to signal any issues.

### Status LED <Badge type="warning" text="RGB" /> <Badge type="tip" text="doorman_rgb_status_led" />
A small WS2812B RGB LED on the Doorman's PCB is used to indicate specific events. The LED can display various colors and patterns to provide visual feedback on different states and actions.

##### Event Types
- Waiting for WiFi / AP Enabled <Badge type="tip" text="Fast orange pulse" />
- Waiting for Home Assistant to Connect <Badge type="tip" text="Slow blue pulse" />
- Home Assistant Connected <Badge type="tip" text="Solid blue light (for 3 seconds)" />
- Ring To Open is Active <Badge type="tip" text="Slow yellow pulse" />
- Nuki Pairing Mode is Active <Badge type="tip" text="Slow purple pulse" /> <Badge type="info" text="Nuki Bridge only" />
- Nuki Paired Successfully <Badge type="tip" text="Solid purple light (for 3 seconds)" /> <Badge type="info" text="Nuki Bridge only" />
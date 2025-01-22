# Entities
The Doorman firmware exposes many entities by default. This page explains the purpose of each one.

## Sensors

### Last Bus Command <Badge type="tip" text="last_bus_command" />
Reports every published command on the bus.

### Uptime <Badge type="tip" text="doorman_uptime" /> <Badge type="info" text="Disabled by default" />
Reports the uptime in seconds.

### Nuki Battery Level <Badge type="tip" text="nuki_battery_level" /> <Badge type="info" text="Nuki Bridge only" />
Reports the battery level of the paired Nuki smart lock.


## Binary Sensors

### Entrance Doorbell <Badge type="tip" text="entrance_doorbell" />
Activates whenever the `door_call` command type is received.

### Second Entrance Doorbell <Badge type="tip" text="second_entrance_doorbell" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `door_call` command type with the second entrance door station `address` is received.

### Apartment Doorbell <Badge type="tip" text="apartment_doorbell" />
Activates whenever the `floor_call` command type is received.

### Pick up phone <Badge type="tip" text="pick_up_phone" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `start_talking` command type is received.

### Pick up phone (door call) <Badge type="tip" text="pick_up_phone_door_call" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `start_talking_door_call` command type is received.

### Hang up phone <Badge type="tip" text="hang_up_phone" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `stop_talking` command type is received.

### Hang up phone (door call) <Badge type="tip" text="hang_up_phone_door_call" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `stop_talking_door_call` command type is received.

### Function Button <Badge type="tip" text="function_button" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `control_function` command type is received.

### Light Button <Badge type="tip" text="light_button" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `light` command type is received.

### Door readiness <Badge type="tip" text="door_readiness" /> <Badge type="info" text="Disabled by default" />
Activates whenever the door readiness is active.

### Flash Button <Badge type="tip" text="doorman_boot_button" /> <Badge type="info" text="Disabled by default" />
Activates when the `FLASH` or `PRG` button on the PCB is pressed.

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


## Text Sensors

### Nuki Door Sensor State <Badge type="tip" text="nuki_door_sensor_state" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Reports the door sensor state of the paired Nuki smart lock.

### Nuki Last Unlock User <Badge type="tip" text="nuki_last_unlock_user" /> <Badge type="info" text="Nuki Bridge only" />
Reports the last user who locked or unlocked the Nuki smart lock.

### Nuki Last Lock Action <Badge type="tip" text="nuki_last_lock_action" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Reports the last lock action of the Nuki smart lock.

### Nuki Last Lock Action Trigger <Badge type="tip" text="nuki_last_lock_action_trigger" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Reports the last log action trigger of the Nuki smart lock.


## Switches

### Ring To Open <Badge type="tip" text="doorman_ring_to_open" />
Controls the [Ring To Open](../guide/automation/ring-to-open) automation.

### Ring To Open: Confirmation <Badge type="tip" text="doorman_ring_to_open_confirmation" /> <Badge type="info" text="Disabled by default" />
Controls the turn-on confirmation for the [Ring To Open](../guide/automation/ring-to-open) automation.

### Ring To Open: Display Status <Badge type="tip" text="doorman_ring_to_open_led_status" /> <Badge type="info" text="Disabled by default" />
Controls the Status LED for the [Ring To Open](../guide/automation/ring-to-open) automation.

### Relay <Badge type="tip" text="doorman_relay" /> <Badge type="info" text="Disabled by default" />
Controls the built-in relay.

### Setup Mode <Badge type="tip" text="doorman_setup_mode" />
Toggles the [Interactive Setup](../guide/getting-started#step-3-interactive-setup) Mode to easily setup your Doorman.

### Experimental Updates <Badge type="tip" text="dev_firmware" />
Enables experimental updates, allowing you to easily switch between the master and dev branches to check for the latest changes.

### Nuki Pairing Mode <Badge type="tip" text="nuki_pairing_mode" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki pairing mode.

### Nuki Auto unlatch <Badge type="tip" text="nuki_auto_unlatch" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Auto unlatch setting.

### Nuki Button: Locking operations <Badge type="tip" text="nuki_button_locking_operations" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Button locking operations setting.

### Nuki LED: Signal <Badge type="tip" text="nuki_led_enabled" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock LED Signal setting.

### Nuki Night Mode <Badge type="tip" text="nuki_night_mode" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Night Mode setting.

### Nuki Night Mode: Auto Lock <Badge type="tip" text="nuki_night_mode_auto_lock_enabled" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Night Mode Auto Lock setting.

### Nuki Night Mode: Reject Auto Unlock <Badge type="tip" text="nuki_night_mode_auto_unlock_disabled" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Night Mode Reject Auto Unlock setting.

### Nuki Night Mode: Lock at Start Time <Badge type="tip" text="nuki_night_mode_immediate_lock_on_start" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Night Mode Lock at Start Time setting.

### Nuki Auto Lock <Badge type="tip" text="nuki_auto_lock" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Auto Lock setting.

### Nuki Auto Lock: Immediately <Badge type="tip" text="nuki_immediate_auto_lock" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Auto Lock Immediately setting.

### Nuki Auto Unlock: Disable <Badge type="tip" text="nuki_auto_unlock_disabled" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Auto Unlock Disable setting.

### Nuki Single Lock <Badge type="tip" text="nuki_single_lock" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Single Lock setting.


## Buttons

### Open Entrance Door <Badge type="tip" text="open_entrance_door" />
Opens the entrance door by sending a `open_door` command on the bus.

### Open Second Entrance Door <Badge type="tip" text="open_second_door" /> <Badge type="info" text="Disabled by default" />
Opens the second entrance door by sending a `open_door` command with the `address` of the second outdoor station on the bus.

### Turn on the Light <Badge type="tip" text="turn_on_light" /> <Badge type="info" text="Disabled by default" />
Turns on the light by sending a `light` command on the bus.

### Identify Indoor Station <Badge type="tip" text="identify_indoor_station" /> <Badge type="info" text="Disabled by default" />
Quickly identifies your indoor station model and saves it.
::: note
Not all models are supported as old models might not support this feature.
:::

### Read Memory <Badge type="tip" text="read_memory" /> <Badge type="info" text="Disabled by default" />
Reads the memory of the intercom phone with the specified serial number.

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


## Numbers

### Serial Number <Badge type="tip" text="serial_number" /> <Badge type="info" text="Disabled by default" />
Sets the indoor station serial number for the command builder/parser.

### Entrance Door Station ID <Badge type="tip" text="entrance_door_station_id" /> <Badge type="info" text="Disabled by default" />
Sets the ID of the entrance outdoor station.

### Second Door Station ID <Badge type="tip" text="second_door_station_id" /> <Badge type="info" text="Disabled by default" />
Sets the ID of the second outdoor station.

### Ring To Open: Delay <Badge type="tip" text="doorman_ring_to_open_delay" /> <Badge type="info" text="Disabled by default" />
Sets the delay for the [Ring To Open](../guide/automation/ring-to-open) automation.
::: tip
Setting the delay to the maximum (60 seconds) will result in the automation generating a random delay between 5 and 15 seconds each time it is triggered. 
:::

### Volume: Ringtone <Badge type="tip" text="intercom_volume_ringtone" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone ringtone volume.

### Volume: Handset <Badge type="tip" text="intercom_volume_handset" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone handset volume.

### Nuki LED: Brightness <Badge type="tip" text="nuki_led_brightness" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock LED Brightness setting.

### Nuki Security Pin <Badge type="tip" text="nuki_security_pin" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Sets the Nuki Bridge Security Pin to authenticate against the Nuki Smart Lock.


## Selects

### Ring To Open: Timeout <Badge type="tip" text="doorman_ring_to_open_timeout_mode" />
Sets the timeout mode for the [Ring To Open](../guide/automation/ring-to-open) automation.

##### Modes:
- Ring once
- 5 to 60 minutes
- Never

### Ring To Open: Door Station <Badge type="tip" text="doorman_ring_to_open_door_trigger" />
Sets the triggering door for the [Ring To Open](../guide/automation/ring-to-open) automation.

##### Options:
- Entrance
- Second Entrance
- Any

### Intercom Model <Badge type="tip" text="intercom_model" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone model. Check the [Supported Models and Settings](esphome-component#model-setting-availability) to see your options.

### Ringtone: Door Call <Badge type="tip" text="intercom_ringtone_door_call" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone ringtone for door calls.

##### Options:
- Ringtone 1 ... 13

### Ringtone: Floor Call <Badge type="tip" text="intercom_ringtone_floor_call" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone ringtone for floor calls.

##### Options:
- Ringtone 1 ... 13

### Ringtone: Internal Call <Badge type="tip" text="intercom_ringtone_internal_call" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone ringtone for internal calls.

##### Options:
- Ringtone 1 ... 13

### Nuki Button: Single Press Action <Badge type="tip" text="nuki_single_button_press_action" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Single Button Press Action setting.

##### Options:
- No Action
- Intelligent
- Unlock
- Lock
- Unlatch
- Lock n Go
- Show Status

### Nuki Button: Double Press Action <Badge type="tip" text="nuki_double_button_press_action" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Double Button Press Action setting.

##### Options:
- No Action
- Intelligent
- Unlock
- Lock
- Unlatch
- Lock n Go
- Show Status

### Nuki Fob: Action 1 <Badge type="tip" text="nuki_fob_action_1" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Fob Action 1 setting.

##### Options:
- No Action
- Unlock
- Lock
- Lock n Go
- Intelligent

### Nuki Fob: Action 2 <Badge type="tip" text="nuki_fob_action_2" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Fob Action 2 setting.

##### Options:
- No Action
- Unlock
- Lock
- Lock n Go
- Intelligent

### Nuki Fob: Action 3 <Badge type="tip" text="nuki_fob_action_3" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Fob Action 3 setting.

##### Options:
- No Action
- Unlock
- Lock
- Lock n Go
- Intelligent

### Nuki Advertising Mode <Badge type="tip" text="nuki_advertising_mode" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Advertising Mode setting.

##### Options:
- Automatic
- Normal
- Slow
- Slowest


## Locks

### Nuki Lock <Badge type="tip" text="nuki_smart_lock" /> <Badge type="info" text="Nuki Bridge only" />
Represents the lock entity for your paired Nuki smart lock.


## Events

### Entrance Doorbell <Badge type="tip" text="entrance_doorbell_pattern" />
Triggers each time a doorbell pattern is detected at the entrance. Learn more about pattern events [here](../guide/automation/pattern-events).

##### Event Types
- single
- double
- triple

### Second Entrance Doorbell <Badge type="tip" text="second_entrance_doorbell_pattern" />
Triggers each time a doorbell pattern is detected at the second entrance. Learn more about pattern events [here](../guide/automation/pattern-events).

##### Event Types
- single
- double
- triple

### Apartment Doorbell <Badge type="tip" text="apartment_doorbell_pattern" />
Triggers each time a doorbell pattern is detected at the apartment. Learn more about pattern events [here](../guide/automation/pattern-events).

##### Event Types
- single
- double
- triple

### Phone pick up <Badge type="tip" text="phone_pick_up_pattern" />
Triggers each time a phone pick up pattern is detected. Learn more about pattern events [here](../guide/automation/pattern-events).

##### Event Types
- single
- double
- triple

## Updates

### Firmware <Badge type="tip" text="update_http_request" />
Shows if an update is available and offers installation via the HTTP OTA update mechanism.


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

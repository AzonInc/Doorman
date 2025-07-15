# Entities
The Doorman firmware exposes many entities by default. Below is a list of the available sensors and controls, along with an explanation of how to configure them.

## Viewing Entities in Home Assistant

1. In the Home Assistant UI, navigate to [ESPHome](https://my.home-assistant.io/redirect/integration/?domain=esphome).
2. Select your Doorman.
3. A dashboard will appear, listing all sensors and controls available for automations.

## Sensors

### Uptime <Badge type="tip" text="doorman_uptime" /> <Badge type="info" text="Disabled by default" />
Reports the uptime in seconds.

### WiFi Signal <Badge type="tip" text="doorman_wifi_signal" /> <Badge type="info" text="Disabled by default" />
Reports the wifi signal in percent.

### Nuki Battery Level <Badge type="tip" text="nuki_battery_level" /> <Badge type="info" text="Nuki Bridge only" />
Reports the battery level of the paired Nuki smart lock.

### Nuki Bluetooth Signal Strength <Badge type="tip" text="nuki_bluetooth_signal_strength" /> <Badge type="info" text="Nuki Bridge only" />
Reports the bluetooth strength of the paired Nuki smart lock.


## Binary Sensors

### Entrance Doorbell <Badge type="tip" text="entrance_doorbell" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `door_call` telegram type is received.

### Second Entrance Doorbell <Badge type="tip" text="second_entrance_doorbell" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `door_call` telegram type with the second entrance door station `address` is received.

### Apartment Doorbell <Badge type="tip" text="apartment_doorbell" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `floor_call` telegram type is received.

### Pick up phone <Badge type="tip" text="pick_up_phone" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `start_talking` telegram type is received.

### Pick up phone (door call) <Badge type="tip" text="pick_up_phone_door_call" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `start_talking_door_call` telegram type is received.

### Hang up phone <Badge type="tip" text="hang_up_phone" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `stop_talking` telegram type is received.

### Hang up phone (door call) <Badge type="tip" text="hang_up_phone_door_call" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `stop_talking_door_call` telegram type is received.

### Function Button <Badge type="tip" text="function_button" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `control_function` telegram type is received.

### Light Button <Badge type="tip" text="light_button" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `light` telegram type is received.

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

### Last Bus Telegram <Badge type="tip" text="last_bus_telegram" />
Reports every published telegram on the bus.

### Nuki Door Sensor State <Badge type="tip" text="nuki_door_sensor_state" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Reports the door sensor state of the paired Nuki smart lock.

### Nuki Last Unlock User <Badge type="tip" text="nuki_last_unlock_user" /> <Badge type="info" text="Nuki Bridge only" />
Reports the last user who locked or unlocked the Nuki smart lock.

### Nuki Last Lock Action <Badge type="tip" text="nuki_last_lock_action" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Reports the last lock action of the Nuki smart lock.

### Nuki Last Lock Action Trigger <Badge type="tip" text="nuki_last_lock_action_trigger" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Reports the last log action trigger of the Nuki smart lock.

### Nuki Security Pin Status <Badge type="tip" text="nuki_pin_status" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Reports the nuki component security pin status.

### ESPHome Version <Badge type="tip" text="esphome_version" /> <Badge type="info" text="Disabled by default" />
Reports the installed base ESPHome version.

### Doorman Firmware Version <Badge type="tip" text="doorman_firmware_version" /> <Badge type="info" text="Disabled by default" />
Reports the installed Doorman firmware version and build time.

### Doorman Hardware <Badge type="tip" text="doorman_hardware_version" /> <Badge type="info" text="Disabled by default" />
Reports the Doorman hardware revision.


## Switches

### Ring To Open <Badge type="tip" text="rto_central" />
Toggles the [Ring To Open](../guide/features/ring-to-open) automation for the specified doors.

::: tip
You can define the doors with the [RTO: Central Toggle - Door Selection](#rto-central-toggle-door-selection) select.
:::

### RTO: Entrance Door <Badge type="tip" text="rto_entrance_door" /> <Badge type="info" text="Disabled by default" />
Controls the [Ring To Open](../guide/features/ring-to-open) automation of the entrance door.

### RTO: Second Entrance Door <Badge type="tip" text="rto_second_entrance_door" /> <Badge type="info" text="Disabled by default" />
Controls the [Ring To Open](../guide/features/ring-to-open) automation of the second entrance door.

### RTO: Apartment Door <Badge type="tip" text="rto_apartment_door" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the [Ring To Open](../guide/features/ring-to-open) automation of the apartment door.

### RTO: Confirmation <Badge type="tip" text="rto_confirmation" /> <Badge type="info" text="Disabled by default" />
Controls the turn-on doorbell-confirmation for the [Ring To Open](../guide/features/ring-to-open) automation.

### RTO: Display Status <Badge type="tip" text="rto_led_status" /> <Badge type="info" text="Disabled by default" />
Controls the Status LED for the [Ring To Open](../guide/features/ring-to-open) automation.

### Relay <Badge type="tip" text="doorman_relay" /> <Badge type="info" text="Disabled by default" />
Controls the built-in relay.

### Use 32-Bit Door Protocol <Badge type="tip" text="use_32_open_door_protocol" /> <Badge type="info" text="Disabled by default" />
This guarantees the long door opener telegram is used and adds a serial number to the short door opener telegram.

### Status LED: Show Bus Activity <Badge type="tip" text="doorman_status_led_bus_activity" /> <Badge type="info" text="Disabled by default" />
When enabled, the status LED will briefly blink to indicate activity on the bus — useful for debugging or just keeping an eye on communication.

### Setup Mode <Badge type="tip" text="doorman_setup_mode" />
Toggles the [Interactive Setup](../guide/getting-started#interactive-setup) Mode to easily setup your Doorman.

### Experimental Firmware <Badge type="tip" text="dev_firmware" />
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

### Hallway Light <Badge type="tip" text="hallway_light" /> <Badge type="info" text="Disabled by default" />
Turns on the light by sending a `light` telegram on the bus.

### Identify Indoor Station <Badge type="tip" text="identify_indoor_station" /> <Badge type="info" text="Disabled by default" />
Quickly identifies your indoor station model and saves it.
::: note
Not all models are supported as old models might not support this feature.
:::

### Read Memory <Badge type="tip" text="read_memory" /> <Badge type="info" text="Disabled by default" />
Reads the memory of the intercom phone with the specified serial number.

### Reset HomeKit Pairing <Badge type="tip" text="homekit_reset_pairing" /> <Badge type="info" text="HomeKit integration only" />
Unpairs all paired HomeKit devices.

### Nuki Unpair Device <Badge type="tip" text="nuki_unpair_device" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Unpairs your Nuki smart lock.

### Save MQTT Configuration <Badge type="tip" text="mqtt_save_settings" /> <Badge type="info" text="MQTT integration only" />
Saves the MQTT credentials and reconnect.

### Install Update <Badge type="tip" text="update_install" />
Installs the latest available Doorman Firmware update.

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

### Serial Number <Badge type="tip" text="indoor_station_serial_number" /> <Badge type="info" text="Disabled by default" />
Sets the indoor station serial number for the telegram builder/parser.

### Entrance Door Station ID <Badge type="tip" text="entrance_door_station_id" /> <Badge type="info" text="Disabled by default" />
Sets the ID of the entrance outdoor station.

### Second Entrance Door Station ID <Badge type="tip" text="second_entrance_door_station_id" /> <Badge type="info" text="Disabled by default" />
Sets the ID of the second outdoor station.

### Volume: Ringtone <Badge type="tip" text="indoor_station_volume_ringtone" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone ringtone volume.

### Volume: Handset Door Call <Badge type="tip" text="indoor_station_volume_handset_door_call" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone handset door call volume.

### Volume: Handset Internal Call <Badge type="tip" text="indoor_station_volume_handset_internal_call" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone handset internal call volume.

### Nuki LED: Brightness <Badge type="tip" text="nuki_led_brightness" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock LED Brightness setting.

### Nuki Security Pin <Badge type="tip" text="nuki_security_pin" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Sets the Nuki Bridge Security Pin to authenticate against the Nuki Smart Lock.

### MQTT Broker Port <Badge type="tip" text="mqtt_broker_port" /> <Badge type="info" text="MQTT integration only" /> <Badge type="info" text="Disabled by default" />
Sets the MQTT Broker Port.


## Texts

### MQTT Broker Address <Badge type="tip" text="mqtt_broker_address" /> <Badge type="info" text="MQTT integration only" />
Sets the MQTT Broker Address.

### MQTT Broker Username <Badge type="tip" text="mqtt_broker_username" /> <Badge type="info" text="MQTT integration only" /> <Badge type="info" text="Disabled by default" />
Sets the MQTT Broker Username.

### MQTT Broker Password <Badge type="tip" text="mqtt_broker_password" /> <Badge type="info" text="MQTT integration only" /> <Badge type="info" text="Disabled by default" />
Sets the MQTT Broker Password.

### Entrance Door Pre-Open-Telegrams <Badge type="tip" text="entrance_door_before_open_cmds" /> <Badge type="info" text="Disabled by default" />
Specify a list of hex-telegrams (separated by semicolons) to be sent on the bus before the `open_door` telegram is issued.

### Second Entrance Door Pre-Open-Telegrams <Badge type="tip" text="second_entrance_door_before_open_cmds" /> <Badge type="info" text="Disabled by default" />
Specify a list of hex-telegrams (separated by semicolons) to be sent on the bus before the `open_door` telegram is issued.


## Selects

### Door Opener Mode <Badge type="tip" text="door_opener_mode" /> <Badge type="info" text="Disabled by default" />
Configure how the entrance door should be opened — either by sending a Bus telegram or by activating the internal Doorman relay.

##### Modes:
- **Bus Telegram**
- **Internal Relay**

### RTO: Central Toggle - Door Selection <Badge type="tip" text="rto_central_doors" />
Defines the doors for which the central `Ring To Open` switch will toggle the [Ring To Open](../guide/features/ring-to-open) automation.

##### Modes:
- **Entrance**
- **Second Entrance**
- **Any Entrance**
- **Apartment**
- **Any Entrance & Apartment**

### RTO: Entrance Door - Delay <Badge type="tip" text="rto_entrance_door_delay" /> <Badge type="info" text="Disabled by default" />
This sets the door opener delay used by the [Ring To Open](../guide/features/ring-to-open) automation.  
To prevent interference with ring pattern detection, the delay starts at **2 seconds** — unless you're using the **`single`** ring pattern condition, which allows shorter delays.

##### Available options:

* **0–1 seconds** → only applies when using the **`single`** ring pattern
* **2–25 seconds**
* **Random** → picks a value in the valid range automatically

### RTO: Second Entrance Door - Delay <Badge type="tip" text="rto_second_entrance_door_delay" /> <Badge type="info" text="Disabled by default" />
This sets the door opener delay used by the [Ring To Open](../guide/features/ring-to-open) automation.  
To prevent interference with ring pattern detection, the delay starts at **2 seconds** — unless you're using the **`single`** ring pattern condition, which allows shorter delays.

##### Available options:

* **0–1 seconds** → only applies when using the **`single`** ring pattern
* **2–25 seconds**
* **Random** → picks a value in the valid range automatically

### RTO: Apartment Door - Delay <Badge type="tip" text="rto_apartment_door_delay" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
This sets the door opener delay used by the [Ring To Open](../guide/features/ring-to-open) automation.  
To prevent interference with ring pattern detection, the delay starts at **2 seconds** — unless you're using the **`single`** ring pattern condition, which allows shorter delays.

##### Available options:

* **0–1 seconds** → only applies when using the **`single`** ring pattern
* **2–25 seconds**
* **Random** → picks a value in the valid range automatically

### RTO: Entrance Door - Timeout <Badge type="tip" text="rto_entrance_door_timeout_mode" />
Sets the timeout mode for the [Ring To Open](../guide/features/ring-to-open) automation.

##### Modes:
- **Ring once**
- **5 to 60 minutes**
- **Never**

### RTO: Second Entrance Door - Timeout <Badge type="tip" text="rto_second_entrance_door_timeout_mode" /> <Badge type="info" text="Disabled by default" />
Sets the timeout mode for the [Ring To Open](../guide/features/ring-to-open) automation.

##### Modes:
- **Ring once**
- **5 to 60 minutes**
- **Never**

### RTO: Apartment Door - Timeout <Badge type="tip" text="rto_apartment_door_timeout_mode" /> <Badge type="info" text="Nuki Bridge only" /> 
Sets the timeout mode for the [Ring To Open](../guide/features/ring-to-open) automation.

##### Modes:
- **Ring once**
- **5 to 60 minutes**
- **Never**

### RTO: Entrance Door - Pattern Condition <Badge type="tip" text="rto_entrance_door_pattern_condition" /> <Badge type="info" text="Disabled by default" />
Defines the trigger pattern condition of the [Ring To Open](../guide/features/ring-to-open) automation.

##### Options:
- **single**
- **double**
- **triple**
- **quadruple**

### RTO: Second Entrance Door - Pattern Condition <Badge type="tip" text="rto_second_entrance_door_pattern_condition" /> <Badge type="info" text="Disabled by default" />
Defines the trigger pattern condition of the [Ring To Open](../guide/features/ring-to-open) automation.

##### Options:
- **single**
- **double**
- **triple**
- **quadruple**

### RTO: Apartment Door - Pattern Condition <Badge type="tip" text="rto_apartment_door_pattern_condition" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Defines the trigger pattern condition of the [Ring To Open](../guide/features/ring-to-open) automation.

##### Options:
- **single**
- **double**
- **triple**
- **quadruple**

### RTO: Central Toggle - Trigger <Badge type="tip" text="rto_toggle_trigger" /> <Badge type="info" text="Disabled by default" />
Defines the trigger to toggle the [Ring To Open](../guide/features/ring-to-open) automation.

##### Options:
- **Manual**
- **Function Button**
- **External Button**

### Indoor Station Model <Badge type="tip" text="indoor_station_model" /> <Badge type="info" text="Disabled by default" />
Sets the intercom indoor station model. Check the [Supported Models and Settings](esphome-component#model-setting-availability) to see your options.

### Ringtone: Entrance Door Call <Badge type="tip" text="indoor_station_ringtone_entrance_door_call" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone ringtone for entrance door calls.

##### Options:
- ****Ringtone 1 ... 13****

### Ringtone: Second Entrance Door Call <Badge type="tip" text="indoor_station_ringtone_second_entrance_door_call" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone ringtone for second entrance door calls.

##### Options:
- **Ringtone 1 ... 13**

### Ringtone: Floor Call <Badge type="tip" text="indoor_station_ringtone_floor_call" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone ringtone for floor calls.

##### Options:
- **Ringtone 1 ... 13**

### Ringtone: Internal Call <Badge type="tip" text="indoor_station_ringtone_internal_call" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone ringtone for internal calls.

##### Options:
- **Ringtone 1 ... 13**

### Nuki Button: Single Press Action <Badge type="tip" text="nuki_single_button_press_action" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Single Button Press Action setting.

##### Options:
- **No Action**
- **Intelligent**
- **Unlock**
- **Lock**
- **Unlatch**
- **Lock n Go**
- **Show Status**

### Nuki Button: Double Press Action <Badge type="tip" text="nuki_double_button_press_action" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Double Button Press Action setting.

##### Options:
- **No Action**
- **Intelligent**
- **Unlock**
- **Lock**
- **Unlatch**
- **Lock n Go**
- **Show Status**

### Nuki Fob: Action 1 <Badge type="tip" text="nuki_fob_action_1" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Fob Action 1 setting.

##### Options:
- **No Action**
- **Unlock**
- **Lock**
- **Lock n Go**
- **Intelligent**

### Nuki Fob: Action 2 <Badge type="tip" text="nuki_fob_action_2" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Fob Action 2 setting.

##### Options:
- **No Action**
- **Unlock**
- **Lock**
- **Lock n Go**
- **Intelligent**

### Nuki Fob: Action 3 <Badge type="tip" text="nuki_fob_action_3" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Fob Action 3 setting.

##### Options:
- **No Action**
- **Unlock**
- **Lock**
- **Lock n Go**
- **Intelligent**

### Nuki Advertising Mode <Badge type="tip" text="nuki_advertising_mode" /> <Badge type="info" text="Nuki Bridge only" /> <Badge type="info" text="Disabled by default" />
Controls the Nuki Smart Lock Advertising Mode setting.

##### Options:
- **Automatic**
- **Normal**
- **Slow**
- **Slowest**


## Locks

### Entrance Door <Badge type="tip" text="entrance_door" />
Represents the entrance door of the building. Only the `Open` and `Unlock` actions are supported.

### Second Entrance Door <Badge type="tip" text="second_entrance_door" /> <Badge type="info" text="Disabled by default" />
Represents the second entrance door of the building. Only the `Open` and `Unlock` actions are supported.

### Apartment Door <Badge type="tip" text="apartment_door" /> <Badge type="info" text="Nuki Bridge only" />
Represents the apartment door Nuki smart lock.


## Events

### Entrance Doorbell <Badge type="tip" text="entrance_doorbell_pattern" />
Triggers each time a doorbell pattern is detected at the entrance. Learn more about pattern events [here](../guide/features/pattern-events).

##### Event Types
- **single**
- **double**
- **triple**
- **quadruple**

### Second Entrance Doorbell <Badge type="tip" text="second_entrance_doorbell_pattern" />
Triggers each time a doorbell pattern is detected at the second entrance. Learn more about pattern events [here](../guide/features/pattern-events).

##### Event Types
- **single**
- **double**
- **triple**
- **quadruple**

### Apartment Doorbell <Badge type="tip" text="apartment_doorbell_pattern" />
Triggers each time a doorbell pattern is detected at the apartment. Learn more about pattern events [here](../guide/features/pattern-events).

##### Event Types
- **single**
- **double**
- **triple**
- **quadruple**

### Phone pick up <Badge type="tip" text="phone_pick_up_pattern" />
Triggers each time a phone pick up pattern is detected. Learn more about pattern events [here](../guide/features/pattern-events).

##### Event Types
- **single**
- **double**
- **triple**
- **quadruple**

## Updates

### Doorman Firmware Update <Badge type="tip" text="update_http_request" />
Shows if an doorman firmware update is available and offers installation via the HTTP OTA update mechanism.


## Lights

### Status LED <Badge type="danger" text="Red" /> <Badge type="tip" text="doorman_status_led" />
A small LED on the Doorman's PCB controlled by the ESPHome [Status LED](https://esphome.io/components/light/status_led.html) component.  By default, this LED remains on when everything is functioning properly but will blink if there are issues.

You can turn off the LED if it becomes annoying. Even when turned off, it will still retain its functionality and blink to signal any issues.

### Status LED <Badge type="warning" text="RGB" /> <Badge type="tip" text="doorman_rgb_status_led" />
A small WS2812B RGB LED on the Doorman's PCB is used to indicate specific events. The LED can display various colors and patterns to provide visual feedback on different states and actions.

##### Event Types
- **Waiting for WiFi / AP Enabled** <Badge type="tip" text="Fast orange pulse" />
- **Waiting for Home Assistant / MQTT / HomeKit to Connect** <Badge type="tip" text="Slow blue pulse" />
- **Home Assistant / MQTT / HomeKit Connected** <Badge type="tip" text="Solid blue light (for 3 seconds)" />
- **MQTT setup required** <Badge type="tip" text="Red-blue pulse" /> <Badge type="info" text="MQTT integration only" />
- **HomeKit pairing required** <Badge type="tip" text="Red-blue pulse" /> <Badge type="info" text="HomeKit integration only" />
- **Setup Mode is Active** <Badge type="tip" text="Fast green-turquoise pulse" />
- **Setup Mode completed** <Badge type="tip" text="Solid green-turquoise (for 3 seconds)" />
- **Ring To Open is Active** <Badge type="tip" text="Slow yellow pulse" />
- **Nuki Pairing Mode is Active** <Badge type="tip" text="Slow purple pulse" /> <Badge type="info" text="Nuki Bridge only" />
- **Nuki Paired Successfully** <Badge type="tip" text="Solid purple light (for 3 seconds)" /> <Badge type="info" text="Nuki Bridge only" />
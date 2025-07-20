# TC:BUS Entities
On this page, you can view all the entities related to TC:BUS.

## Binary Sensors

### Entrance Doorbell <Badge type="tip" text="entrance_doorbell" /> <Badge type="info" text="Disabled by default" />
Activates whenever the `door_call` telegram type with the entrance door station `address` is received.

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


## Text Sensors

### Last Telegram <Badge type="tip" text="last_bus_telegram" />
Reports every published telegram on the bus.


## Switches

### Setup Mode <Badge type="tip" text="doorman_setup_mode" />
Toggles the [Interactive Setup](../guide/getting-started#interactive-setup) Mode to easily setup your Doorman.

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

### Use 32-Bit Door Protocol <Badge type="tip" text="use_32_open_door_protocol" /> <Badge type="info" text="Disabled by default" />
This guarantees the long door opener telegram is used and adds a serial number to the short door opener telegram.

## Buttons

### Identify <Badge type="tip" text="identify_indoor_station" /> <Badge type="info" text="Disabled by default" />
Quickly identifies your indoor station model and saves it.
::: note
Not all models are supported as old models might not support this feature.
:::

### Read Memory <Badge type="tip" text="read_memory" /> <Badge type="info" text="Disabled by default" />
Reads the memory of the intercom phone with the specified serial number.


## Numbers

### Serial Number <Badge type="tip" text="indoor_station_serial_number" /> <Badge type="info" text="Disabled by default" />
Sets the indoor station serial number for the telegram builder/parser.

### Entrance Station Address <Badge type="tip" text="entrance_door_station_address" /> <Badge type="info" text="Disabled by default" />
Sets the address of the entrance outdoor station.

### Second Entrance Station Address <Badge type="tip" text="second_entrance_door_station_address" /> <Badge type="info" text="Disabled by default" />
Sets the address of the second entrance outdoor station.

### Volume: Ringtone <Badge type="tip" text="indoor_station_volume_ringtone" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone ringtone volume.

### Volume: Handset Door Call <Badge type="tip" text="indoor_station_volume_handset_door_call" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone handset door call volume.

### Volume: Handset Internal Call <Badge type="tip" text="indoor_station_volume_handset_internal_call" /> <Badge type="info" text="Disabled by default" />
Sets the intercom phone handset internal call volume.


## Texts

### Entrance Door Pre-Open-Telegrams <Badge type="tip" text="entrance_door_before_open_cmds" /> <Badge type="info" text="Disabled by default" />
Specify a list of hex-telegrams (separated by semicolons) to be sent on the bus before the `open_door` telegram is issued.

### Second Entrance Door Pre-Open-Telegrams <Badge type="tip" text="second_entrance_door_before_open_cmds" /> <Badge type="info" text="Disabled by default" />
Specify a list of hex-telegrams (separated by semicolons) to be sent on the bus before the `open_door` telegram is issued.


## Selects

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
- **Ringtone 1 ... 13**

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


## Locks

### Entrance Door <Badge type="tip" text="entrance_door" />
Represents the entrance door of the building. Only the `Open` and `Unlock` actions are supported.

### Second Entrance Door <Badge type="tip" text="second_entrance_door" /> <Badge type="info" text="Disabled by default" />
Represents the second entrance door of the building. Only the `Open` and `Unlock` actions are supported.


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


## Lights

### Hallway Light <Badge type="tip" text="hallway_light" /> <Badge type="info" text="Disabled by default" />
Turns on the light by sending a `light` telegram on the bus.
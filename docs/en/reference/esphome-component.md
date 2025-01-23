# TC:BUS ESPHome Component

The TC:BUS Component for ESPHome allows you to interface with a [TCS:Bus](https://www.tcsag.de/) or [Koch TC:Bus](https://kochag.ch/) intercom system, providing automation, monitoring, and interaction capabilities within the [ESPHome](https://esphome.io/) ecosystem. This component can trigger automations based on specific commands received from the intercom system.

It also supports sending commands to the intercom and receiving various status updates (e.g., bus commands and door readiness). Additionally, actions can be set up to respond to specific commands from the intercom system.

::: tip Note
This component requires hardware like the Doorman-S3 or a [DIY solution](https://github.com/peteh/doorman) in order to communicate on the bus.
:::


## Configuration Options

### TC:BUS Hub
The `tc_bus` hub component offers the following configuration options:

| Option                 | Description                                                                                                                                   | Required | Default       |
|------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------|----------|---------------|
| `id`                   | Unique ID for the component.                                                                                                                  | Yes      |               |
| `rx_pin`               | GPIO pin for receiving data from the TCS intercom.                                                                                            | No       | `9`           |
| `tx_pin`               | GPIO pin for transmitting data to the TCS intercom. Should be connected to the transistor.                                                    | No       | `8`           |
| `event`                | Event name to be generated in Home Assistant when a bus command is received. For example, if set to `tc`, the event will be `esphome.tc`. Set to `none` to disable event generation. | No       | `tc`         |
| `on_command`           | Defines actions to be triggered when a command is received from the intercom. Returns a `CommandData` structure as the `x` variable.          | No       |               |
| `on_read_memory_complete` | Defines actions to be triggered when the memory reading is complete. Returns a `std::vector<uint8_t>` buffer as the `x` variable.          | No       |               |
| `on_read_memory_timeout`  | Defines actions to be triggered when the memory reading times out.                                                                         | No       |               |


### Text Sensors
The `tc_bus` Text Sensor component offers the following configuration options:

| Option                         | Description                                                                                             | Required | Default       |
|--------------------------------|---------------------------------------------------------------------------------------------------------|----------|---------------|
| `serial_number`                | Indoor Station Serial Number Input to set the serial number of the predefined indoor station.           | No       |               |
| `volume_handset_door_call`     | Door Call Handset Volume Select to set the handset volume for door calls of your indoor station.        | No       |               |
| `volume_handset_internal_call` | Internal Call Handset Volume Select to set the handset volume for internal calls of your indoor station.| No       |               |
| `volume_ringtone`              | Ringtone Volume Select to set the ringtone volume of your indoor station.                               | No       |               |


### Number Inputs
The `tc_bus` Number component offers the following configuration options:

| Option                 | Description                                                | Required | Default       |
|------------------------|------------------------------------------------------------|----------|---------------|
| `bus_command`          | Text Sensor to display the last received bus command.      | No       |               |
| `hardware_version`     | Text Sensor to display the Doorman-S3 hardware version.    | No       |               |

### Select Inputs
The `tc_bus` Select component offers the following configuration options:

| Option                               | Description                                                                                                    | Required | Default       |
|--------------------------------------|----------------------------------------------------------------------------------------------------------------|----------|---------------|
| `model`                              | Model Select to set the model of your indoor station (used to read and write settings). Take a look at the [supported models and settings](#model-setting-availability).| No       | `None`        |
| `ringtone_entrance_door_call`        | Entrance Door Call Ringtone Select to set the entrance door call ringtone of your indoor station.              | No       | |
| `ringtone_second_entrance_door_call` | Second Entrance Door Call Ringtone Select to set the second entrance door call ringtone of your indoor station.| No       | |
| `ringtone_floor_call`                | Floor Call Ringtone Select to set the floor call ringtone of your indoor station.                              | No       | |
| `ringtone_internal_call`             | Internal Call Ringtone Select to set the internal call ringtone of your indoor station.                        | No       | |


### Binary Sensors

The **TC:BUS Binary Sensor** detects binary states such as doorbell presses. It can be configured to trigger based on a predefined command or a lambda expression.

| Option           | Description                                                                                              | Required | Default       |
|------------------|----------------------------------------------------------------------------------------------------------|----------|---------------|
| `id`             | Unique ID for the binary sensor component.                                                               | Yes      |               |
| `icon`           | Icon to represent the sensor in the UI.                                                                  | No       | `mdi:doorbell`|
| `name`           | Name of the binary sensor.                                                                               | No       | `Doorbell`    |
| `auto_off`       | Time period after which the sensor automatically turns off, useful for momentary signals like doorbell presses.  | No       | `3s`          |
| `command`        | A specific 32-bit hexadecimal command that triggers the binary sensor when received from the TCS intercom.| Yes       | `0`           |
| `command_lambda` | Lambda expression used to dynamically generate the command that will trigger the binary sensor, instead of using a fixed command. Cannot be used with `command`.  | No       |               |
| `type`           | Command type that will trigger the binary sensor, used alongside `address` and `serial_number`. Cannot be used with `command`.  | Yes       | `unknown`     |
| `address`        | 8-bit address that serves as a condition to trigger the binary sensor. If you set it to `255`, it will catch all addresses. | No       | `0`           |
| `address_lambda` | Lambda expression to evaluate whether the binary sensor should trigger based on the address.              | No       |               |
| `payload`        | 32-bit payload that serves as a condition to trigger the binary sensor.  | No       | `0`           |
| `payload_lambda` | Lambda expression to evaluate whether the binary sensor should trigger based on the payload.              | No       |               |
| `serial_number`  | Specific intercom serial number that serves as a condition to trigger the binary sensor. If you set it to `255`, it will catch all serial numbers. | No       | `unknown`     |

::: info
You can use **either** `command`/`command_lambda` **or** a combination of `type`, `address`/`address_lambda`, `payload`/`payload_lambda`, and `serial_number`, but **not both** simultaneously.\
This ensures the binary sensor triggers either through a specific command or a combination of parameters, preventing conflicts.
:::


## Callbacks
### Received Command
The `on_command` callback of the `tc_bus` hub allows you to utilize the [CommandData](#command-data) structure, accessible as the `x` variable.

```yaml
on_command:
  - lambda: |-
      ESP_LOGD("TAG", "Received Command Type: %s", command_type_to_string(x.type));

      if (x.type == COMMAND_TYPE_OPEN_DOOR) {
        ESP_LOGD("TAG", "Opened Door of outdoor station %i", x.address);
      }
```

### Read Memory Complete
The `on_read_memory_complete` callback of the `tc_bus` hub allows you to work with the memory buffer, accessible as the `x` variable.

```yaml
on_read_memory_complete:
  - logger.log: "Completed memory reading!"
  - lambda: |-
      std::string hexString = str_upper_case(format_hex(x));
      ESP_LOGI("tcs_bus", "Memory Dump: %s", hexString.c_str());
```

### Read Memory Timeout
The `on_read_memory_timeout` callback of the `tc_bus` hub allows you to detect a failed memory reading. Most probably when a model doesn't support the related commands.

```yaml
on_read_memory_timeout:
  - logger.log: "Failed to read Memory"
```

### Identification of Indoor Station Complete
The `on_identify_complete` callback of the `tc_bus` hub allows you to utilize the [DeviceData](#device-data) structure, accessible as the `x` variable.

```yaml
on_identify_complete:
  - logger.log: "Completed identification!"
  - lambda: |-
      std::string hexString = str_upper_case(format_hex(x));
      ESP_LOGI("tcs_bus", "Memory Dump: %s", hexString.c_str());
```

### Identification of Indoor Station Timeout
The `on_identify_timeout` callback of the `tc_bus` hub allows you to detect a failed identification of the indoor station. Most probably when a model is too old doesn't support this process.

```yaml
on_identify_timeout:
  - logger.log: "Failed to identify indoor station!"
```


## Actions
### Read Memory
The `tc_bus.read_memory` action allows you to read the memory of any indoor station using the serial number.

```yaml
on_...:
  - tc_bus.read_memory:
      serial_number: 123456
```

### Identify Indoor Station
The `tc_bus.identify` action allows you to identify the model of any indoor station using the serial number.
::: tip Note
Not all models support this feature. For older indoor stations, you may need to select the model manually.
:::

```yaml
on_...:
  - tc_bus.identify:
      serial_number: 123456
```

### Set Programming Mode
The `tc_bus.set_programming_mode` action allows you to enable or disable the programming mode of the control unit.

```yaml
on_...:
  - tc_bus.set_programming_mode:
      programming_mode: true
```

### Update Setting
The `tc_bus.update_setting` action allows you to change the supported settings of any indoor station.
Take a look at the [supported models and settings](#model-setting-availability).

```yaml
on_...:
  - tc_bus.update_setting:
      type: volume_ringtone
      value: 7
      serial_number: 123456
```

### Sending Commands

You can send commands on the bus using the `tc_bus.send` action.

::: tip Note
You can either use the `command` field to send a specific command or use the `type`, `address`, `payload`, and `serial_number` fields to create a more complex message. **Both cannot be used at the same time**.

You can explicitly send a 32-bit command by using the optional `is_long` property, which is useful when the command begins with leading zeros.
:::

#### Example 1: Sending a raw Command

```yaml
on_...:
  - tc_bus.send:
      command: 0x1A2B3C4D
```
#### Example 2: Sending a raw Command with fixed size

```yaml
on_...:
  - tc_bus.send:
      command: 0x00000280
      is_long: True
```

#### Example 3: Sending a Command via Command Builder

```yaml
on_...:
  - tc_bus.send:
      type: open_door
      address: 0
      payload: 0
      serial_number: 123456
```

#### Example 3: Sending Multiple Commands

For some configurations, you may need to send multiple commands consecutively. In these cases, you must insert at least a 100ms delay between the commands.

```yaml
on_...:
  - tc_bus.send:
      command: 0x1100
  - delay: 100ms
  - tc_bus.send:
      command: 0x2100
```


## Event Handling
If the `event` parameter is set (and not `none`), an event is generated each time a command is received. You can monitor these events in Home Assistant on the [developer tools](https://my.home-assistant.io/redirect/developer_events/) page.

Each time a command is received, an event like the following is generated:

```yaml
event_type: esphome.doorman
data:
  device_id: xxxxxxxxxxxxxxxxxxxxxxxxx
  command: "0x00002400"
  type: "end_of_door_readiness"
  address: "0"
  payload: "0"
  serial_number: "0"
origin: LOCAL
time_fired: "2024-01-01T00:00:00.000000+00:00"
context:
  id: xxxxxxxxxxxxxxxxxxxxxxxx
  parent_id: null
  user_id: null
```

To trigger a Home Assistant automation based on this event, you can use an `event` trigger:

```yaml
platform: event
event_type: esphome.doorman
event_data:
  command: "0x00002480"
```

```yaml
platform: event
event_type: esphome.doorman
event_data:
  type: "end_of_door_readiness"
  address: "0"
  serial_number: "0"
```

Be sure to modify the command and event name as needed based on your configuration.


## Example YAML Configuration

Here is an example configuration for the TCS Intercom component in ESPHome:

```yaml
external_components:
  - source: github://AzonInc/Doorman
    components: [ tc_bus ]

# TC:BUS configuration
tc_bus:
  id: my_tc_bus
  rx_pin: GPIO9
  tx_pin: GPIO8
  event: "doorman"
  on_command:
    - logger.log: "Received command from intercom!"

number:
  - platform: tc_bus
    serial_number:
      name: "Serial Number"

text_sensor:
  - platform: tc_bus
    bus_command:
      name: "Last Bus Command"

# Binary sensor for doorbell press
binary_sensor:
  - platform: tc_bus
    id: doorbell_sensor_raw
    name: "Outdoor Station Doorbell (raw)"
    icon: "mdi:doorbell"
    command: 0x0C30BA80
    auto_off: 2s

  - platform: tc_bus
    id: doorbell_sensor_new
    name: "Outdoor Station Doorbell (parser)"
    icon: "mdi:doorbell"
    type: door_call
    auto_off: 2s

  - platform: tc_bus
    id: doorbell_sensor_new_other
    name: "Outdoor Station Doorbell of other serial number (parser)"
    icon: "mdi:doorbell"
    type: door_call
    serial_number: 123456

  - platform: tc_bus
    id: door_opener_sensor
    name: "Someone opened the door of outdoor station 0"
    icon: "mdi:door"
    type: open_door
    address: 0
    serial_number: 255 # 255 is used to catch all

# Sending commands
button:
  - platform: template
    name: "Open Door (raw)"
    on_press:
      - tc_bus.send:
          command: 0x1100

  - platform: template
    name: "Open Door (builder)"
    on_press:
      - tc_bus.send:
          type: open_door
          address: 0
```


## Advanced Configuration

### Accessing intercom settings
If you need to access the supported settings in the memory buffer you can use the `get_setting` method of the `tc_bus` hub.
Take a look at the [setting types](#setting-types).

Example (read and write setting):
```yaml
button:
  - platform: template
    name: "Read Handset volume"
    on_press:
      -lambda: |-
          ESP_LOGD("TAG", "Handset volume: %i", id(tc_bus_intercom)->get_setting(SETTING_VOLUME_HANDSET_DOOR_CALL));

  - platform: template
    name: "Set Handset volume"
    on_press:
      - tc_bus.update_setting:
          type: volume_handset_door_call
          value: 7
```

## Command Data
The `CommandData` structure is used internally and can also be used in the `on_command`.

```c++
struct CommandData {
    uint32_t command;
    std::string command_hex;
    CommandType type;
    uint8_t address;
    uint32_t serial_number;
    uint32_t payload;
    bool is_long;
};
```

## Device Data
The `DeviceData` structure is used internally in the identification process.

```c++
struct DeviceData {
    Model model = MODEL_NONE;
    uint32_t firmware_version = 0;
    uint8_t firmware_major = 0;
    uint8_t firmware_minor = 0;
    uint8_t firmware_patch = 0;
    uint8_t hardware_version = 0; 
};
```

## Command Types
You can use command types in binary sensors and also when [sending commands](#sending-commands):

- door_call <Badge type="tip" text="COMMAND_TYPE_DOOR_CALL" />
- floor_call <Badge type="tip" text="COMMAND_TYPE_FLOOR_CALL" />
- internal_call <Badge type="tip" text="COMMAND_TYPE_INTERNAL_CALL" />
- control_function <Badge type="tip" text="COMMAND_TYPE_CONTROL_FUNCTION" />
- start_talking_door_call <Badge type="tip" text="COMMAND_TYPE_START_TALKING_DOOR_CALL" />
- start_talking <Badge type="tip" text="COMMAND_TYPE_START_TALKING" />
- stop_talking_door_call <Badge type="tip" text="COMMAND_TYPE_STOP_TALKING_DOOR_CALL" />
- stop_talking <Badge type="tip" text="COMMAND_TYPE_STOP_TALKING" />
- open_door <Badge type="tip" text="COMMAND_TYPE_OPEN_DOOR" />
- open_door_long <Badge type="tip" text="COMMAND_TYPE_OPEN_DOOR_LONG" />
- light <Badge type="tip" text="COMMAND_TYPE_LIGHT" />
- door_opened <Badge type="tip" text="COMMAND_TYPE_DOOR_OPENED" />
- door_closed <Badge type="tip" text="COMMAND_TYPE_DOOR_CLOSED" />
- end_of_ringtone <Badge type="tip" text="COMMAND_TYPE_END_OF_RINGTONE" />
- end_of_door_readiness <Badge type="tip" text="COMMAND_TYPE_END_OF_DOOR_READINESS" />
- initialize_door_station <Badge type="tip" text="COMMAND_TYPE_INITIALIZE_DOOR_STATION" />
- reset <Badge type="tip" text="COMMAND_TYPE_RESET" />
- select_device_group <Badge type="tip" text="COMMAND_TYPE_SELECT_DEVICE_GROUP" />
- select_device_group_reset <Badge type="tip" text="COMMAND_TYPE_SELECT_DEVICE_GROUP_RESET" />
- search_devices <Badge type="tip" text="COMMAND_TYPE_SEARCH_DEVICES" />
- found_device <Badge type="tip" text="COMMAND_TYPE_FOUND_DEVICE" />
- found_device_subsystem <Badge type="tip" text="COMMAND_TYPE_FOUND_DEVICE_SUBSYSTEM" />
- programming_mode <Badge type="tip" text="COMMAND_TYPE_PROGRAMMING_MODE" />
- read_memory_block <Badge type="tip" text="COMMAND_TYPE_READ_MEMORY_BLOCK" />
- select_memory_page <Badge type="tip" text="COMMAND_TYPE_SELECT_MEMORY_PAGE" />
- write_memory <Badge type="tip" text="COMMAND_TYPE_WRITE_MEMORY" />
- request_version <Badge type="tip" text="COMMAND_TYPE_REQUEST_VERSION" />


## Setting Types
Here are the available setting types you can use to update the settings of your indoor station:

- ringtone_floor_call <Badge type="tip" text="SETTING_RINGTONE_FLOOR_CALL" />
- ringtone_entrance_door_call <Badge type="tip" text="SETTING_RINGTONE_ENTRANCE_DOOR_CALL" />
- ringtone_second_entrance_door_call <Badge type="tip" text="SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL" />
- ringtone_internal_call <Badge type="tip" text="SETTING_RINGTONE_INTERNAL_CALL" />
- volume_ringtone <Badge type="tip" text="SETTING_VOLUME_RINGTONE" />
- volume_handset_door_call <Badge type="tip" text="SETTING_VOLUME_HANDSET_DOOR_CALL" />
- volume_handset_internal_call <Badge type="tip" text="SETTING_VOLUME_HANDSET_INTERNAL_CALL" />


## Model Setting availability
Here are the available settings for specific indoor station models:

| Model                        | Available settings                                                                                         |
|------------------------------|------------------------------------------------------------------------------------------------------------|
| TCS ISH1030 / Koch TTS25     | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call`                                      |
| TCS ISH3030 / Koch TCH50 / Scantron Lux2 | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call` |
| TCS ISH3130 / Koch TCH50P / Scantron LuxPlus | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call` |
| TCS ISH3230 / Koch TCH50 GFA | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call` |
| TCS ISH3340                  | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call` |
| TCS ISW3030 / Koch TC50 / Scantron Stilux | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call` |
| TCS ISW3230 / Koch TC50 GFA  | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call` |
| TCS ISW3340                  | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call` |
| TCS ISW3130 / Koch TC50P     | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call` |
| TCS IVH3222 / Koch VTCH50 / Scantron VLux | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call` |
| TCS IVH4222 / Koch VTCH50/2D | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call` |
| TCS ISW3330 / Koch TC50 BW   | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call` |
| TCS ISW5010 / Koch TC60      | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_second_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call`, `volume_handset_internal_call` |
| TCS ISW5020                  | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_second_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call`, `volume_handset_internal_call` |
| TCS ISW5030                  | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_second_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call`, `volume_handset_internal_call` |
| TCS ISW5031                  | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_second_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call`, `volume_handset_internal_call` |
| TCS ISW5033                  | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_second_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call`, `volume_handset_internal_call` |
| TCS IVW511x / Koch VTC60/2D / Scantron VIVO | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_second_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call`, `volume_handset_internal_call` |
| TCS IVW521x | `ringtone_floor_call`, `ringtone_entrance_door_call`, `ringtone_second_entrance_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset_door_call`, `volume_handset_internal_call` |
| TCS ISW6010                  | Verification and implementation required |
| TCS ISW6031                  | Verification and implementation required |
| TCS IVW6511                  | Verification and implementation required |
| TCS ISW7030 / Koch TC70      | Verification and implementation required |
| TCS IVW7510 / Koch VTC70     | Verification and implementation required |
| TCS ISH7030 / Koch TCH70     | Verification and implementation required |
| TCS IVH7510 / Koch VTCH70    | Verification and implementation required |
| TCS ISWM7000                 | Verification and implementation required |
| TCS IVWM7000                 | Verification and implementation required |
| TCS ISW4100 / Koch TC31      | Verification and implementation required |
| TCS IMM2100 / Koch TCE31     | Verification and implementation required |
| TCS IVW2210 / Koch Ecoos     | Verification and implementation required |
| TCS IVW2211 / Koch Ecoos     | Verification and implementation required |
| TCS IVW2212 / Koch Ecoos / Scantron SLIM60T | Verification and implementation required |
| TCS VTC42V2                  | Verification and implementation required |
| TCS TC40V2                   | Verification and implementation required |
| TCS VTC40                    | Verification and implementation required |
| TCS TC40                     | Verification and implementation required |
| TCS TC2000                   | Verification and implementation required |
| TCS TC20P                    | Verification and implementation required |
| TCS TC20F                    | Verification and implementation required |
| TCS ISH3022                  | Verification and implementation required |
| TCS ISW3022                  | Verification and implementation required |
| TCS IMM1000 / Koch TCH30     | Verification and implementation required |
| TCS IMM1100 / Koch TCHE30    | Verification and implementation required |
| TCS IMM1300 / Koch VTCH30    | Verification and implementation required |
| TCS IMM1310 / Koch VTCHE30   | Verification and implementation required |
| TCS IMM1110 / Koch TCHEE30   | Verification and implementation required |
| TCS IMM1500                  | Verification and implementation required |
| TCS IVW2220 / Koch Sky       | Verification and implementation required |
| TCS IVW2221 / Koch Sky R1.00 | Verification and implementation required |
| TCS IVW3011 / Koch Skyline Plus | Verification and implementation required |
| TCS IVW3012 / Koch Skyline/Aldup | Verification and implementation required |
| TCS VMH / Koch VMH           | Verification and implementation required |
| TCS VML / Koch VML           | Verification and implementation required |
| TCS VMF / Koch VMF           | Verification and implementation required |
| Jung TKIS                    | Verification and implementation required |
| Jung TKISV                   | Verification and implementation required |
| TCS CAIXXXX / Koch CAIXXXX   | Verification and implementation required |
| TCS CAI2000 / Koch Carus     | Verification and implementation required |
| TCS ISW42X0                  | Verification and implementation required |
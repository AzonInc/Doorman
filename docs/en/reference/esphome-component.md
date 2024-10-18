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
| `model`                | Model of your intercom phone (used to read and write settings). Take a look at the [supported models and settings](#model-setting-availability).| No       | `NONE`        |
| `event`                | Event name to be generated in Home Assistant when a bus command is received. For example, if set to `tc`, the event will be `esphome.tc`. Set to `none` to disable event generation. | No       | `tc`         |
| `serial_number`        | 32-bit hexadecimal value representing the intercom's serial number.                                                                           | No       | `0`           |
| `serial_number_lambda` | Lambda function to dynamically assign the serial number. This can be used as an alternative to manually setting `serial_number`.              | No       |               |
| `bus_command`          | Text sensor to display the last received bus command.                                                                                         | No       |               |
| `hardware_version`     | Text sensor to display the Doorman-S3 hardware version.                                                                                       | No       |               |
| `door_readiness`       | Binary sensor indicating the door readiness state.                                                                                            | No       |               |
| `on_command`           | Defines actions to be triggered when a command is received from the intercom. Returns a `CommandData` structure as the `x` variable.          | No       |               |
| `on_read_memory_complete` | Defines actions to be triggered when the memory reading is complete. Returns a `std::vector<uint8_t>` buffer as the `x` variable.          | No       |               |
| `on_read_memory_timeout`  | Defines actions to be triggered when the memory reading times out.                                                                         | No       |               |

### Command Listener Binary Sensors

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


## Actions
### Read Memory
The `tc_bus.read_memory` action allows you to read the memory of any indoor station using the serial number.

```yaml
on_...:
  - tc_bus.read_memory:
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
:::

#### Example 1: Sending a raw Command

```yaml
on_...:
  - tc_bus.send:
      command: 0x1A2B3C4D
```

#### Example 2: Sending a Command via Command Builder

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
  serial_number: 798906
  bus_command:
    name: "Last Bus Command"
  hardware_version:
    name: "Doorman Hardware"
  door_readiness:
    name: "Door Readiness"
  on_command:
    - logger.log: "Received command from intercom!"

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

### Using Lambda for Serial Number
If you need to calculate or dynamically derive the serial number, use the `serial_number_lambda` option:

```yaml
tc_bus:
  serial_number_lambda: |-
    return 123456;
```

### Accessing intercom settings
If you need to access the supported settings in the memory buffer you can use the `get_setting` method of the `tc_bus` hub.

Example (read and write setting):
```yaml
number:
  - platform: template
    id: intercom_volume_handset
    name: "Volume: Handset"
    icon: "mdi:volume-high"
    min_value: 0
    max_value: 7
    step: 1
    mode: slider
    lambda: !lambda "return id(tc_bus_intercom)->get_setting(SETTING_VOLUME_HANDSET);"
    update_interval: 1s
    set_action:
      - tc_bus.update_setting:
          type: volume_handset
          value: !lambda "return x;"
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
    uint8_t length;
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
Here are the available setting types you can use to update the settings of your intercom phone:

- ringtone_floor_call <Badge type="tip" text="SETTING_RINGTONE_FLOOR_CALL" />
- ringtone_door_call <Badge type="tip" text="SETTING_RINGTONE_DOOR_CALL" />
- ringtone_internal_call <Badge type="tip" text="SETTING_RINGTONE_INTERNAL_CALL" />
- volume_ringtone <Badge type="tip" text="SETTING_VOLUME_RINGTONE" />
- volume_handset <Badge type="tip" text="SETTING_VOLUME_HANDSET" />


## Model Setting availability
Here are the available settings for specific intercom phone models:

| Model            | Available settings                                                                                         |
|------------------|------------------------------------------------------------------------------------------------------------|
| `TCS_ISH1030`    | `ringtone_floor_call`, `ringtone_door_call`, `ringtone_internal_call`                                      |
| `TCS_ISH3030`    | `ringtone_floor_call`, `ringtone_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset` |
| `TCS_ISH3230`    | `ringtone_floor_call`, `ringtone_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset` |
| `TCS_ISH3340`    | `ringtone_floor_call`, `ringtone_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset` |
| `TCS_ISW3030`    | `ringtone_floor_call`, `ringtone_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset` |
| `TCS_ISW3230`    | `ringtone_floor_call`, `ringtone_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset` |
| `TCS_ISW3340`    | `ringtone_floor_call`, `ringtone_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset` |
| `TCS_IVH3222`    | `ringtone_floor_call`, `ringtone_door_call`, `ringtone_internal_call`                                      |
| `KOCH_TC50`      | `ringtone_floor_call`, `ringtone_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset` |
| `KOCH_TCH50`     | `ringtone_floor_call`, `ringtone_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset` |
| `KOCH_TCH50P`    | `ringtone_floor_call`, `ringtone_door_call`, `ringtone_internal_call`, `volume_ringtone`, `volume_handset` |
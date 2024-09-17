# TC:BUS ESPHome Komponente

The TC:BUS Component for ESPHome allows you to interface with a [TCS:Bus](https://www.tcsag.de/) or [Koch TC:Bus](https://kochag.ch/) intercom system, providing automation, monitoring, and interaction capabilities within the [ESPHome](https://esphome.io/) ecosystem. This component can trigger automations based on specific commands received from the intercom system.

It also supports sending commands to the intercom and receiving various status updates (e.g., bus commands and door readiness). Additionally, actions can be set up to respond to specific commands from the intercom system.

::: tip Note
This component requires hardware like the Doorman-S3 or a [DIY solution](https://github.com/peteh/doorman) in order to communicate on the bus.
:::

## Configuration Options

### TCS Intercom Hub
The `tcs_intercom` hub component offers the following configuration options:

| Option                 | Description                                                                                                                                   | Required | Default       |
|------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------|----------|---------------|
| `id`                   | Unique ID for the component.                                                                                                                  | Yes      |               |
| `rx_pin`               | GPIO pin for receiving data from the TCS intercom.                                                                                            | No       | `9`           |
| `tx_pin`               | GPIO pin for transmitting data to the TCS intercom. Should be connected to the transistor.                                                    | No       | `8`           |
| `event`                | Event name to be generated in Home Assistant when a bus command is received. For example, if set to `tcs`, the event will be `esphome.tcs`. Set to `none` to disable event generation. | No       | `tcs`         |
| `serial_number`        | 32-bit hexadecimal value representing the intercom's serial number.                                                                           | No       | `0`           |
| `serial_number_lambda` | Lambda function to dynamically assign the serial number. This can be used as an alternative to manually setting `serial_number`.               | No       |               |
| `bus_command`          | Text sensor to display the last received bus command.                                                                                         | No       |               |
| `hardware_version`     | Text sensor to display the Doorman-S3 hardware version.                                                                                       | No       |               |
| `door_readiness`       | Binary sensor indicating the door readiness state.                                                                                            | No       |               |
| `on_command_action`    | Defines actions to be triggered when a command is received from the intercom. Returns a `CommandData` structure as the `x` variable.           | No       |               |

### Command Listener Binary Sensors

The **TCS Intercom Binary Sensor** detects binary states such as doorbell presses. It can be configured to trigger based on a predefined command or a lambda expression.

| Option           | Description                                                                                              | Required | Default       |
|------------------|----------------------------------------------------------------------------------------------------------|----------|---------------|
| `id`             | Unique ID for the binary sensor component.                                                               | Yes      |               |
| `tcs_intercom_id`| ID of the associated TCS intercom component that the binary sensor will listen to.                        | Yes      |               |
| `icon`           | Icon to represent the sensor in the UI.                                                                  | No       | `mdi:doorbell`|
| `name`           | Name of the binary sensor.                                                                               | No       | `Doorbell`    |
| `auto_off`       | Time period after which the sensor automatically turns off, useful for momentary signals like doorbell presses.  | No       | `3s`          |
| `command`        | A specific 32-bit hexadecimal command that triggers the binary sensor when received from the TCS intercom.| No       | `0`           |
| `command_lambda` | Lambda expression used to dynamically generate the command that will trigger the binary sensor, instead of using a fixed command. Cannot be used with `command`.  | No       |               |
| `type`           | Command type that will trigger the binary sensor, used alongside `address` and `serial_number`. Cannot be used with `command`.  | No       | `unknown`     |
| `address`        | 8-bit address that serves as a condition to trigger the binary sensor.                                    | No       | `0`           |
| `address_lambda` | Lambda expression to evaluate whether the binary sensor should trigger based on the address.              | No       |               |
| `serial_number`  | Specific intercom serial number that serves as a condition to trigger the binary sensor.                  | No       | `unknown`     |

::: info
You can use **either** `command`/`command_lambda` **or** a combination of `type`, `address`/`address_lambda`, and `serial_number`, but **not both** simultaneously.\
This ensures the binary sensor triggers either through a specific command or a combination of parameters, preventing conflicts.
:::

## Callback
The `on_command_action` callback of the `tcs_intercom` hub allows you to utilize the [CommandData](#command-data) structure, accessible as the `x` variable.

```yaml
on_command_action:
  - lambda: |-
      ESP_LOGD("TAG", "Received Command Type: %s", command_type_to_string(x.type));

      if (x.type == COMMAND_TYPE_OPEN_DOOR) {
        ESP_LOGD("TAG", "Opened Door of outdoor station %i", x.address);
      }
```

## Sending Commands

You can send commands on the bus using the `tcs_intercom.send` action.

::: tip Note
You can either use the `command` field to send a specific command or use the `type`, `address`, and `serial_number` fields to create a more complex message. **Both cannot be used at the same time**.
:::

#### Example 1: Sending a Direct Command

```yaml
on_...:
  - tcs_intercom.send:
      command: 0x1A2B3C4D
```

#### Example 2: Sending a Command with Type, Address, and Serial Number

```yaml
on_...:
  - tcs_intercom.send:
      type: open_door
      address: 0
      serial_number: 123456
```

#### Example 3: Sending Multiple Commands

For some configurations, you may need to send multiple commands consecutively. In these cases, you must insert at least a 100ms delay between the commands.

```yaml
on_...:
  - tcs_intercom.send:
      command: 0x1100
  - delay: 100ms
  - tcs_intercom.send:
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
# TCS Intercom configuration
tcs_intercom:
  id: my_tcs_intercom
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
  on_command_action:
    - logger.log: "Received command from intercom!"

# Binary sensor for doorbell press
binary_sensor:
  - platform: tcs_intercom
    id: doorbell_sensor
    tcs_intercom_id: my_tcs_intercom
    name: "Doorbell Press"
    icon: "mdi:doorbell"
    command: 0x0C30BA80
    auto_off: 2s

  - platform: tcs_intercom
    id: door_opener_sensor
    tcs_intercom_id: my_tcs_intercom
    name: "Someone opened the door of outdoor station 0"
    icon: "mdi:door"
    type: open_door
    address: 0
    serial_number: 0
    auto_off: 2s

# Action to send a command
script:
  - id: send_command
    then:
      - tcs_intercom.send:
          id: my_tcs_intercom
          command: 0x1C30BA80
```

### Explanation:
- **`rx_pin` and `tx_pin`**: Define the GPIO pins used for communication with the TCS intercom.
- **`serial_number`**: Specifies the intercom’s serial number.
- **`bus_command`** and **`hardware_version`**: Set up text sensors to display the last bus command and hardware version.
- **`door_readiness`**: Binary sensor indicating the door’s readiness state.
- **`on_command_action`**: Specifies actions to be triggered based on received commands.

## Advanced Configuration

### Using Lambda for Serial Number
If you need to calculate or dynamically derive the serial number, use the `serial_number_lambda` option:

```yaml
tcs_intercom:
  id: my_tcs_intercom
  serial_number_lambda: |-
    return 123456;  # Example of setting a dynamic serial number
```

## Command Data
The `CommandData` structure is used internally and can also be used in the `on_command_action`.

```c++
struct CommandData {
    uint32_t command;
    std::string command_hex;
    CommandType type;
    uint8_t address;
    uint32_t serial;
    uint8_t length;
};
```

## Command Types
Here are the available command types you can use as binary sensor conditions or with the `tcs_intercom.send` action:

- unknown <Badge type="tip" text="COMMAND_TYPE_UNKNOWN" />
- door_call <Badge type="tip" text="COMMAND_TYPE_DOOR_CALL" />
- floor_call <Badge type="tip" text="COMMAND_TYPE_FLOOR_CALL" />
- internal_call <Badge type="tip" text="COMMAND_TYPE_INTERNAL_CALL" />
- control_function <Badge type="tip" text="COMMAND_TYPE_CONTROL_FUNCTION" />
- start_talking_door_station <Badge type="tip" text="COMMAND_TYPE_START_TALKING_DOOR_STATION" />
- start_talking_ia <Badge type="tip" text="COMMAND_TYPE_START_TALKING_IA" />
- stop_talking_door_station <Badge type="tip" text="COMMAND_TYPE_STOP_TALKING_DOOR_STATION" />
- stop_talking_ia <Badge type="tip" text="COMMAND_TYPE_STOP_TALKING_IA" />
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
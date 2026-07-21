---
description: Complete documentation for the TC:BUS ESPHome component, including setup, configuration, and usage details.
---

# TC:BUS Component <Badge type="tip" text="tc_bus" />
The TC:BUS Component for ESPHome allows you to interface with [TCS:Bus](https://www.tcsag.de/) or [Koch TC:Bus](https://kochag.ch/) intercom systems, providing automation, monitoring, and interaction capabilities within the [ESPHome](https://esphome.io/) ecosystem.

This component can trigger automations based on specific telegrams received from the intercom system, send telegrams to the intercom, and receive various status updates (e.g., bus telegrams and door readiness). Actions can also be configured to respond to particular telegrams from the intercom.

::: tip NOTE
This is the only component required for communication on the bus. However, it can be extended with additional components, such as [TC:BUS Device](./esphome-component-device) and [TC:BUS Serial](./esphome-component-serial).

**Note:** This component requires hardware like the Doorman S3 or a [DIY solution](https://github.com/peteh/doorman) to communicate on the bus.
:::

## Configuration
The `tc_bus` hub serves as the central component enabling bus communication. It provides the following configuration options:

| Option                    | Description                                                                                                                       | Required | Default       |
|---------------------------|-----------------------------------------------------------------------------------------------------------------------------------|----------|---------------|
| `id`                      | Unique ID for the component.                                                                                                      | ✅ | |
| `rx_pin`                  | GPIO pin for receiving data from the TC:BUS.                                                                                      | | `GPIO09` |
| `tx_pin`                  | GPIO pin for transmitting data to the TC:BUS. Should be connected to the transistor.                                              | | `GPIO08` |
| `on_telegram`             | Defines actions to be triggered when a telegram is received from the TC:BUS. Returns a `TelegramData` struct as the `x` variable. | | |


## Number Inputs
The `tc_bus_device` Number Input platform offers the following configuration options:

| Option                         | Description                                                                                                   | Required | Default       |
|--------------------------------|---------------------------------------------------------------------------------------------------------------|----------|---------------|
| `tc_bus_device_id`             | ID of the related `tc_bus_device` instance.                                                                   | ✅ | |
| `entrance_address`                      | Address Number Input to set the address for the entrance outdoor station.                                              | | `63` |
| `second_entrance_address`                      | Address Number Input to set the address for the second entrance outdoor station.                                              | | `63` |

## Text Sensors
The `tc_bus` Text Sensor component offers the following configuration options:

| Option                 | Description                                                | Required | Default       |
|------------------------|------------------------------------------------------------|----------|---------------|
| `bus_telegram`         | Displays the most recently received bus telegram, showing the full communication traffic from all connected devices. | | |

## Binary Sensors
The `tc_bus` Binary Sensor detects binary states such as doorbell presses. It can be configured to trigger based on a predefined telegram or on special cases.

### Base Options
| Option           | Description                                                                                              | Required | Default       |
|------------------|----------------------------------------------------------------------------------------------------------|----------|---------------|
| `id`             | Unique ID for the binary sensor component.                                                               | ✅ | |
| `tc_bus_id`      | ID of the related `tc_bus` instance.                                                                     | | |
| `sensor_type`    | Can be used as telegram listener. Available options: `telegram`. If set to `telegram`, the options below apply.| ✅ | `telegram`          |
| `icon`           | Icon to represent the sensor in the UI.                                                                  | | `mdi:doorbell`|
| `name`           | Name of the binary sensor.                                                                               | | `Doorbell`    |

### Sensor Type: `telegram`
This sensor type supports advanced triggering options, enabling it to respond to specific received telegrams.

::: tip NOTE
You can use **either** `telegram` **or** a combination of `type`, `address`, `payload`, and `serial_number`, but **not both** simultaneously.  
This ensures the binary sensor triggers either through a specific telegram or a combination of parameters, preventing conflicts.
:::

| Option           | Description                                                                                              | Required | Default       |
|------------------|----------------------------------------------------------------------------------------------------------|----------|---------------|
| `auto_off`       | Time period after which the sensor automatically turns off, useful for momentary signals like doorbell presses.  | | `3s`          |
| `telegram`        | A specific 32-bit hexadecimal telegram that triggers the binary sensor when received from the TC:BUS.| ✅ | `0`           |
| `type`           | Telegram type that will trigger the binary sensor, used alongside `address`, `payload` and `serial_number`. Cannot be used with `telegram`.  | ✅ | |
| `address`        | 8-bit address that serves as a condition to trigger the binary sensor. If you set it to `255`, it will catch all addresses. | | `0`           |
| `payload`        | 32-bit payload that serves as a condition to trigger the binary sensor.  | | `0`           |
| `serial_number`  | Specific device serial number that serves as a condition to trigger the binary sensor. If you set it to `255`, it will catch all serial numbers. | | `0` |

### Sensor Type: `door_readiness`
This sensor shows the door readiness state.

## Locks
The `tc_bus` Lock platform can be configured to trigger based on a predefined outdoor station.

| Option           | Description                                                                                              | Required | Default       |
|------------------|----------------------------------------------------------------------------------------------------------|----------|---------------|
| `id`             | Unique ID for the lock component.                                                                        | ✅ | |
| `tc_bus_id`      | ID of the related `tc_bus` instance.                                                                     | | |
| `icon`           | Icon to represent the lock in the UI.                                                                    | | `mdi:door` |
| `name`           | Name of the lock.                                                                                        | | `Entrance Door` |
| `auto_lock`      | Time period after which the lock resets the virtual state to `locked`.                                   | | `5s` |
| `address`        | 8-bit address that serves as a condition to trigger the lock. If you set it to `255`, it will catch all addresses. | | `0` |
| `before_unlock_action` | Defines actions to be triggered before the `open_door` telegram is sent.                           | | |
| `after_unlock_action` | Defines actions to be triggered after the `open_door` telegram is sent.                             | | |
| `lock_action`    | Defines actions to be triggered when the lock state is changed back to locked.                           | | |



## Callbacks
### Received Telegram <Badge type="tip" text="on_telegram" />
This callback allows you to utilize the [TelegramData](#telegram-data) struct, accessible as the `x` variable.

```yaml
on_telegram:
  - lambda: |-
      ESP_LOGD("TAG", "Received Telegram Type: %s", telegram_type_to_string(x.type));

      if (x.type == TELEGRAM_TYPE_OPEN_DOOR) {
        ESP_LOGD("TAG", "Opened Door of outdoor station %i", x.address);
      }
```

## Actions
### Set Programming Mode <Badge type="tip" text="tc_bus.set_programming_mode" />
This action allows you to enable or disable the programming mode of the control unit.

```yaml
on_...:
  - tc_bus.set_programming_mode:
      programming_mode: true
```

### Sending Telegrams <Badge type="tip" text="tc_bus.send" />
You can send telegrams on the bus using this action.

::: tip NOTE
You can either use the `telegram` field to send a specific telegram or use the `type`, `address`, `payload`, and `serial_number` fields to create a more complex message. **Both cannot be used at the same time**.

You can explicitly send a 32-bit telegram by using the optional `is_long` property, which is useful when the telegram begins with leading zeros.
:::

::: code-group
```yaml [Telegram Builder]
on_...:
  - tc_bus.send:
      type: open_door
      address: 0
      payload: 0
      serial_number: 123456
```

```yaml [Raw Telegram]
on_...:
  - tc_bus.send:
      telegram: 0x1A2B3C4D
```

```yaml [Raw Telegram (fixed size)]
on_...:
  - tc_bus.send:
      telegram: 0x00000280
      is_long: True
```
:::


## Example YAML Configuration
This is an example configuration for the component in ESPHome:

```yaml
external_components:
  - source: github://azoninc/doorman@master
    components: [ tc_bus ]

# TC:BUS configuration
tc_bus:
  id: my_tc_bus
  rx_pin: GPIO9
  tx_pin: GPIO8
  on_telegram:
    - logger.log: "Received telegram from bus!"

# Outdoor Stations - Doors
lock:
  - platform: tc_bus
    id: entrance_door_lock
    name: "Entrance Door"
    auto_lock: 5s
    address: 0
    before_unlock_action:
      - logger.log: "Before Unlock Action Triggered"
    after_unlock_action:
      - logger.log: "After Unlock Action Triggered"
    lock_action:
      - logger.log: "Lock Action Triggered"

text_sensor:
  - platform: tc_bus
    bus_telegram:
      name: "Last Bus Telegram"

# Binary sensor for doorbell press
binary_sensor:
  - platform: tc_bus
    id: doorbell_sensor_raw
    name: "Outdoor Station Doorbell (raw)"
    icon: "mdi:doorbell"
    telegram: 0x0C30BA80
    auto_off: 2s

  - platform: tc_bus
    id: doorbell_sensor_parser
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

# Sending telegrams
button:
  - platform: template
    name: "Open Door (raw)"
    on_press:
      - tc_bus.send:
          telegram: 0x1100

  - platform: template
    name: "Open Door (builder)"
    on_press:
      - tc_bus.send:
          type: open_door
          address: 0
```

## Telegram Data
The `TelegramData` struct is used internally and can also be used in the `on_telegram`.

```c++
struct TelegramData {
    uint32_t raw;

    bool is_long;
    bool is_response;
    bool is_retransmission;

    char hex[9];

    TelegramType type;
    uint8_t address;
    uint32_t serial_number;
    uint32_t payload;
};
```

## Telegram Types
You can use telegram types in binary sensors and also when [sending telegrams](#sending-telegrams):

- door_call <Badge type="tip" text="TELEGRAM_TYPE_DOOR_CALL" />
- floor_call <Badge type="tip" text="TELEGRAM_TYPE_FLOOR_CALL" />
- internal_call <Badge type="tip" text="TELEGRAM_TYPE_INTERNAL_CALL" />
- control_function <Badge type="tip" text="TELEGRAM_TYPE_CONTROL_FUNCTION" />
- start_talking_door_call <Badge type="tip" text="TELEGRAM_TYPE_START_TALKING_DOOR_CALL" />
- start_talking <Badge type="tip" text="TELEGRAM_TYPE_START_TALKING" />
- stop_talking_door_call <Badge type="tip" text="TELEGRAM_TYPE_STOP_TALKING_DOOR_CALL" />
- stop_talking <Badge type="tip" text="TELEGRAM_TYPE_STOP_TALKING" />
- open_door <Badge type="tip" text="TELEGRAM_TYPE_OPEN_DOOR" />
- open_door_long <Badge type="tip" text="TELEGRAM_TYPE_OPEN_DOOR_LONG" />
- light <Badge type="tip" text="TELEGRAM_TYPE_LIGHT" />
- door_opened <Badge type="tip" text="TELEGRAM_TYPE_DOOR_OPENED" />
- door_closed <Badge type="tip" text="TELEGRAM_TYPE_DOOR_CLOSED" />
- end_of_ringtone <Badge type="tip" text="TELEGRAM_TYPE_END_OF_RINGTONE" />
- end_of_door_readiness <Badge type="tip" text="TELEGRAM_TYPE_END_OF_DOOR_READINESS" />
- initialize_door_station <Badge type="tip" text="TELEGRAM_TYPE_INITIALIZE_DOOR_STATION" />
- reset <Badge type="tip" text="TELEGRAM_TYPE_RESET" />
- select_device_group <Badge type="tip" text="TELEGRAM_TYPE_SELECT_DEVICE_GROUP" />
- select_device_group_reset <Badge type="tip" text="TELEGRAM_TYPE_SELECT_DEVICE_GROUP_RESET" />
- search_devices <Badge type="tip" text="TELEGRAM_TYPE_SEARCH_DEVICES" />
- found_device <Badge type="tip" text="TELEGRAM_TYPE_FOUND_DEVICE" />
- found_device_subsystem <Badge type="tip" text="TELEGRAM_TYPE_FOUND_DEVICE_SUBSYSTEM" />
- programming_mode <Badge type="tip" text="TELEGRAM_TYPE_PROGRAMMING_MODE" />
- read_memory_block <Badge type="tip" text="TELEGRAM_TYPE_READ_MEMORY_BLOCK" />
- select_memory_page <Badge type="tip" text="TELEGRAM_TYPE_SELECT_MEMORY_PAGE" />
- select_device_memory_page <Badge type="tip" text="TELEGRAM_TYPE_SELECT_DEVICE_MEMORY_PAGE" />
- write_memory <Badge type="tip" text="TELEGRAM_TYPE_WRITE_MEMORY" />
- request_version <Badge type="tip" text="TELEGRAM_TYPE_REQUEST_VERSION" />
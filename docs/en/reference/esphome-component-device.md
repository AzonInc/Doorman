---
description: Complete documentation for the TC:BUS Device ESPHome component, including setup, configuration, and usage details.
---

<script setup>
  import { data } from '../../loader/capabilities.data.js'

  const classicIndoorStations = data.filter(m => m.deviceGroup === 0);
  const handsfreeIndoorStations = data.filter(m => m.deviceGroup === 1);
  const outdoorStations = data.filter(m => m.deviceGroup === 2);
  const controllers = data.filter(m => m.deviceGroup === 4);
  const functionalExtensions = data.filter(m => m.deviceGroup === 6);
  const formatCaps = (caps) => {
    return caps.length == 0 ? 'None' : caps.map(c => `<code>${c}</code>`).join(', ');
  }
</script>

# TC:BUS Device Component <Badge type="tip" text="tc_bus_device" />
This component extends the [TC:BUS](./esphome-component) base component and simplifies communication with individual devices on the bus. It allows you to identify devices, access their memory, and read or modify their settings. See the section on [supported models and settings](#model-setting-availability) for details.

The component also supports creating **virtual bus devices**.

## Configuration
The `tc_bus_device` component offers the following configuration options:

| Option                    | Description                                                                                                                                   | Required | Default |
|---------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------|----------|---------|
| `id`                      | Unique ID for the component.                                                                                                                  | ✅ | |
| `type`                    | Device Group of the TC:BUS Device. E.g. Indoor Station, Outdoor Station.                                                                      | ✅ | |
| `virtual`                 | Enables the virtual device mode, assigning it a unique serial number/address based on the host MAC address so it can emulate or interact with other devices on the TC:BUS. | | `False` |
| **Physical Device**       | | | |
| `auto_configuration`      | When enabled, the component [automatically identifies](#automatic-configuration) the device using the serial number and reads device memory based on the device model. | | `False` |
| `on_read_memory_complete` | Defines actions to be triggered when the memory reading is complete. | | |
| `on_read_memory_failed`  | Defines actions to be triggered when the memory reading fails. | | |
| `on_identify_complete`    | Defines actions to be triggered when the identification of the indoor station is complete. Returns a `ModelData` object as the `x` variable.  | | |
| `on_identify_unknown`     | Defines actions to be triggered when the identification of the indoor station completes with unknown model.                                   | | |
| `on_identify_failed`     | Defines actions to be triggered when the identification of the indoor station times out.                                                      | | |
| **Virtual Device**        | | | |
| `on_incoming_call`        | Defines actions to be triggered when the virtual device receives a call. Returns a `TelegramData` object as the `x` variable.                 | | |
| `on_call_started`         | Defines actions to be triggered when the virtual device is connected to a call. Returns a `TelegramData` object as the `x` variable.          | | |
| `on_call_ended`           | Defines actions to be triggered when the virtual device is disconnected from a call. Returns a `TelegramData` object as the `x` variable.     | | |
| `on_call_failed`          | Defines actions to be triggered when the virtual device fails to call another indoor or outdoor station.                                      | | |

## Automatic Configuration <Badge type="warning" text="Only physical" />
If you enable the `auto_configuration` option, the component automatically attempts to identify the device model as soon as the serial number is set.
Once the model is successfully identified, the component will automatically read the device memory to retrieve its current configuration and state.

## Number Inputs
The `tc_bus_device` Number Input platform offers the following configuration options:

| Option                         | Description                                                                                                   | Required | Default       |
|--------------------------------|---------------------------------------------------------------------------------------------------------------|----------|---------------|
| `tc_bus_device_id`             | ID of the related `tc_bus_device` instance.                                                                   | ✅ | |
| `serial_number`                | Serial Number Input to set the serial number of the predefined bus device.                                    | ✅ | |
| `parallel_serial_number`       | Parallel Serial Number Input to set the parallel serial number of the predefined bus device.                  | | |
| `address`                      | Address Number Input to set the address for the outdoor station.                                              | | |
| `volume_handset_door_call`     | Door Call Handset Volume Number Input to set the handset volume for door calls of the indoor station.         | | |
| `volume_handset_internal_call` | Internal Call Handset Volume Number Input to set the handset volume for internal calls of the indoor station. | | |
| `volume_ringtone`              | Ringtone Volume Number Input to set the ringtone volume of the indoor station.                                | | |
| `address_divider`              | Address Divider Input to set the address divider for the second outdoor station on the indoor station.        | | |
| `door_readiness_duration`      | Door Readiness Duration Number Input to set the door readiness duration of the outdoor station.               | | |
| `call_time_duration`           | Call Time Duration Number Input to set the call time duration of the outdoor station.                         | | |
| `door_opener_duration`         | Door Opener Duration Number Input to set the door opener duration of the outdoor station.                     | | |


## Select Inputs
The `tc_bus_device` Select platform offers the following configuration options:

| Option                               | Description                                                                                                    | Required | Default       |
|--------------------------------------|----------------------------------------------------------------------------------------------------------------|----------|---------------|
| `tc_bus_device_id`                   | ID of the related `tc_bus_device` instance.                                                                    | ✅ | |
| `model`                              | Model Select to set the model of the device (used to read and write settings). Take a look at the [supported models and settings](#model-setting-availability).| ✅ | |
| `ringtone_entrance_door_call`        | Select to set the entrance door call ringtone of the indoor station. | | |
| `ringtone_second_entrance_door_call` | Select to set the second entrance door call ringtone of the indoor station. | | |
| `ringtone_floor_call`                | Select to set the floor call ringtone of the indoor station. | | |
| `ringtone_internal_call`             | Select to set the internal call ringtone of the indoor station. | | |


## Switches
The `tc_bus_device` Switch platform offers the following configuration options:

| Option                   | Description                                                                                                    | Required | Default       |
|--------------------------|----------------------------------------------------------------------------------------------------------------|----------|---------------|
| `tc_bus_device_id`       | ID of the related `tc_bus_device` instance.                                                                    | ✅ | |
| `use_long_door_opener` | Always forces sending the long door opener telegram, including the serial number, instead of the short one. | | |
| `ringtone_mute`          | Switch to mute the ringtone of the  indoor station. | | |
| `auto_answer_call`       | Switch to set the auto answer call setting of the indoor station. | | |
| `address_lock`           | Switch to lock the address of the outdoor station. | | |
| `calling_requires_door_readiness`     | Switch to set the requirement of door readiness for calls to the outdoor station. | | |
| `door_opener_requires_door_readiness` | Switch to set the requirement of door readiness for opening the entrance door. | | |
| `door_opener_requires_active_call`    | Switch to set the requirement of an active call for opening the entrance door. | | |

## Buttons
The `tc_bus_device` Button platform offers the following configuration options:

| Option                | Description                                                                                       | Required | Default       |
|-----------------------|---------------------------------------------------------------------------------------------------|----------|---------------|
| `tc_bus_device_id`    | ID of the related `tc_bus_device` instance.                                                       | ✅ | |
| `identify_device`     | This starts the identification process to determine the device model by using it's serial number. | | |
| `read_memory`         | This reads the device memory if supported into a memory buffer. Take a look at the [supported models and settings](#model-setting-availability). | | |

## Binary Sensors
The `tc_bus_device` Binary Sensor detects binary states such as doorbell presses. It can be configured to trigger based on a predefined telegram or on special cases.

### Base Options
| Option           | Description                                                                                              | Required | Default       |
|------------------|----------------------------------------------------------------------------------------------------------|----------|---------------|
| `id`             | Unique ID for the binary sensor component.                                                               | ✅ |               |
| `tc_bus_device_id` | ID of the related `tc_bus_device` instance.                                                            | ✅ |               |
| `sensor_type`    | Can be used as telegram listener or door opener state for virtual outdoor stations. Available options: `telegram`, `virtual_door_opener`. If set to `telegram`, the options below apply.| ✅ | `telegram`          |
| `icon`           | Icon to represent the sensor in the UI.                                                                  | | `mdi:doorbell`|
| `name`           | Name of the binary sensor.                                                                               | | `Doorbell`    |

### Sensor Type: `telegram`
This sensor type supports advanced triggering options, enabling it to respond to specific received telegrams.

| Option           | Description                                                                                              | Required | Default       |
|------------------|----------------------------------------------------------------------------------------------------------|----------|---------------|
| `auto_off`       | Time period after which the sensor automatically turns off, useful for momentary signals like doorbell presses.  | | `3s`          |
| `type`           | Telegram type that will trigger the binary sensor, used alongside `address` and `payload`.               | ✅ |      |
| `address`        | 8-bit address that serves as a condition to trigger the binary sensor. If you set it to `255`, it will catch all addresses. | | `0`           |
| `payload`        | 32-bit payload that serves as a condition to trigger the binary sensor.                                  | | `0`           |

### Sensor Type: `virtual_door_opener`
This sensor type replaces the physical door opener when using a virtual outdoor station.
It allows you to trigger a relay connected to a buzzer or door release mechanism to open the door. The activation duration follows the door opener time configured for the virtual outdoor station.

## Callbacks
### Read Memory Complete <Badge type="tip" text="on_read_memory_complete" /> <Badge type="warning" text="Only physical" />
This callback allows you to detect a successful memory reading.

```yaml
on_read_memory_complete:
  - logger.log: "Completed memory reading!"
```

### Read Memory Failed <Badge type="tip" text="on_read_memory_failed" /> <Badge type="warning" text="Only physical" />
This callback allows you to detect a failed memory reading. Most probably when a model doesn't support the related telegrams.

```yaml
on_read_memory_failed:
  - logger.log: "Failed to read Memory"
```

### Device Identification Complete <Badge type="tip" text="on_identify_complete" /> <Badge type="warning" text="Only physical" />
This callback allows you to utilize the [ModelData](#model-data) struct, accessible as the `x` variable.

```yaml
on_identify_complete:
  - logger.log: "Completed identification!"
  - lambda: |-
      std::string hexString = str_upper_case(format_hex(x));
      ESP_LOGI("tc_bus", "Memory Dump: %s", hexString.c_str());
```

### Device Identification Complete (Unknown) <Badge type="tip" text="on_identify_unknown" /> <Badge type="warning" text="Only physical" />
This callback allows you to detect an unknown model identification of the device. Most probably when a model is too old and doesn't support this process or is not implemented yet.

```yaml
on_identify_unknown:
  - logger.log: "Failed to identify device - unknown model!"
```

### Device Identification Failed <Badge type="tip" text="on_identify_failed" /> <Badge type="warning" text="Only physical" />
This callback allows you to detect a failed identification of the device. Most probably when a model is too old and doesn't support this process.

```yaml
on_identify_failed:
  - logger.log: "Failed to identify device!"
```

### Incoming Call <Badge type="tip" text="on_incoming_call" /> <Badge type="warning" text="Only virtual" />
Triggered when a call is received from an indoor or outdoor station. The [TelegramData](./esphome-component#telegram-data) struct is available as `x`.

```yaml
on_incoming_call:
  - lambda: |-
      if(x.type == TELEGRAM_TYPE_DOOR_CALL)
      {
        ESP_LOGI("tc_bus", "Incoming call from outdoor station %i", x.address);
      }
      else
      {
        ESP_LOGI("tc_bus", "Incoming call from indoor station %i", x.serial_number);
      } 
```

### Call Started <Badge type="tip" text="on_call_started" /> <Badge type="warning" text="Only virtual" />
This callback allows you to detect when the call is connected to the virtual device. The [TelegramData](./esphome-component#telegram-data) struct is available as `x`.

```yaml
on_call_started:
  - lambda: |-
      if(x.type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL)
      {
        ESP_LOGI("tc_bus", "Connected to outdoor station %i", x.address);
      }
      else
      {
        ESP_LOGI("tc_bus", "Connected to indoor station %i", x.serial_number);
      } 
```

### Call Ended <Badge type="tip" text="on_call_ended" /> <Badge type="warning" text="Only virtual" />
This callback allows you to detect when the call is disconnected from the virtual device. The [TelegramData](./esphome-component#telegram-data) struct is available as `x`.

```yaml
on_call_started:
  - lambda: |-
      if(x.type == TELEGRAM_TYPE_START_TALKING_DOOR_CALL)
      {
        ESP_LOGI("tc_bus", "Disconnected (AS) Address: %i SN: %i", x.address, x.serial_number);
      }
      else
      {
        ESP_LOGI("tc_bus", "Disconnected (IS) Address: %i SN: %i", x.address, x.serial_number);
      } 
```

### Call Failed <Badge type="tip" text="on_call_failed" /> <Badge type="warning" text="Only virtual" />
This callback allows you to detect when the incoming or outgoing call failed.

```yaml
on_call_failed:
  - logger.log: "Call failed!"
```

## Actions
### Read Memory <Badge type="tip" text="tc_bus_device.read_memory" /> <Badge type="warning" text="Only physical" />
This action allows you to read the memory of any supported device on the bus.

```yaml
on_...:
  - tc_bus_device.read_memory:
      id: my_tc_bus_indoor_station_device
```

### Identify devices <Badge type="tip" text="tc_bus_device.identify" /> <Badge type="warning" text="Only physical" />
This action allows you to automatically detect the model of a supported device on the bus.

::: tip Note
Automatic identification is not available on all devices. At present, only indoor stations are fully supported in the identification process.
Nonetheless, unsupported devices may still return identification data. If your device does so, please submit the response data as an issue so it can be included in future updates.

If no identification response is received, the device does not support the identification protocol. This usually applies to older hardware that predates this feature. In such cases, you will need to manually configure the device model.
:::

```yaml
on_...:
  - tc_bus_device.identify:
      id: my_tc_bus_indoor_station_device
```

### Update Settings <Badge type="tip" text="tc_bus_device.update_setting" />
This action allows you to change the supported settings of any supported device on the bus.
Take a look at the [supported models and settings](#model-setting-availability).

```yaml
on_...:
  - tc_bus_device.update_setting:
      id: my_tc_bus_indoor_station_device
      type: volume_ringtone
      value: 7
```

### Update Doorbell Buttons <Badge type="tip" text="tc_bus_device.update_doorbell_button" /> <Badge type="warning" text="Only physical" />
This action allows you to configure the physical doorbell buttons of an outdoor station device.

::: warning EXPERIMENTAL
This feature is experimental, use with caution.
:::

#### Supported actions:

- `none` — No action is assigned.
- `door_call` — Triggers a door call.
- `light` — Controls the light function.
- `control_function` — Executes a custom control function.

If no action is specified, the default value is `none`.
If no payload is provided, it remains unset.

For matrix systems, you can use the `button_col` parameter. For other systems, this parameter is not required.

:::code-group
```yaml [Rows only]
on_...:
  - tc_bus_device.update_doorbell_button:
      id: my_tc_bus_outdoor_station_device
      button_row: 1
      primary_action: door_call
      primary_payload: 798906
      secondary_action: control_function
      secondary_payload: 8
```
```yaml [Matrix]
on_...:
  - tc_bus_device.update_doorbell_button:
      id: my_tc_bus_outdoor_station_device
      button_row: 2
      button_col: 4
      primary_action: door_call
      primary_payload: 798906
      secondary_action: control_function
      secondary_payload: 8
```
:::

### Open Door <Badge type="tip" text="tc_bus_device.open_door" />
This action allows you to activate the door opener for **virtual outdoor stations** and automatically send the appropriate door opener command to indoor stations, based on the current door readiness state and the associated outdoor station address.

```yaml
on_...:
  - tc_bus_device.open_door:
      id: my_tc_bus_indoor_station_device
```

### Answer Call <Badge type="tip" text="tc_bus_device.answer_call" /> <Badge type="warning" text="Only virtual" />
This action allows you to answer an incoming call.

```yaml
on_...:
  - tc_bus_device.answer_call:
      id: my_virtual_tc_bus_indoor_station_device
```

### End Call <Badge type="tip" text="tc_bus_device.end_call" /> <Badge type="warning" text="Only virtual" />
This action allows you to end the current call.

```yaml
on_...:
  - tc_bus_device.end_call:
      id: my_virtual_tc_bus_indoor_station_device
```

### Initiate Call <Badge type="tip" text="tc_bus_device.call" /> <Badge type="warning" text="Only virtual" />
This action allows you to end the current call.

:::code-group
```yaml [Talk to AS]
# This initiates a call to the outdoor station
# with address 0 from the virtual indoor station
on_...:
  - tc_bus_device.call:
      id: my_virtual_tc_bus_indoor_station_device
      address: 0
```
```yaml [Door Call to SN]
# This initiates a call to the indoor station
# with serial number 123456 from the virtual outdoor station
on_...:
  - tc_bus_device.call:
      id: my_virtual_tc_bus_outdoor_station_device
      address: 123456
```
```yaml [Internal Call to SN]
# This initiates a call to the indoor station
# with serial number 123456 from the virtual indoor station
on_...:
  - tc_bus_device.call:
      id: my_virtual_tc_bus_indoor_station_device
      address: 123456
      internal: true
```
:::

### Sending Telegrams <Badge type="tip" text="tc_bus_device.send" />
You can send device related telegrams on the bus using this action.

```yaml
on_...:
  - tc_bus_device.send:
      id: my_tc_bus_indoor_station_device
      type: open_door
      address: 0
      payload: 0
```

## Example YAML Configuration
This is an example configuration for the component in ESPHome:

:::code-group
```yaml [Physical Indoor Station]
external_components:
  - source: github://azoninc/doorman@master
    components: [ tc_bus, tc_bus_device ]

# TC:BUS configuration
tc_bus:
  rx_pin: GPIO9
  tx_pin: GPIO8

# TC:BUS Device configuration
tc_bus_device:
  - id: my_tc_bus_indoor_station_device
    type: indoor_station
    auto_configuration: true
    # Optional
    on_read_memory_complete:
      - logger.log: "Memory reading successful!"
    on_read_memory_failed:
      - logger.log:
          format: "Memory reading failed!"
          level: ERROR
    on_identify_complete:
      - logger.log:
          format: "Identified Hardware: %s (v%i) | Firmware: %i.%i.%i"
          args: [ 'model_to_string(x.model)', 'x.hardware_version', 'x.firmware_major', 'x.firmware_minor', 'x.firmware_patch' ]
          level: INFO
    on_identify_unknown:
      - logger.log: "Setup: Unable to identify the Indoor Station, unknown model."
    on_identify_failed:
      - logger.log:
          format: "Failed to identify the Indoor Station. Please select it manually."
          level: ERROR

number:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_indoor_station_device
    serial_number:
      name: "Serial Number"
    # Optional
    parallel_serial_number:
      name: "Parallel Serial Number"
    volume_ringtone:
      name: "Volume: Ringtone"
    volume_handset_door_call:
      name: "Volume: Handset Door Call"
    volume_handset_internal_call:
      name: "Volume: Handset Internal Call"

select:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_indoor_station_device
    model:
      name: "Model"
    # Optional
    ringtone_entrance_door_call:
      name: "Ringtone: Entrance Door Call"
    ringtone_second_entrance_door_call:
      name: "Ringtone: Second Entrance Door Call"
    ringtone_floor_call:
      name: "Ringtone: Floor Call"
    ringtone_internal_call:
      name: "Ringtone: Internal Call"

switch:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_indoor_station_device
    # Optional
    use_long_door_opener:
      name: "Enforce long Door Opener Telegram"
    ringtone_mute:
      name: "Ringtone: Mute"
    call_time_unlimited:
      name: "Call Time Unlimited"

button:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_indoor_station_device
    # Optional
    identify_device:
      name: "Identify Device"
    read_memory:
      name: "Read Memory"

binary_sensor:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_indoor_station_device
    id: doorbell_sensor
    name: "Outdoor Station Doorbell (parser)"
    icon: "mdi:doorbell"
    type: door_call
```
```yaml [Virtual Indoor Station]
external_components:
  - source: github://azoninc/doorman@master
    components: [ tc_bus, tc_bus_device ]

# TC:BUS configuration
tc_bus:
  rx_pin: GPIO9
  tx_pin: GPIO8

# TC:BUS Device configuration
tc_bus_device:
  - id: my_tc_bus_indoor_station_device
    type: indoor_station
    virtual: true
    # Optional
    on_incoming_call:
      - logger.log: "Incoming call from indoor- or outdoor station"
    on_call_started:
      - logger.log: "Call started, bus is in audio mode"
    on_call_ended:
      - logger.log: "Call ended"
    on_call_failed:
      - logger.log: "Call failed due to timeout or missing response"

number:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_indoor_station_device
    serial_number:
      name: "Serial Number"
    # Optional
    parallel_serial_number:
      name: "Parallel Serial Number"
    # If the calling address is > address divider,
    # it's the second outdoor station, otherwise the first.
    address_divider:
      name: "AS Address Divider"
    # 8 - 120 sec. (0 is unlimited)
    call_time_duration:
      name: "Maximum call duration"

select:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_indoor_station_device
    model:
      name: "Model"

switch:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_indoor_station_device
    # Optional
    use_long_door_opener:
      name: "Always use long door opener Telegram"
    auto_answer_call:
      name: "Automatically answer calls"
    call_time_unlimited:
      name: "Call Time Unlimited"
```
```yaml [Virtual Outdoor Station]
external_components:
  - source: github://azoninc/doorman@master
    components: [ tc_bus, tc_bus_device ]

# TC:BUS configuration
tc_bus:
  rx_pin: GPIO9
  tx_pin: GPIO8

# TC:BUS Device configuration
tc_bus_device:
  - id: my_tc_bus_outdoor_station_device
    type: outdoor_station
    virtual: true
    # Optional
    on_incoming_call:
      - logger.log: "Incoming call from indoor station"
    on_call_started:
      - logger.log: "Call started, bus is in audio mode"
    on_call_ended:
      - logger.log: "Call ended"
    on_call_failed:
      - logger.log: "Call failed due to timeout or missing response"

number:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_outdoor_station_device
    serial_number:
      name: "Serial Number"
    # Optional
    # With a single outdoor station the address is usually 0
    address:
      name: "Address"
    # 8 - 120 sec. (0 is unlimited)
    call_time_duration:
      name: "Maximum call duration"
    # 8 - 120 sec. (0 is unlimited)
    door_readiness_duration:
      name: "Door readiness duration"
    # 0 - 15 sec.
    door_opener_duration:
      name: "Door opener duration"

select:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_outdoor_station_device
    model:
      name: "Model"

switch:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_outdoor_station_device
    # Optional
    address_lock:
      name: "Address Lock"
    calling_requires_door_readiness:
      name: "Calling required door readiness"
    door_opener_requires_active_call:
      name: "Door Opener requires active call"
    door_opener_requires_door_readiness:
      name: "Door Opener requires door readiness"

binary_sensor:
  - platform: tc_bus_device
    tc_bus_device_id: my_tc_bus_outdoor_station_device
    sensor_type: virtual_door_opener
    name: "Door Opener Active"
    on_press:
      - logger.log: Turn on door opener
    on_release:
      - logger.log: Turn off door opener
```
:::

## Advanced Configuration

### Accessing device settings
If you need to access the supported settings in the memory buffer you can use the `get_setting` and `update_setting` methods of the `tc_bus_device` instance. Take a look at the [setting types](#setting-types).

Example (read and write setting):
```yaml
button:
  - platform: template
    name: "Read Handset volume via lambda"
    on_press:
      - lambda: |-
          ESP_LOGD("TAG", "Handset volume: %i", id(my_tc_bus_indoor_station_device)->get_setting(SETTING_VOLUME_HANDSET_DOOR_CALL));

  - platform: template
    name: "Set Handset volume via lambda"
    on_press:
      - lambda: |-
          id(my_tc_bus_indoor_station_device)->update_setting(SETTING_VOLUME_HANDSET_DOOR_CALL, 7);

  - platform: template
    name: "Set Handset volume via action"
    on_press:
      - tc_bus_device.update_setting:
          id: my_tc_bus_indoor_station_device
          type: volume_handset_door_call
          value: 7
```

### Accessing outdoor station doorbell button configuration
If you need to access the doorbell button configuration of an outdoor station in the memory buffer you can use the `get_doorbell_button` and `update_doorbell_button` methods of the `tc_bus_device` instance. Take a look at the [doorbell button actions](#doorbell-button-actions).

Example (read and write doorbell button actions):
```yaml
button:
  - platform: template
    name: "Read Actions of Doorbell Button 1 via lambda"
    on_press:
      - lambda: |-
          DoorbellButtonAction button = id(my_tc_bus_outdoor_station_device)->get_doorbell_button(1);
          ESP_LOGD("TAG", "Primary Action: %x", button.primary_action);
          ESP_LOGD("TAG", "Primary Payload: %x", button.primary_payload);
          ESP_LOGD("TAG", "Secondary Action: %x", button.secondary_action);
          ESP_LOGD("TAG", "Secondary Payload: %x", button.secondary_payload);

  - platform: template
    name: "Set Doorbell Button 1 via lambda"
    on_press:
      - lambda: |-
          DoorbellButtonAction button;
          button.primary_action = DOORBELL_BUTTON_ACTION_DOOR_CALL;
          button.primary_payload = 123456;

          id(my_tc_bus_outdoor_station_device)->update_doorbell_button(1, button);

  - platform: template
    name: "Set Doorbell Button 1 via action"
    on_press:
      - tc_bus_device.update_doorbell_button:
          id: my_tc_bus_outdoor_station_device
          button_row: 1
          primary_action: door_call
          primary_payload: 123456

  - platform: template
    name: "Set Matrix Button 1;5 via action"
    on_press:
      - tc_bus_device.update_doorbell_button:
          id: my_tc_bus_outdoor_station_device
          button_row: 1
          button_col: 5
          primary_action: door_call
          primary_payload: 123456

  - platform: template
    name: "Reset Doorbell Button 1 via action"
    on_press:
      - tc_bus_device.update_doorbell_button:
          id: my_tc_bus_outdoor_station_device
          button_row: 1
```

## Doorbell Button Data
The `DoorbellButtonConfig` struct is used internally.

```c++
struct DoorbellButtonConfig
 {
    DoorbellButtonAction primary_action = DOORBELL_BUTTON_ACTION_NONE;
    uint32_t primary_payload = DOORBELL_BUTTON_UNASSIGNED;
    DoorbellButtonAction secondary_action = DOORBELL_BUTTON_ACTION_NONE;
    uint32_t secondary_payload = DOORBELL_BUTTON_UNASSIGNED;
};
```

## Model Data
The `ModelData` struct is used internally in the identification process.

```c++
struct ModelData {
    Model model = MODEL_NONE;

    uint32_t firmware_version = 0;
    uint8_t firmware_major = 0;
    uint8_t firmware_minor = 0;
    uint8_t firmware_patch = 0;
    uint8_t hardware_version = 0;
    
    uint8_t device_group = 0;
    uint8_t memory_size = 0;
    uint32_t capabilities = 0;
};
```

## Doorbell Button Actions
Here are the available doorbell button actions you can use to update the doorbell button configuration of your outdoor station:

- none <Badge type="tip" text="DOORBELL_BUTTON_ACTION_NONE" />
- light <Badge type="tip" text="DOORBELL_BUTTON_ACTION_LIGHT" />
- door_call <Badge type="tip" text="DOORBELL_BUTTON_ACTION_DOOR_CALL" />
- control_function <Badge type="tip" text="DOORBELL_BUTTON_ACTION_CONTROL_FUNCTION" />

## Setting Types
Here are the available setting types you can use to update the settings of your bus devices.

### Indoor Station
- ringtone_mute <Badge type="tip" text="SETTING_RINGTONE_MUTE" />
- ringtone_floor_call <Badge type="tip" text="SETTING_RINGTONE_FLOOR_CALL" />
- ringtone_entrance_door_call <Badge type="tip" text="SETTING_RINGTONE_ENTRANCE_DOOR_CALL" />
- ringtone_second_entrance_door_call <Badge type="tip" text="SETTING_RINGTONE_SECOND_ENTRANCE_DOOR_CALL" />
- ringtone_internal_call <Badge type="tip" text="SETTING_RINGTONE_INTERNAL_CALL" />
- volume_ringtone <Badge type="tip" text="SETTING_VOLUME_RINGTONE" />
- volume_handset_door_call <Badge type="tip" text="SETTING_VOLUME_HANDSET_DOOR_CALL" />
- volume_handset_internal_call <Badge type="tip" text="SETTING_VOLUME_HANDSET_INTERNAL_CALL" />
- as_address_divider <Badge type="tip" text="SETTING_AS_ADDRESS_DIVIDER" />
- vas_address_divider <Badge type="tip" text="SETTING_VAS_ADDRESS_DIVIDER" />
- use_long_door_opener_protocol <Badge type="tip" text="SETTING_USE_LONG_DOOR_OPENER_PROTOCOL" />
- ambient_light_in_standby <Badge type="tip" text="SETTING_AMBIENT_LIGHT_IN_STANDBY" />
- call_time_unlimited <Badge type="tip" text="SETTING_CALL_TIME_UNLIMITED" />
- auto_answer_call <Badge type="tip" text="SETTING_AUTO_ANSWER_CALL" />

### Outdoor Station
- address <Badge type="tip" text="SETTING_ADDRESS" />
- address_lock <Badge type="tip" text="SETTING_ADDRESS_LOCK" />
- button_rows <Badge type="tip" text="SETTING_BUTTON_ROWS" />
- calling_requires_door_readiness <Badge type="tip" text="SETTING_CALLING_REQUIRES_DOOR_READINESS" />
- door_opener_requires_door_readiness <Badge type="tip" text="SETTING_DOOR_OPENER_REQUIRES_DOOR_READINESS" />
- door_opener_requires_active_call <Badge type="tip" text="SETTING_DOOR_OPENER_REQUIRES_ACTIVE_CALL" />
- door_opener_duration <Badge type="tip" text="SETTING_DOOR_OPENER_DURATION" />
- door_readiness_duration <Badge type="tip" text="SETTING_DOOR_READINESS_DURATION" />
- call_time_duration <Badge type="tip" text="SETTING_CALL_TIME_DURATION" />
- has_code_lock <Badge type="tip" text="SETTING_HAS_CODE_LOCK" />

## Model Setting Availability
In general, **all listed models** are supported.
However, some support configuration directly via Doorman, while others don't.

### Classic Indoor Stations
Below is a list of available settings for specific classic indoor station models:

<table>
  <thead>
    <tr>
      <th>Model</th>
      <th>Available settings</th>
    </tr>
  </thead>
  <tbody>
    <tr v-for="m in classicIndoorStations" :key="m.enum">
      <td>{{ m.name }}</td>
      <td v-html="formatCaps(m.caps)"></td>
    </tr>
  </tbody>
</table>

### Handsfree Indoor Stations
Below is a list of available settings for specific handsfree indoor station models:

<table>
  <thead>
    <tr>
      <th>Model</th>
      <th>Available settings</th>
    </tr>
  </thead>
  <tbody>
    <tr v-for="m in handsfreeIndoorStations" :key="m.enum">
      <td>{{ m.name }}</td>
      <td v-html="formatCaps(m.caps)"></td>
    </tr>
  </tbody>
</table>

### Outdoor Stations
Below is a list of available settings for specific outdoor station models:

<table>
  <thead>
    <tr>
      <th>Model</th>
      <th>Available settings</th>
    </tr>
  </thead>
  <tbody>
    <tr v-for="m in outdoorStations" :key="m.enum">
      <td>{{ m.name }}</td>
      <td v-html="formatCaps(m.caps)"></td>
    </tr>
  </tbody>
</table>

### Controllers / Power Supplies
Below is a list of available settings for specific controller models:

<table>
  <thead>
    <tr>
      <th>Model</th>
      <th>Available settings</th>
    </tr>
  </thead>
  <tbody>
    <tr v-for="m in controllers" :key="m.enum">
      <td>{{ m.name }}</td>
      <td v-html="formatCaps(m.caps)"></td>
    </tr>
  </tbody>
</table>

### Functional Extensions
Below is a list of available settings for specific functional extension models:

<table>
  <thead>
    <tr>
      <th>Model</th>
      <th>Available settings</th>
    </tr>
  </thead>
  <tbody>
    <tr v-for="m in functionalExtensions" :key="m.enum">
      <td>{{ m.name }}</td>
      <td v-html="formatCaps(m.caps)"></td>
    </tr>
  </tbody>
</table>
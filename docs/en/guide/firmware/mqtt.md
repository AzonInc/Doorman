## MQTT Firmware

When using the MQTT firmware, various topics are published to your broker. Here's how the topic structure and controls work.

### Topic Structure
Each entity publishes its state to a topic in the following format:
```
<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/state
```

You can control certain entities by publishing a command to this topic format:
```
Topic: <TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/command  
Payload: ON or OFF or whatever is supported
```

### Example
To enable or disable the [Ring-To-Open](../automation/ring-to-open.md) automation, send `ON` or `OFF` as the payload to the topic:  
```
Topic: doorman-s3/switch/ring_to_open/command  
Payload: ON
```

### Special Topics
Certain special topics allow for advanced commands.

#### Send a hexadecimal Command
To send a hexadecimal command to the bus, use the topic `doorman-s3/send_raw_command ` with a JSON payload. Here's an example payload:
```json
{
    "command": 0x1C30BA80,
    "is_long": false
}
```

#### Send a parsed Command
To send a command via command builder to the bus, use the topic `doorman-s3/send_command` with a JSON payload. Here's an example payload:
```json
{
    "type": "open_door",
    "address": 0,
    "payload": 0,
    "serial_number": 123456
}
```
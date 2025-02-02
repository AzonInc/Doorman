## MQTT

When using the MQTT firmware, various topics are published to your broker. Here's how the topic structure and controls work.

### Topic Structure
Each entity publishes its state to a topic in the following format:
```
<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/state
```

You can control certain entities by publishing a command to this topic format:
::: code-group
``` [Topic]
<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/command 
```
``` [Payload]
ON or OFF or whatever is supported
```
:::

### Example
To enable or disable the [Ring-To-Open](../automation/ring-to-open.md) automation, send `ON` or `OFF` as the payload to the topic:
::: code-group
``` [Topic]
doorman-s3/switch/ring_to_open/command
```
``` [Payload]
ON
```
:::

### Special Topics
Certain special topics allow for advanced commands.

#### Send a Command (Hexadecimal)
Here's an example of how to send a hexadecimal command (uint32) to the bus:
::: code-group
``` [Topic]
doorman-s3/send_raw_command
```
```json [Payload]
{
    "command": 0x1C30BA80
}
```
```json [Advanced Payload]
{
    "command": 0x1C30BA80,
    "is_long": false
}
```
:::

#### Send a Command (Command Builder)
Here's an example of how to use the command builder to send a command to the bus:
::: code-group
``` [Topic]
doorman-s3/send_command
```
```json [Payload]
{
    "type": "open_door",
    "address": 0,
    "payload": 0,
    "serial_number": 123456
}
```
:::
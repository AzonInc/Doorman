# MQTT Integration

This page provides everything you need — from configuration to practical examples using MQTT topics and payloads.

## Setup
<!--@include: ./instructions/mqtt.md-->

## Topics
Various topics are published to your broker. Here's how the topic structure and controls work.

### Structure
Each entity publishes its state to a topic in the following format:
```
<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/state
```

You can control certain entities by publishing a telegram to this topic format:
::: code-group
``` [Topic]
<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/telegram 
```
``` [Payload]
ON or OFF or whatever is supported
```
:::

### Example
To enable or disable the [Ring-To-Open](../features/ring-to-open.md) automation, send `ON` or `OFF` as the payload to the topic:
::: code-group
``` [Topic]
doorman-s3/switch/rto__entrance_door/telegram
```
``` [Payload]
ON
```
:::

## Special Topics
Certain special topics allow for advanced telegrams.

### Send a Telegram (Hexadecimal)
Here's an example of how to send a hexadecimal telegram (uint32) to the bus:
::: code-group
``` [Topic]
doorman-s3/send_raw_telegram
```
```json [Payload]
{
    "telegram": 0x1C30BA80
}
```
```json [Advanced Payload]
{
    "telegram": 0x00000680,
    "is_long": true
}
```
:::

### Send a Telegram (Telegram Builder)
Here's an example of how to use the telegram builder to send a telegram to the bus:
::: code-group
``` [Topic]
doorman-s3/send_telegram
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
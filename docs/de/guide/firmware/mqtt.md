## MQTT

Bei Verwendung der MQTT-Firmware werden verschiedene Topics an deinen Broker gesendet. So funktioniert die Struktur der Topics und Steuerung.

### Topic Struktur
Jede Entität veröffentlicht ihren Status auf einem Topic im folgenden Format:
```
<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/state
```

Du kannst bestimmte Entitäten steuern, indem du einen Befehl an ein Topic mit folgendem Format sendest:
::: code-group
``` [Topic]
<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/command 
```
``` [Payload]
ON oder OFF oder was sonst unterstützt wird
```
:::

### Beispiel
Um die [Ring-To-Open](../automation/ring-to-open.md) Automatisierung zu aktivieren oder zu deaktivieren, sende ON oder OFF als Payload an dieses Topic:
::: code-group
``` [Topic]
doorman-s3/switch/ring_to_open/command
```
``` [Payload]
ON
```
:::

### Spezielle Topics
Es gibt spezielle Topics, die erweiterte Befehle ermöglichen.

#### Senden eines Commands (Hexadezimal)
Hier ist ein Beispiel um hexadezimale Commands (uint32) an den Bus zu senden:
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

#### Senden eines Commands (Command Builder)
Hier ist ein Beispiel um Commands via Command Builder an den Bus zu senden:
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
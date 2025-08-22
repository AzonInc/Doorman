# MQTT Integration

Diese Seite bietet alles, was du brauchst – von der Konfiguration bis hin zu praktischen Beispielen mit MQTT-Topics und Payloads.

## Einrichtung
<!--@include: ./instructions/mqtt.md-->

## Topics
Bei Verwendung der MQTT-Firmware werden verschiedene Topics an deinen Broker gesendet. So funktioniert die Struktur der Topics und Steuerung.

### Topic Struktur
Jede Entität veröffentlicht ihren Status auf einem Topic im folgenden Format:
```
<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/state
```

Du kannst bestimmte Entitäten steuern, indem du einen Befehl an ein Topic mit folgendem Format sendest:
::: code-group
``` [Topic]
<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/telegram 
```
``` [Payload]
ON oder OFF oder was sonst unterstützt wird
```
:::

### Beispiel
Um die [Ring-To-Open](../features/ring-to-open.md) Automatisierung zu aktivieren oder zu deaktivieren, sende ON oder OFF als Payload an dieses Topic:
::: code-group
``` [Topic]
doorman-s3/switch/rto__entrance_door/telegram
```
``` [Payload]
ON
```
:::

## Spezielle Topics
Es gibt spezielle Topics, die erweiterte Befehle ermöglichen.

### Senden eines Telegrams (Hexadezimal)
Hier ist ein Beispiel um hexadezimale Telegrams (uint32) an den Bus zu senden:
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

### Senden eines Telegrams (Telegram Builder)
Hier ist ein Beispiel um Telegrams via Telegram Builder an den Bus zu senden:
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
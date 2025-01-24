## MQTT Firmware

Bei Verwendung der MQTT-Firmware werden verschiedene Topics an deinen Broker gesendet. So funktioniert die Struktur der Topics und Steuerung.

### Topic Struktur
Jede Entität veröffentlicht ihren Status auf einem Topic im folgenden Format:
```
<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/state
```

Du kannst bestimmte Entitäten steuern, indem du einen Befehl an ein Topic mit folgendem Format sendest:
```
Topic: <TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/command  
Payload: ON oder OFF oder was immer unterstützt wird
```

### Beispiel
Um die [Ring-To-Open](../automation/ring-to-open.md) Automatisierung zu aktivieren oder zu deaktivieren, sende ON oder OFF als Payload an dieses Topic:
```
Topic: doorman-s3/switch/ring_to_open/command  
Payload: ON
```

### Spezielle Topics
Es gibt spezielle Topics, die erweiterte Befehle ermöglichen.

#### Senden eines hexadezimalen Commands
Um einen hexadezimalen Befehl an den Bus zu senden, verwende das Topic `doorman-s3/send_raw_command` mit einer JSON-Payload. Beispiel:  
```json
{
    "command": 0x1C30BA80,
    "is_long": false
}
```

#### Senden eines Commands mit Command Builder
Um einen Befehl mittels Command Builder an den Bus zu senden, verwende das Topic `doorman-s3/send_command` mit einer JSON-Payload. Beispiel:  
```json
{
    "type": "open_door",
    "address": 0,
    "payload": 0,
    "serial_number": 123456
}
```
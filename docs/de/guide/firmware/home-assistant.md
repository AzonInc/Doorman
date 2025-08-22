# Home Assistant Integration

Entdecke praktische Beispiele zur Integration und Automatisierung von Doorman mit Home Assistant, von einfachen Setups bis hin zu fortgeschrittenen Konfigurationen.

## Zu Home Assistant hinzufügen
<!--@include: ./instructions/home-assistant.md-->

## Bus Telegrams senden
Mit Home Assistant kannst du Aktionen nutzen, um Telegrams über den Bus zu senden.
Benutze entweder `telegram` für hexadezimale Befehle oder `type`, `address`, `payload` und `serial_number` um Befehle über den Telegram Builder zu senden.

::: tip
Denk an das führende `0x` beim Senden eines Befehls mit der `telegram` Eigenschaft. Wenn du es weglässt, musst du den HEX-Befehl zuerst in eine Dezimalzahl umwandeln.
:::

::: code-group
```yaml [Telegram Builder]
service: esphome.doorman_s3_send_tc_telegram
data:
  type: open_door
  address: 0
  payload: 0
  serial_number: 0
```
```yaml [HEX Telegrams]
service: esphome.doorman_s3_send_tc_telegram_raw
data:
  telegram: 0x3a001100
```
:::

## Auf ESPHome-Ereignisse hören
Der Doorman sendet `esphome.doorman`-Ereignisse an Home Assistant, jedes Mal wenn ein Befehl empfangen wird.

Jedes Ereignis sieht so aus:
```yaml
event_type: esphome.doorman
data:
  device_id: 373c62d6788cf81d322763235513310e
  telegram: "00001100"
  type: "open_door"
  address: "0"
  payload: "0"
  serial_number: "0"
origin: LOCAL
time_fired: "2024-08-12T12:34:13.718317+00:00"
context:
  id: 01J5399Y2PP2XS2VRYKBT3H3AV
  parent_id: null
  user_id: null
```

Beispiel für eine Automation:
::: code-group
```yaml [Telegram Builder]
alias: Bei Doorman TC Türöffnungsbefehl auslösen
description: ""
trigger:
  - platform: event
    event_type: esphome.doorman
    event_data:
      type: "open_door"
condition: []
action: []
mode: single
```
```yaml [HEX Telegrams]
alias: Bei Doorman TC Türöffnungsbefehl auslösen
description: ""
trigger:
  - platform: event
    event_type: esphome.doorman
    event_data:
      telegram: "00001100"
condition: []
action: []
mode: single
```
:::

## Blueprint Sammlung

Dies ist eine Sammlung von Community-beigesteuerten Blueprints.  
Erweitere die Sammlung gern mit deinen eigenen Automationen.

### Bestätigbare Tür-Öffnen-Benachrichtigung

Mit diesem Blueprint kannst du eine bestätigbare Benachrichtigung an ein Gerät mit der Home Assistant-App senden, wann immer jemand an der Tür klingelt.

Die Standard-Bestätigungsaktion löst einen Befehl zum Öffnen der Eingangstür aus, aber du kannst sie nach Belieben anpassen.

[![Öffne deine Home Assistant-Instanz und zeige den Blueprint-Import-Dialog mit einem spezifischen, vorausgefüllten Blueprint an.](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2Fazoninc%2Fdoorman%2Fblob%2Fdev%2Fblueprints%2Fconfirmable_open_door_notification.yaml)

### Tür bei Klingelmuster öffnen

Mit diesem Blueprint kannst du die Tür automatisch öffnen, wenn jemand die Klingel in einem bestimmten Muster betätigt.  

Standardmäßig wird die Eingangstür geöffnet, aber du kannst die Aktion nach deinen Wünschen anpassen.  

[![Öffne deine Home Assistant-Instanz und zeige den Blueprint-Import-Dialog mit einem spezifischen, vorausgefüllten Blueprint an.](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2Fazoninc%2Fdoorman%2Fblob%2Fdev%2Fblueprints%2Fopen_door_on_pattern.yaml)
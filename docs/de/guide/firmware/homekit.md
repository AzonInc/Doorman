# HomeKit Integration

Diese Seite bietet dir alle Informationen, die du brauchst, um dein Doorman-Gerät mit Apple HomeKit einzurichten und zu steuern.

## Einschränkungen
Die HomeKit-Integration unterstützt derzeit nur die wichtigsten und grundlegendsten Funktionen deines Doormans. Erweiterte Einstellungen und weniger häufig genutzte Funktionen stehen über HomeKit nicht zur Verfügung und müssen weiterhin über Home Assistant oder die Weboberfläche gesteuert werden.

### Unterstützte Entitäten
- **Ereignis:** Entrance Doorbell
- **Ereignis:** Second Entrance Doorbell
- **Ereignis:** Apartment Doorbell
- **Schloss:** Entrance Door
- **Schloss:** Second Entrance Door
- **Schalter:** Ring To Open
- **Licht:** Hallway Light

### Ereignis-Zuordnung
HomeKit unterstützt nur drei Ereignistypen, daher sind wir auf folgende Zuordnungen beschränkt:

- **single** → `SINGLE_PRESS`
- **double** → `SINGLE_PRESS`
- **long** → `LONG_PRESS`

Alle weiteren Ereignisse werden aufgrund dieser Einschränkung ebenfalls auf `LONG_PRESS` abgebildet:

- **triple** → `LONG_PRESS`
- **quadruple** → `LONG_PRESS`

## Accessory-Bundles
Zur Verbesserung der Benutzerfreundlichkeit und zur Optimierung des internen Speicherverbrauchs bieten wir folgende vordefinierte Accessory-Bundles an:

- **Entrance Doorbell** → *Entrance Door*, *Hallway Light*
- **Second Entrance Doorbell** → *Second Entrance Door*, *Hallway Light*

## Koppeln
<!--@include: ./instructions/homekit.md-->

## Kopplung zurücksetzen
Klicke in der Doorman-Weboberfläche im Bereich `HomeKit Configuration` auf den Button `Reset HomeKit Pairing`, um alle aktuell mit deinem Doorman verbundenen Controller zu entkoppeln.
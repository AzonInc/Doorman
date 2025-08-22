# Doorman Hardware-Spezifikationen
Hier findest du ein paar Infos zur Nutzung:

## Stromversorgung
Dank des breiten Spektrums des Spannungswandlers kann der Doorman S3 mit einer Spannung von 5 bis 30V DC betrieben werden.

## Stromverbrauch
Die folgenden Tabellen zeigen den Stromverbrauch verschiedener Firmware-Varianten bei Betrieb mit einer **24 V DC-Spannungsversorgung**.

::: tip HINWEIS
Die integrierte RGB-Status-LED verbraucht zusätzlich etwa **4 mA**, sofern sie aktiv ist. Dieser Wert ist nicht in den unten aufgeführten Angaben enthalten.
:::

### Standard Firmware
| Integration | Stromverbrauch |
| :----- | :-----: |
| Home Assistant | wird noch bestimmt |
| MQTT | wird noch bestimmt |
| HomeKit | wird noch bestimmt |
| Custom | wird noch bestimmt |

### Nuki-Bridge Firmware
| Integration | Stromverbrauch |
| :----- | :-----: |
| Home Assistant | 20-22mA |
| Custom | wird noch bestimmt |

## Gleichrichterbrücke
Die Gleichrichterbrücke sorgt dafür, dass die Polarität der Busleitung passt. Du musst dir darüber keine Gedanken machen.

## Relais
Beim Doorman-S3 wird ein Solid-State-Relais verwendet. Dieses kann Lasten bis zu 40V bei maximal 2,5A schalten.

## Externer Button
An der BTN Schraubklemme hängt ein 10 kΩ Pull-up Widerstand. Du kannst den Button an die Schraubklemmen BTN und G anschließen.
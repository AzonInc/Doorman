---
description: Discover Doorman's GPIO pinout and connections for integrating buttons, relays, and other peripherals.
---

# GPIO Pinbelegung

Der Doorman S3 nutzt bestimmte GPIO-Pins für spezielle Funktionen und bietet zusätzliche GPIO-Pins für deine eigenen Sensoren und Geräte.

## Revision 1.3.x - 1.4.x
| GPIO  | Verwendungszweck |
| :---  | :---  |
| GPIO01 | Rote Status-LED |
| GPIO02 | WS2812B RGB Status-LED |
| GPIO08 | TC Bus TX - Kurzschluss des Busses zu Ground |
| GPIO09 | TC Bus RX - Liest Bus-Daten / ADC-Eingang (mit integriertem Spannungsteiler - 1M+160K) |
| GPIO40 | Freier I/O |
| GPIO41 | 10K Onboard-Widerstand für externen Button |
| GPIO42 | Relais für analogen Türöffner oder Licht |
| GPIO48 | Freier I/O |

## Revision 1.5.x
| GPIO  | Verwendungszweck |
| :---  | :---  |
| GPIO01 | Rote Status-LED |
| GPIO02 | WS2812B RGB Status-LED |
| GPIO08 | TC Bus TX - Kurzschluss des Busses zu Ground |
| GPIO09 | TC Bus RX - Liest Bus-Daten / ADC-Eingang (mit integriertem Spannungsteiler - 1M+160K) |
| GPIO10 | Verbunden mit GPIO09 - Alternativer ADC Eingang |
| GPIO40 | Freier I/O |
| GPIO41 | 10K Onboard-Widerstand für externen Button |
| GPIO42 | Relais für analogen Türöffner oder Licht |
| GPIO48 | Freier I/O |

## Revision 1.6.x
| GPIO  | Verwendungszweck |
| :---  | :---  |
| GPIO01 | Rote Status-LED |
| GPIO02 | WS2812B RGB Status-LED |
| GPIO08 | TC Bus TX - Kurzschluss des Busses zu Ground |
| GPIO09 | TC Bus RX - Liest Bus-Daten / ADC-Eingang (mit integriertem Spannungsteiler - 1M+147K) |
| GPIO10 | Verbunden mit GPIO09 - Alternativer ADC Eingang |
| GPIO40 | Freier I/O |
| GPIO41 | Externer Button |
| GPIO42 | Relais für analogen Türöffner oder Licht |

## Revision 2.0.x
| GPIO | Verwendungszweck |
| :----- | :-----|
| GPIO01 | Rote Status-LED |
| GPIO02 | SK6812B RGB Status-LED |
| GPIO05 | SIEDLE:IN-HOME-BUS TX Carrier |
| GPIO06 | SIEDLE:IN-HOME-BUS TX Data |
| GPIO07 | Freier I/O via Extension Board Verbinder |
| GPIO08 | TC:BUS TX |
| GPIO09 | TC:BUS RX / SIEDLE:IN-HOME-BUS RX Carrier |
| GPIO10 | SIEDLE:IN-HOME-BUS RX Data |
| GPIO12 | Freier I/O via Extension Board Verbinder |
| GPIO13 | I²C CLK via Extension Board Verbinder |
| GPIO14 | I²C DAT via Extension Board Verbinder |
| GPIO15 | Freier I/O via Extension Board Verbinder |
| GPIO16 | Freier I/O via Extension Board Verbinder |
| GPIO17 | Freier I/O via Extension Board Verbinder |
| GPIO18 | Freier I/O via Extension Board Verbinder |
| GPIO40 | Freier I/O via Schrauben Terminal |
| GPIO41 | Externer Button / Freier I/O via Schrauben Terminal |
| GPIO42 | Relais für analogen Türöffner oder Licht |

## Revision 2.1.x
| GPIO | Verwendungszweck |
| :----- | :-----|
| GPIO01 | Rote Status-LED |
| GPIO02 | SK6812B RGB Status-LED |
| GPIO04 | SIEDLE:1+n Wohnungsklingeltaste |
| GPIO05 | SIEDLE:IN-HOME-BUS TX Carrier |
| GPIO06 | SIEDLE:IN-HOME-BUS TX Data |
| GPIO07 | Freier I/O via Extension Board Verbinder |
| GPIO08 | TC:BUS TX |
| GPIO09 | TC:BUS RX / SIEDLE:IN-HOME-BUS RX Carrier / SIEDLE:1+n Rufader |
| GPIO10 | Bus Spannung (Faktor 11) |
| GPIO10 | SIEDLE:IN-HOME-BUS RX Data |
| GPIO12 | Freier I/O via Extension Board Verbinder |
| GPIO13 | I²C CLK via Extension Board Verbinder |
| GPIO14 | I²C DAT via Extension Board Verbinder |
| GPIO15 | Freier I/O via Extension Board Verbinder |
| GPIO16 | Freier I/O via Extension Board Verbinder |
| GPIO17 | Freier I/O via Extension Board Verbinder |
| GPIO18 | Freier I/O via Extension Board Verbinder |
| GPIO40 | Freier I/O via Schrauben Terminal |
| GPIO42 | Relais für analogen Türöffner oder Licht |
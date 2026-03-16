---
description: Discover Doorman's GPIO pinout and connections for integrating buttons, relays, and other peripherals.
---

# GPIO Pinout

The Doorman S3 uses certain GPIO pins for specific functions and provides additional GPIO pins for your own sensors and devices.

## Revision 1.3 - 1.4
| GPIO | Purpose |
| :----- | :-----|
| GPIO01 | Red Status LED |
| GPIO02 | WS2812B RGB Status LED |
| GPIO08 | TC:BUS TX - Shorts the Bus to Ground |
| GPIO09 | TC:BUS RX - Reads Bus Data / ADC input (with onboard Voltage Divider - 1M+160K) |
| GPIO40 | Free I/O |
| GPIO41 | 10K Onboard Resistor for External Button |
| GPIO42 | Relay for Analog Door Opener or Light |
| GPIO48 | Free I/O |

## Revision 1.5
| GPIO | Purpose |
| :----- | :-----|
| GPIO01 | Red Status LED |
| GPIO02 | WS2812B RGB Status LED |
| GPIO08 | TC:BUS TX - Shorts the Bus to Ground |
| GPIO09 | TC:BUS RX - Reads Bus Data / ADC input (with onboard Voltage Divider - 1M+160K) |
| GPIO10 | Connected to GPIO09, alternative ADC input |
| GPIO40 | Free I/O |
| GPIO41 | 10K Onboard Resistor for External Button |
| GPIO42 | Relay for Analog Door Opener or Light |
| GPIO48 | Free I/O |

## Revision 1.6
| GPIO | Purpose |
| :----- | :-----|
| GPIO01 | Red Status LED |
| GPIO02 | WS2812B RGB Status LED |
| GPIO08 | TC:BUS TX - Shorts the Bus to Ground |
| GPIO09 | TC:BUS RX - Reads Bus Data / ADC input (with onboard Voltage Divider - 1M+147K) |
| GPIO10 | Connected to GPIO09, alternative ADC input |
| GPIO40 | Free I/O |
| GPIO41 | External Button |
| GPIO42 | Relay for Analog Door Opener or Light |

## Revision 2.0
| GPIO | Purpose |
| :----- | :-----|
| GPIO01 | Red Status LED |
| GPIO02 | SK6812B RGB Status LED |
| GPIO05 | SIEDLE:IN-HOME-BUS TX Carrier |
| GPIO06 | SIEDLE:IN-HOME-BUS TX Data |
| GPIO07 | Free I/O via Extension Board Connector |
| GPIO08 | TC:BUS TX |
| GPIO09 | TC:BUS RX / SIEDLE:IN-HOME-BUS RX Carrier |
| GPIO10 | SIEDLE:IN-HOME-BUS RX Data |
| GPIO12 | Free I/O via Extension Board Connector |
| GPIO13 | I²C CLK via Extension Board Connector |
| GPIO14 | I²C DAT via Extension Board Connector |
| GPIO15 | Free I/O via Extension Board Connector |
| GPIO16 | Free I/O via Extension Board Connector |
| GPIO17 | Free I/O via Extension Board Connector |
| GPIO18 | Free I/O via Extension Board Connector |
| GPIO40 | Free I/O via Screw Terminal |
| GPIO41 | External Button / Free I/O via Screw Terminal |
| GPIO42 | Relay for Analog Door Opener or Light |
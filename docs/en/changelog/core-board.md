---
description: Explore the complete history of PCB revisions, including new features, improvements, and changes in each update.
---

# Core Board Changelog
This page highlights the new features, improvements, and changes made in each revision.

## 2.2.0 <Badge type="warning" text="Next" />
- Changed R10 to 500mW rated resistor
- Redesigned the `SIEDLE:IN-HOME-BUS` RX circuit
- Merged voltage detection circuit with `SIEDLE:IN-HOME-BUS` RX circuit to save components
- Removed D9

## 2.1.0 <Badge type="danger" text="Fail" />
- Added D9 to protect Q1B
- Added voltage divider circuit as `ET` to screw terminal to detect `SIEDLE:1+N` ETR signals with GPIO4
- Added voltage detection circuit to read the current bus voltage with GPIO10
- Changed R5 to 59K and R12 to 10K to support `SIEDLE:1+N` systems
- Changed R2 to 1K
- Changed R13 to 500mW rated resistor
- Changed R15 to 750mW rated resistor
- Changed Extension Board Connector Pin 11 to VBUS
- Removed GPIO41 from screw terminal

#### Note
D9 causes issues reading bus data. After removing D9 reading works as expected.
The `SIEDLE:IN-HOME-BUS` RX circuit is not stable enough.

## 2.0.0 <Badge type="tip" text="Current" />
- Added `SIEDLE:IN-HOME-BUS` communication circuit
- Added a 2x7-pin extension board connector exposing selected GPIOs, 3.3V, GND and Bus lines
- Redesigned `TC:BUS` communication circuit
- Changed ESP32-S3-WROOM-1-N8R8 to ESP32-S3-MINI-1-N4R2
- Removed footprints for optional audio components

## 1.6.1
- Added some optional component footprints for audio PoC

## 1.6.0
- Improved Schematics
- Changed Voltage Divider to 1M + 147K (allow up to 28V Bus signals)
- Replaced Terminal Pin 5 with 3.3V
- Replaced Terminal Pin 6 with GPIO40
- Replaced Terminal Pin 7 with GPIO41
- Extended ground plane around power supply section
- Moved diode after the resistor of BUSPWR jumper
- Removed BTN resistor from GPIO41 (use internal ESP resistor)

## 1.5.0
- Power supply redesign based on the reference design
- New method to power the Doorman S3 via `TC:BUS` using a jumper cap
- Connected GPIO10 to the BUS line, thus it's not needed to change the firmware to work with the voltage anymore
- Removed copper around the wifi antenna to improve signal strength
- New ESPHome Logo

## 1.4.0
- Added a diode to prevent multiple power sources damage the circuit

## 1.3.0 <Badge type="info" text="Initial release" />
No changelog available

## 1.2.0 <Badge type="info" text="Development Board" />
No changelog available

## 1.1.0 <Badge type="info" text="Development Board" />
No changelog available

## 1.0.0 <Badge type="info" text="Development Board" />
No changelog available
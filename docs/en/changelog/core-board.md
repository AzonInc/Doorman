---
description: Explore the complete history of PCB revisions, including new features, improvements, and changes in each update.
---

# Core Board Changelog
This page highlights the new features, improvements, and changes made in each revision.

## 2.0.0 <Badge type="warning" text="Next" />
- Removed footprints for optional audio components
- Redesigned `TC:BUS` communication circuit
- Added `SIEDLE:IN-HOME-BUS` communication circuit
- Replaced ESP32-S3-WROOM-1-N8R8 with ESP32-S3-MINI-1-N4R2
- Added a 2x7-pin extension board connector exposing selected GPIOs, GND and Bus lines

## 1.6.1
- Added some optional component footprints for audio PoC

## 1.6.0 <Badge type="tip" text="Current" />
- Replaced Terminal Pin 5 with 3.3V
- Replaced Terminal Pin 6 with GPIO40
- Replaced Terminal Pin 7 with GPIO41
- Removed BTN resistor  from GPIO41 (use internal ESP resistor)
- Extended ground plane around power supply section
- Changed Voltage Divider to 1M + 147K (allow up to 28V Bus signals)
- Moved diode after the resistor of BUSPWR jumper
- Improved Schematics

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
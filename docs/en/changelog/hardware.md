---
description: Explore the complete history of Doorman PCB revisions, including new features, improvements, and changes in each update.
---

# PCB Revision History & Changelog
Welcome to the complete history of all PCB updates! This page highlights the new features, improvements, and changes made in each revision.

## Doorman S3
### 2.0.0 <Badge type="warning" text="Next" />
- Remove footprints for optional audio components
- Introduce a new TC:BUS communication circuit
- Replace ESP32-S3-WROOM-1-N8R8 with ESP32-S3-MINI-1-N4R2
- Add an extension board connector exposing selected GPIOs, GND, TC:BUS and power rails

### 1.6.1
- Add some optional component footprints for audio PoC

### 1.6.0 <Badge type="tip" text="Current" />
- Replace Terminal Pin 5 with 3.3V
- Replace Terminal Pin 6 with GPIO40
- Replace Terminal Pin 7 with GPIO41
- Remove BTN resistor  from GPIO41 (use internal ESP resistor)
- Extend ground plane around power supply section
- Change Voltage Divider to 1M + 147K (allow up to 28V Bus signals)
- Move diode after the resistor of BUSPWR jumper
- Improved Schematics

### 1.5.0
- Power supply redesign based on the reference design
- New method to power the Doorman S3 via TC:BUS using a jumper cap
- Connected GPIO10 to the BUS line, thus it's not needed to change the firmware to work with the voltage anymore
- Removed copper around the wifi antenna to improve signal strength
- New ESPHome Logo

### 1.4.0
- Added a diode to prevent multiple power sources damage the circuit

### 1.3.0
Initial release

### 1.2.0
No changelog available / Development Board

### 1.1.0
No changelog available / Development Board

### 1.0.0
No changelog available / Development Board

## Doorman EA (Audio Extension Board)
### 1.0.0 <Badge type="warning" text="Next" />
Initial release
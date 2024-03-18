# Doorman

This repository contains the source files for the Doorman, a device which lets you connect your TCS Intercom to any home automation system.

## Need Support?

If you need help with your Doorman, please join the Doorman [Discord server](https://discord.gg/MMT4bxhQ68) where you can ask for help from the community and myself.

## Repository Structure

At the root of the repository you will find these directories:

- `pcb`: KiCad schematic and PCB design files
- `firmware`: ESPHome configuration files
- `components`: ESPHome external component files

## Features

- ESP32-S3-WROOM-1 N16R2
- USB-C port for easy flashing (and because USB-C is cool 😎)
- Status LED (GPIO 01)
- Configurable WS2812B RGB LED to indicate a specific event (GPIO 02)
- GPIO Pin Connector (GPIO 09 & GPIO 10)
- External Button Pin Connector with pulldown resistor (GPIO 38)
- Open the front Door
- Detect Door Bell

## Compatibility

If your TCS Intercom got a, b and P Screw Terminals its likely compatible.
- **a** is the Bus line
- **b** is GND 
- **P** is the +24V line in 3-wire mode or just the apartment bell button in 2-wire mode

The scope of functions highly depends on the specific hardware and wiring used in the building.
If the power supply in your Building is powerful enough you can power your Doorman with the +24V P line. Otherwise just use a USB-C Cable or use the Screw Terminals to connect any other Power Source (5-24V DC, preferably 1A).

## Credits

Doorman is heavily built on the code and the information of the following projects:

**[TCSIntercomArduino](https://github.com/atc1441/TCSintercomArduino)**
Different Methods to read from and write to the TCS Bus.
Also see the [Reverse Engineering Video](https://www.youtube.com/watch?v=xFLoauqj9yA&t=11s).

**[tcs-monitor](https://github.com/Syralist/tcs-monitor)**
An mqtt monitor for listening to the TCS Bus. Also see this [Blog Post](https://blog.syralist.de/posts/smarthome/klingel/).

**[Doorman](https://github.com/peteh/doorman)**
This Doorman project is based a lot on peteh's one.
The main goal is to provide a proper PCB for his Doorman Project.
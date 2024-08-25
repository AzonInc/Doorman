![PCB front and back](./images/title_dark.png){width=500px}

# What is Doorman?

Doorman allows you to connect your [TCS](https://www.tcsag.de/) or [Koch](https://www.kochag.ch/) intercom to any home automation system. Additionally, some rebranded [Niko](https://www.niko.eu/) and [Scantron](https://scantron.dk/) intercoms are counterparts to the TCS and Koch models.

### What about other intercoms?
Doorman isn’t limited to TCS or Koch intercoms.\
You can use it to simulate a button press to trigger a door opener or light with the integrated solid-state relay (up to 40V, max 2.5A).

If your intercom operates on a 2-wire bus within the 14-24V DC range, it might be possible to implement other protocols as well.

You can also [monitor the voltage level](firmware/stock-firmware#advanced-examples) on older intercoms (14-24V DC) using the internal ADC and trigger specific actions based on the readings.

<div class="tip custom-block" style="padding-top: 8px">

Just received your Doorman? Jump to the [Quickstart](getting-started) guide.

</div>

## Features

- Powered by ESP32-S3
- USB-C port for easy flashing (and because USB-C is cool 😎)
- Compact design that fits almost anywhere
- Configurable WS2812B RGB LED to indicate specific events
- Two additional GPIOs for easy expansion with your own sensors and devices (e.g., I²C)
- Solid-state relay for switching up to 40V (max. 2.5A), ideal for older intercom systems
- External Button (G + BTN)
- Bridge rectifier for bus-wire input to correct polarity
- TCS bus communication for functions like opening the front door and detecting the doorbell
- Can be used as a Nuki Bridge with the [Nuki Bridge Firmware](firmware/nuki-bridge-firmware). Note that this firmware is incompatible with the Nuki app, but it works with the Home Assistant app.

## Interested in Buying One?

Feel free to contact me on [Discord](https://discord.gg/t2d34dvmBf) or via [E-Mail](mailto:flo@azon.ai?subject=Doorman).

I offer fully assembled and tested Doorman-S3 units with the [Stock Firmware](firmware/nuki-bridge-firmware) pre-installed, ready for direct integration and adoption into Home Assistant.
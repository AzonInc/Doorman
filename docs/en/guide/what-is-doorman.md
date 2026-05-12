---
description: Doorman is an open source intercom gateway that lets you connect TCS or Koch intercoms (and some rebranded Niko, Scantron, and older Jung TKM models) to any home automation system.
---

# What is Doorman?

Doorman allows you to connect your [TCS TC:BUS](https://www.tcsag.de/), [Koch TC:BUS](https://www.kochag.ch/) or [SIEDLE IN-HOME-BUS](https://www.siedle.de/) intercom to any home automation system. Additionally, some rebranded [Niko](https://www.niko.eu/), [Scantron](https://scantron.dk/) and older [Jung](https://www.jung-group.com/) TKM intercoms are counterparts to the TCS and Koch models.

<div style="display: flex; flex-direction: row; flex-wrap: wrap; justify-content: space-between; gap: 20px;align-items: center;" markdown>
   <div style="min-width:260px;flex: 1 1 260px;" markdown>

### What about other intercoms?
    
Doorman isn't limited to TC:BUS and IN-HOME-BUS intercoms.

You can use it to simulate a button press to trigger a door opener or light with the integrated solid-state relay (up to 40V, max 2.5A).

If your intercom operates on a 2-wire bus within the 14-30V DC range, it might be possible to implement other protocols as well.
You can also monitor the voltage level on older intercoms (14-30V DC) using the internal ADC and trigger specific actions based on the readings.

   </div>
   <div style="min-width: 185px;max-width:200px;flex: 1 1 185px;" markdown>
        <img style="" src="/enclosure-base-board.png">
   </div>
</div>

<div class="tip custom-block" style="padding-top: 8px">

Just received your Doorman? Jump to the [Quickstart](getting-started) guide.

</div>

## Features

- Powered by ESP32-S3
- USB-C port for easy flashing (and because USB-C is cool 😎)
- Compact design that fits almost anywhere
- WS2812B/SK6812B RGB LED to indicate specific events
- Two additional GPIOs for easy expansion with your own sensors and devices (e.g., I²C, external button)
- Solid-state relay for switching up to 40V (max. 2.5A), ideal for older intercom systems
- Bridge rectifier for bus-wire input to correct polarity
- TC:BUS communication for functions like opening the front door and detecting the doorbell
- SIEDLE:IN-HOME-BUS communication for functions like opening the front door and detecting the doorbell
- Can be used as a Nuki Bridge with the [Nuki Bridge Addon](firmware/nuki-bridge).

## Interested in a Doorman?

If you'd like a fully assembled Doorman S3 unit, you can use the button below to submit a non-binding inquiry.

If you have any questions or prefer direct contact, you can also reach out on [Discord](https://discord.gg/t2d34dvmBf) or via [email](mailto:flo@azon.ai?subject=Doorman).

<div class="custom-layout">
    <a class="btn" href="../order">Send Inquiry</a>
</div>
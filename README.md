<h1 align="center">
    <br>
    Doorman S3
    <br>
    <br>
    <img src=".github/images/title_dark.png" alt="Doorman S3" height="275">
    <br>
</h1>

This repository contains the source files for the Doorman S3 Intercom Gateway, a device which lets you connect your [TCS](https://www.tcsag.de/) or [Koch](https://www.kochag.ch/) intercom to any home automation system.\
There are also some rebranded [Niko](https://www.niko.eu/) and [Scantron](https://scantron.dk/) intercoms which are counterparts of the TCS and Koch ones.

However it's not limited to these intercoms only.
With the integrated relay you can easily simulate a button press to trigger the door opener or the light (up to 40V, max 2.5A).

If your intercom is not a TCS or Koch one but operates on a 2-wire bus within the 14-24V range it might be possible to implement other protocols as well.

You could also monitor the voltage level on older intercoms (14-24V) by replacing the tcs_intercom component with a template Binary Sensor combined with ADC and trigger specific actions based on it.

Thanks a lot to [PCBWay](https://pcbway.com) for sponsoring this Project.
Scroll down to "Manufacturing" to read more.

## 🚀 Getting started

Make sure to check out the [Doorman Docs](https://doorman.azon.ai/).\
You will find everything you need. If you don't I'm happy to answer your questions on [Discord](https://discord.gg/t2d34dvmBf) or [GitHub Issues](https://github.com/AzonInc/Doorman/issues).

## 📦 Interested in buying one?

Feel free to contact me on [Discord](https://discord.com/users/275370456163287040) or by [E-Mail](mailto:flo@azon.ai?subject=Doorman).

I offer you a fully assembled and tested Doorman-S3 which comes with the Stock Firmware pre-installed for direct integration and adoption into Home Assistant.

## 🤖 Features

- Powered by ESP32-S3
- USB-C port for easy flashing (and because USB-C is cool 😎)
- It's super small (37mm x 37mm) and fits almost everywhere
- Configurable WS2812B RGB LED to indicate specific events
- 2 additional GPIO via Screw Terminal for easy expansion with your own sensors and devices
- 1 Relay via Screw Terminal to switch up to 40V (max. 2.5A) for oldschool intercom systems
- 1 External Button via Screw Terminal (G + BTN)
- Bridge Rectifier for the Bus-wire input to correct polarity
- TCS Bus Communication (e.g. Open the front door, Detect Doorbell)
- You can optionally use your Doorman as a Nuki Bridge with the [Nuki Bridge Firmware](https://doorman.azon.ai/firmware/nuki-bridge-firmware). It is incompatible with the Nuki App, Bluetooth LE only.

## ✔️ Compatibility

If your TCS or Koch Intercom got a, b and P labeled Screw Terminals and is operating on 24V DC it's likely compatible.\
Make sure to check out the [Compatibility Docs](https://doorman.azon.ai/guide/hardware-compatibility).

## 🚩 Repository Structure

At the root of the repository you will find these directories:

- `pcb`: KiCad schematic and PCB design files
- `firmware`: ESPHome configuration files
- `enclosure`: Enclosure STL Files

## 🛠️ Manufacturing

<img src=".github/images/pcbway_delivery.png" alt="PCBWay Delivery" height="275">

I didnt know which PCB manufacturer is good but fortunately PCBWay reached out to me and offered to sponsor PCB fabrication.\
Well... I'm more than satisified with the quality, especially the nice colors. Everything looks clean and part sourcing was an ease. They also sent me two more unpopulated PCBs extra.
I soldered the ESP Modules using a heating plate myself because I still had ESP32 Modules at home.

It was really easy to get in touch with them and whenever there rised a question they didn't hesitate to ask.
A special thanks goes to Liam and Lynne for supporting me throughout the entire process. Even tho I made a lot of changes they were always very patient with me. It was a pleasure working with you guys.

If you need a good quality one stop manufacturer I can definitely recommend [PCBWay](https://pcbway.com) :)

You can find all the neccessary files [here](https://github.com/AzonInc/doorman/tree/master/pcb).

## 🙌 Contributing
If you would like to contribute, please feel free to open a Pull Request.

## 📜 Credits

Doorman is heavily built on the code and the information of the following projects and people.\
This project would not have been possible without all of them. ❤️

**[TCSIntercomArduino](https://github.com/atc1441/TCSintercomArduino)**\
Different Methods to read from and write to the TCS Bus.\
Feel free to watch the [Reverse Engineering Video](https://www.youtube.com/watch?v=xFLoauqj9yA) if you're interested.

**[tcs-monitor](https://github.com/Syralist/tcs-monitor)**\
An mqtt monitor for listening to the TCS Bus.\
You can find more information in this [Blog Post](https://blog.syralist.de/posts/smarthome/klingel/).

**[Doorman](https://github.com/peteh/doorman)**\
A lot of this Doorman project is based on peteh's one.\
The main goal is to provide a proper PCB for his Doorman Project.

**[ESPHome_nuki_lock](https://github.com/uriyacovy/ESPHome_nuki_lock)**\
This module builds an ESPHome lock platform for Nuki Smartlock (nuki_lock).

**[AStrehlau](https://github.com/AStrehlau)**\
Thanks a lot for your time, patience, knowledge and every single valuable advice that made this Project better, smaller and more affordable.
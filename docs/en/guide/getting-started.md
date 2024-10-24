# Getting Started

Is this your first time here? Don't worry, we've got you covered!
Below, you'll find a detailed guide for setting up your Doorman for the first time.

Please note that these instructions are based on the pre-flashed Doorman PCB that I sell. If you've had a PCB manufactured yourself, make sure to flash the firmware first; otherwise, you might be waiting a long time for anything described below to happen. 😄

**Thank you so much for using Doorman! ❤️**

## Wiring
First, open your intercom enclosure. On most models, you will find a screw terminal labeled with `a`, `b`, `E`, and `P`.

Connect the `b` line (Ground) to one of the TCS:BUS terminals on your Doorman, and connect the `a` line (24V Bus) to the other TCS:BUS terminal on your Doorman.

### Power supply options:
::: details 3-Wire Mode via intercom <Badge type="tip" text="Recommended" />
Connect the `P` line (+24V) to the `P` terminal on your Doorman.

> [!WARNING]
> The intercom power supply must provide at least 60mA.

Example:
![3-wire](./images/3wire.png){width=300px}
:::

::: details 2-Wire Mode via external wired Power Supply
Connect the external power supply to your Doorman using the `P` (+5V to +30V) and `G` (Ground) screw terminals.

Example:
![2-wire external via screw terminal](./images/2wire_power_screwterminal.png){width=300px}
:::

::: details 2-Wire Mode via external USB-C Power Supply
Connect the external power supply via the USB-C port.

Example:
![2-wire external via usb](./images/2wire_power_usb_c.png){width=300px}
:::

::: details 2-Wire Mode via intercom <Badge type="danger" text="Impossible" />
> [!DANGER] Unfortunately this scenario is not possible!
> Using the `a`-bus line as a power source results in a loud beeping noise. This issue is likely due to the high-frequency switching power supply and may be addressed in future hardware revisions.

Example:
![2-wire external via usb](./images/2wire_power_a_terminal.png){width=300px}
:::


## Setup

### Step 1: Connect to Wi-Fi
When you power on your Doorman-S3 for the first time, it will blink rapidly (orange) and create a new Access Point named `Doorman-S3 Setup`. You can set up your WiFi credentials using the Access Point, Improv Serial, or Improv Bluetooth.

The Access Point password is `open-sesame`.

Once connected to the Access Point, the web interface should open automatically (see also login to network notifications).\
If it doesn't, you can manually navigate to http://192.168.4.1/ in your browser.

::: tip
You can take advantage of automatic setup in Home Assistant if your Home Assistant has Bluetooth enabled.

Additionally, with mDNS support, Home Assistant will automatically discover your Doorman once it's connected to WiFi.
:::
![Home Assistant Discovery](./images/discovery.png)

### Step 2: Connect to Home Assistant
After connecting Doorman to your network, it will blink slowly (blue) and should be automatically discovered by Home Assistant. Simply click on `Configure` to add the newly discovered ESPHome node.

### Step 3: Bus Commands
::: tip
After connecting your Doorman to Home Assistant, the `Interactive Setup` Process will automatically begin as described below.

There's no need to start it manually; it will initiate automatically on every restart, provided the process hasn't been completed or canceled.
:::

#### Interactive Setup <Badge type="warning" text="NEW" />
To simplify the configuration of the key commands, you can use the Interactive Setup Process.

To get started, either access the internal web server of your Doorman or visit the [ESPHome Integration page](https://my.home-assistant.io/redirect/integration/?domain=esphome) and select the newly listed Doorman S3 device entry.

In the `Configuration` section, you will find the `Interactive Setup: Start` button. Click this button to initiate the setup process.

Once started, the `Interactive Setup: Status` Text Sensor will guide you through the required steps (e.g., press button X, wait, or pick up the phone).\
During the setup, the RGB Status LED will pulse green-turquoise while waiting for you to complete each task and will remain solid green-turquoise for 3 seconds after saving the command.

After the setup is complete, the process will automatically end and display the corresponding status.

#### Manual: No pain, no gain!
You will also find the good old `Last Bus Command` Text Sensor that tracks the most recent bus command in hexadecimal format.
Additionally, each received command is logged in the ESPHome Console (at the Debug log level) and published as a Home Assistant event.

To capture the codes, press the buttons on your intercom phone and then copy the codes into the corresponding configuration text inputs.

## ESPHome adoption

If you want to customize your Doorman firmware, you can add the Doorman device to your [ESPHome Dashboard](https://my.home-assistant.io/redirect/supervisor_ingress/?addon=5c53de3b_esphome) and flash your customized [Stock](firmware/stock-firmware.md) or [Nuki Bridge](firmware/nuki-bridge-firmware.md) firmware.

After adding the device, your configuration will look like this:
::: details Example
> [!NOTE]
> You can add new components and modify existing ones here. For more information on the possibilities, check out the [Examples](firmware/stock-firmware#examples) section.
>
> If you need to deeply customize the firmware, you'll need to use the `base.yaml` file from the repository.

```yaml
substitutions:
  name: doorman-s3
  friendly_name: Doorman S3

packages:
  AzonInc.Doorman: github://AzonInc/doorman/firmware/doorman-stock.yaml@master

esphome:
  name: ${name}
  name_add_mac_suffix: false
  friendly_name: ${friendly_name}

api:
  encryption:
    key: ...

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
```
:::
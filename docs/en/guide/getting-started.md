# Getting Started

First time here? Don't worry — we've got you covered! 😊

Below is a step-by-step guide to help you set up your Doorman for the very first time. Just follow along — everything is laid out in the order you need to do it.

Thank you so much for choosing Doorman and supporting this project! ❤️

## Disclaimer
By using or installing Doorman, you accept full responsibility for any modifications made to your intercom system.
System behavior may vary depending on your specific installation and environment.

If you are uncertain about any part of the setup or wiring, please consult a qualified electrician. Improper installation may result in **device malfunction or damage to your intercom system**.

:::tip NOTE
This is **not an official or certified product**. It is the result of a reverse engineering effort and is provided **as-is**, with **no guarantee of compatibility, safety, or functionality**.
:::

## Flashing the Firmware
If you bought a **pre-flashed Doorman PCB**, you're good to go — skip this step! 🎉

But if you **built your own board** or need to **update the firmware**, now's the time to flash it.
Otherwise, you'll be staring at a very quiet device while wondering why nothing works. 😄

<div class="custom-layout"><a class="btn" target="_blank" href="./firmware/installation">Install or Update Firmware</a></div>

## Connect the Wires
Start by opening your indoor station's enclosure. On most models, you'll find screw terminals labeled `a`, `b`, `E`, and `P`.

The Doorman connects **in parallel** to the bus — just like any other device.

Before proceeding, review the power supply options below and choose the one that best suits your setup.

::: danger CAUTION
**NEVER connect an external power source to the TC:BUS terminals!**

Doing so can cause **serious damage**.  
Always double-check all connections carefully before plugging anything in.
:::

### Power supply options:
::: details 3-Wire Mode via indoor station <Badge type="tip" text="Recommended" />
> [!DANGER] IMPORTANT
> **Please remove the jumper cap on `BUS PWR` before you proceed!**
> 
> The intercom’s power supply must provide **at least 60 mA**, which is usually the case.  
> However, since the available power is limited, **connecting additional devices like the Doorman-S3 directly to the bus may cause instability** in the system.

Connect the `b` line (Ground) and the `a` line (24V Bus) to the TC:BUS terminals on your Doorman—one line per terminal.

Then connect the `P` line (+24V) to the `P` terminal on your Doorman.

Example:
![3-wire](./images/3wire.png){width=300px}
:::

::: details 2-Wire Mode via indoor station <Badge type="danger" text="Hardware revision 1.5 and later" />
> [!INFO] INFO
> Using the `a`-bus line as a power source on revisions older than `1.5` results in a loud beeping noise. This issue is likely due to the high-frequency switching power supply.
>
> Starting with revision `1.5` this method will only produce a subtle, yet noticeable hissing sound on the speaker.

> [!DANGER] IMPORTANT
> The intercom’s power supply must provide **at least 60 mA**, which is usually the case.  
> However, since the available power is limited, **connecting additional devices like the Doorman-S3 directly to the bus may cause instability** in the system.

Connect the `b` line (Ground) and the `a` line (24V Bus) to the TC:BUS terminals on your Doorman—one line per terminal.

Once connected, place a jumper cap on `BUS PWR` to enable power from the bus.

Example:
![2-wire via bus pwr jumper](./images/2wire_intercom.png){width=300px}
:::

::: details 2-Wire Mode via external wired Power Supply
> [!DANGER] IMPORTANT
> Please remove the jumper cap on `BUS PWR` before you proceed!

Connect the `b` line (Ground) and the `a` line (24V Bus) to the TC:BUS terminals on your Doorman—one line per terminal.

Then connect the external power supply to your Doorman using the `P` (+5V to +30V) and `G` (Ground) screw terminals.

Example:
![2-wire external via screw terminal](./images/2wire_power_screwterminal.png){width=300px}
:::

::: details 2-Wire Mode via external USB-C Power Supply
> [!DANGER] IMPORTANT
> Please remove the jumper cap on `BUS PWR` before you proceed!

Connect the `b` line (Ground) and the `a` line (24V Bus) to the TC:BUS terminals on your Doorman—one line per terminal.

Then, power your Doorman using the USB-C port.

Example:
![2-wire external via usb](./images/2wire_power_usb_c.png){width=300px}
:::


## Connect to Wi-Fi
When you power on your **Doorman-S3** for the first time, it will start **blinking rapidly in orange** — that means it's ready for Wi-Fi setup!

You can connect it to Wi-Fi using one of three methods:  
**Access Point**, **Improv Serial (USB)**, or **Improv Bluetooth**.

### Access Point

<div style="display: flex; flex-direction: row; flex-wrap: wrap; justify-content: space-between; gap: 20px;align-items: center;" markdown>
   <div style="min-width:260px;flex: 1 1 260px;" markdown>

   1. **Connect to the Wi-Fi network** named `Doorman-S3 Setup`.
      Use the password: `Op3n-Sesame!`

   2. Once connected, the **setup page should open automatically**.
      ::: tip
      You might also see a pop-up or notification asking you to sign in to the network.
      :::

      If nothing happens, just open your browser and go to **http://192.168.4.1** or click the button below:

      <div class="custom-layout">
         <a class="btn" target="_blank" href="http://192.168.4.1">Connect Doorman to Wi-Fi</a>
      </div>

   3. Enter the **Wi-Fi credentials** for the network you want your Doorman to join.

   4. Once you see **“Wi-Fi connected successfully”**, you can close the window.

   5. Your **Doorman** is now connected and ready to be added to **Home Assistant** using the **ESPHome integration**.

   </div>
   <div style="min-width: 185px;max-width:200px;flex: 1 1 185px;" markdown>
      <img style="" src="./images/ap.png">
   </div>
</div>

### Improv Bluetooth <Badge type="tip" text="Home Assistant" />
<div style="display: flex; flex-direction: row; flex-wrap: wrap; justify-content: space-between; gap: 20px;align-items: center;" markdown>
   <div style="min-width:250px;flex: 1 1 250px;" markdown>

   1. Make sure your **Home Assistant** is updated to the latest version — this ensures full compatibility with your **Doorman** device.

   2. Open the **Home Assistant** app on your mobile device.

   3. Go to **Settings → Devices & Services**.

   4. Look for a new device named **`doorman-s3` (Improv via BLE)** and tap **Add**.

   5. Enter the **Wi-Fi credentials** for the network you want the device to connect to.

   6. When you see **“Wi-Fi connected successfully”**, you're all set — you can close the prompt.

   7. Your **Doorman** is now ready to be added to Home Assistant using the **ESPHome integration**.

   </div>
   <div style="min-width: 185px;max-width:200px;flex: 1 1 185px;" markdown>
      <img style="" src="./images/discovery_ble.png">
   </div>
</div>

### Improv Bluetooth
You can also connect your Doorman to Wi-Fi using Bluetooth. This is a quick and easy option — especially if you're using a phone or laptop with Bluetooth support.

Make sure your Doorman is powered on and nearby, then click the button below to start:

<improv-wifi-launch-button>
   <button slot="activate">
         <div class="custom-layout">
            <a class="btn">Connect Doorman to Wi-Fi</a>
         </div>
   </button>
   <div slot="unsupported">
         <div class="danger custom-block">
            <p class="custom-block-title">OH SNAP!</p>
            <p>Your browser does not support provisioning :(</p>
         </div>
   </div>
</improv-wifi-launch-button>

### Improv Serial <Badge type="tip" text="USB Connection" />
If your device isn't showing up automatically, or you're setting it up for the first time, this method is a great alternative.

Just connect your Doorman to this computer using a USB cable, then click the button below to connect it to Wi-Fi:

<improv-wifi-serial-launch-button>
   <button slot="activate">
         <div class="custom-layout">
            <a class="btn">Connect Doorman to Wi-Fi</a>
         </div>
   </button>
   <div slot="unsupported">
         <div class="danger custom-block">
            <p class="custom-block-title">OH SNAP!</p>
            <p>Your browser does not support provisioning :(</p>
         </div>
   </div>
</improv-wifi-serial-launch-button>


## Add to Home Assistant <Badge type="tip" text="Home Assistant integration" />

:::warning BEFORE YOU PROCEED
This is the **default** for all **pre-flashed Doorman** units.  
You can skip this step if you haven't flashed the `Home Assistant` Smart Home integration.
:::

<!--@include: ./firmware/instructions/home-assistant.md-->

## Setup MQTT Broker <Badge type="tip" text="MQTT integration" />

:::warning BEFORE YOU PROCEED
You can skip this step if you haven't flashed the `MQTT` Smart Home integration.
:::

<!--@include: ./firmware/instructions/mqtt.md-->

## HomeKit Pairing <Badge type="tip" text="HomeKit integration" />

:::warning BEFORE YOU PROCEED
You can skip this step if you haven't flashed the `HomeKit` Smart Home integration.
:::

<!--@include: ./firmware/instructions/homekit.md-->

## Interactive Setup
::: tip
When you first connect your Doorman to Home Assistant, it will be in `Setup Mode` for interactive setup already.

You don't need to manually activate this mode; it will start automatically at each reboot as long as the setup process has not been completed or canceled.
:::

### 1. Access the Settings
Open the settings either through your Doorman's Webinterface or visit the [ESPHome Integration page](https://my.home-assistant.io/redirect/integration/?domain=esphome) and select the Doorman S3 device.

### 2. Activate Setup Mode
Go to the `Configuration` section and enable `Setup Mode` to begin the interactive setup.  
Once the setup process begins, the RGB status LED will pulse green-turquoise.

::: warning BEFORE YOU PROCEED
The indoor station **must be connected**, to complete the setup process.
:::

### 3. Wait until you ring the doorbell
When you press the doorbell button at your apartment or entrance, the system will save your indoor station's serial number and attempt to detect the model.

:::info
If you have multiple door stations, the firmware will try to detect additional stations automatically.

To enable detection of the **second doorbell** and allow unlocking of the **second door**, you need to **press the second doorbell** or **manually press the unlock button** on the second door **at least once** — but only **after setup is complete**.

🕒 Wait about **one minute after setup finishes** before doing this, so the system can correctly store the second door's address.
:::

Once the model is successfully detected, the system will read the memory of your indoor station.
**Note:** This entire process can take **up to 30 seconds**.

If the model detection is successful or if it times out, the setup will be considered finished.

### 4. Setup complete
The LED will remain green-turquoise for 3 seconds, then turn off, and the setup mode will be turned off. The setup is complete.

## Next Steps
You're probably looking to configure your Doorman. To get started, open the Doorman web interface or the Home Assistant [device dashboard](https://my.home-assistant.io/redirect/integration/?domain=esphome).

If you're unsure about what each entity does, what it controls, or how to configure it, check out the [Entity Reference](../reference/entities) for a detailed guide on all available entities, their functions, and setup options.

## Still having issues?
If you're having trouble setting up your Doorman in Home Assistant, we're here to help.  
Join us on [Discord](https://discord.gg/t2d34dvmBf) to get assistance and discuss with other users.
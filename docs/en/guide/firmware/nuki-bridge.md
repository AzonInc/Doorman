---
description: Documentation for the Nuki Bridge Addon, including setup and usage to integrate your Nuki system with the intercom gateway.
---

# Nuki Bridge Addon

This add-on integrates the [Nuki Bridge](https://github.com/uriyacovy/ESPHome_nuki_lock) component via Bluetooth LE (BLE). Since it uses ESPHome's own Bluetooth stack, it can be combined with **Improv over BLE**. It's not compatible with the MQTT or HomeKit integrations, or with the Audio Extension Board, as there is not enough flash for both.

::: tip NOTE
While the official Nuki app allows you to control the smart lock, it does not support the emulated Nuki Bridge provided by this add-on. To control your lock remotely through this bridge, you'll need to use the Home Assistant app instead.
:::

## Pair your Smart Lock
::: warning
If your Smart Lock is already paired, this process will unpair it!
:::

Depending on the PCB revision, either press the `FLASH`- or `PRG`-button on the Doorman S3 PCB for 5 seconds until the status LED starts flashing purple, or enable the Nuki Bridge Pairing Mode switch in Home Assistant / ESPHome Webinterface. Then, press the button on your Smart Lock for 5 seconds until the light ring turns on.

After a successful pairing, the status LED will remain steadily on for 3 seconds.

## Unpair your Smart Lock
You can unpair your smart lock either by using the `Unpair Device` button in the Home Assistant or Doorman web interface, or by pressing the physical `FLASH` or `PRG` button on the Doorman S3 PCB.

::: info Physical Button
If your Smart Lock is already paired with Doorman, press the `FLASH`- or `PRG`-button on the Doorman S3 PCB for 5 seconds until the RGB status LED starts flashing purple. Your Smart Lock will then be unpaired. Note that the pairing mode will time out after 30 seconds.
:::
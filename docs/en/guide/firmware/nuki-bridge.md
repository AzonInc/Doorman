# Nuki Bridge Addon

This add-on integrates the [Nuki Bridge](https://github.com/uriyacovy/ESPHome_nuki_lock) component via Bluetooth LE (BLE). **Using Improv over BLE at the same time is not supported**, as the Nuki Bridge occupies the Bluetooth stack. It’s also not compatible with the MQTT or HomeKit integrations.

::: tip NOTE
While the official Nuki app allows you to control the smart lock, it does not support the emulated Nuki Bridge provided by this add-on. To control your lock remotely through this bridge, you'll need to use the Home Assistant app instead.
:::

## Pair your Nuki Smart Lock
::: warning
If your Smart Lock is already paired, this process will unpair it!
:::

Depending on the PCB revision, either press the `FLASH`- or `PRG`-button on the Doorman-S3 PCB for 5 seconds until the status LED starts flashing purple, or enable the Nuki Pairing Mode switch in Home Assistant / ESPHome Webinterface. Then, press the button on your Smart Lock for 5 seconds until the light ring turns on.

After a successful pairing, the status LED will remain steadily on for 3 seconds.

## Unpair your Nuki Smart Lock
You can unpair your smart lock either by using the `Nuki Unpair Device` button in the Home Assistant or Doorman web interface, or by pressing the physical `FLASH` or `PRG` button on the Doorman-S3 PCB.

::: info Physical Button
If your Smart Lock is already paired with Doorman, press the `FLASH`- or `PRG`-button on the Doorman-S3 PCB for 5 seconds until the RGB status LED starts flashing purple. Your Smart Lock will then be unpaired. Note that the pairing mode will time out after 30 seconds.
:::
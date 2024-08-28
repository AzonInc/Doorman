# Support

If you're experiencing issues with your Doorman, help is available through several channels:

- The below troubleshooting guide covers many common issues
- The [Discord server](https://discord.gg/t2d34dvmBf) is a great place to get assistance from me and the community
- The [GitHub issues](https://github.com/AzonInc/Doorman/issues) may contain solutions to problems you're encountering

Please note that support is provided on a best-effort basis. While I will assist as much as possible, please understand that I may not be able to help immediately or with every situation, as this is not my full-time job. Thank you for your understanding.

## Common Issues

::: details Web Serial (Flashing via ESP Web Tools) Connect-Disconnect loop
If you've received a Doorman-S3 PCB that hasn't been pre-flashed with the [Stock Firmware](../firmware/stock-firmware.md), the ESP will not have any firmware installed. As a result, it will enter a continuous restart loop by default, making it impossible to flash using ESP Web Tools or any other tool.

#### Solution
To resolve this, hold down the button labeled `FLASH` while connecting the USB cable to your computer. This will put the ESP into bootloader mode, preventing it from constantly restarting.
:::

::: details A loud beeping noise occurs on the phone
Please take a look [here](../hardware-compatibility#known-issues).
:::

::: details Home Assistant can't connect / BAD_INDICATOR errno=11
If Home Assistant can't connect to your Doorman, or you are receiving the error Reading failed: BAD_INDICATOR errno=11 in the logs, this indicates Home Assistant may be trying to connect with an incorrect encryption key.

First, try restarting Home Assistant entirely to see if the issue persists. If it does, here are some things to try based on how you manage your Doorman's firmware updates.


#### OTA Updates / Firmware Binary

The pre-compiled firmware binaries that are used for OTA updates do not have an API encryption key built-in, therefore Home Assistant must be attempting to connect with an encryption key or deprecated API password. To remedy this, please try removing the integration entry for your Doorman by going to Settings > Devices & Services, then remove the ESPHome integration for your Doorman there. Then re-add your Doorman to Home Assistant by creating a new ESPHome integration using the device's IP or hostname. Do not specify an encryption key or password if prompted.

#### ESPHome Dashboard

If you manage updates with an ESPHome YAML, you must make sure that the encryption key you specify in the api: section of your configuration matches the encryption key that Home Assistant uses. Try re-adding the integration to Home Assistant, and make sure to specify the encryption key when prompted. You may also try removing the encryption key from your configuration to see if this remedies the issue. If it does, this may indicate an issue with Home Assistant or ESPHome itself.
:::

::: details Receiving commands does not work
If your Doorman is unable to receive commands there could be several reasons.

#### Check the wiring
Please make sure you wired everything correctly.\
You can find the guide [here](../getting-started#wiring).

#### Check the Bus voltage
There are cases where the bus voltage is inusfficient and causing the reading to fail.\
This can happen when other devices are drawing too much current.

You can check the a+b Bus voltage when receiving commands using an oscillosope.
If the bus voltage constantly goes up and down there's an issue with the power draw of other bus participants.
:::

## Other Issues

If you are experiencing other issues, here are some things to try:

- Reflash the firmware [here](../firmware/stock-firmware).

If everything else fails, please join the [Discord server](https://discord.gg/t2d34dvmBf) for support. If necessary, we will work to triage an issue on the GitHub repository.

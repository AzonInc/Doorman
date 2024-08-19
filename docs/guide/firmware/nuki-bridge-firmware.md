# Nuki Bridge Firmware <Badge type="tip" text="Arduino Framework" />

This Configuration inherits everything from the Stock Firmware and adds a Nuki Bridge Component via BLE on top.
It doesn't support Improv via Bluetooth LE because the BLE Stack is used by the Nuki Bridge Component.

### Update Firmware
There are a few ways to update the firmware:
- ESPHome OTA <Badge type="warning" text="Requires ESPHome Dashboard" />
- HTTP OTA <Badge type="tip" text="Latest release build, no customization" />
- Web Serial <Badge type="tip" text="Latest release build, no customization" />

You can connect your Doorman via USB-C and click the button below to install the latest and greatest Doorman Nuki Bridge Firmware right here via Web Serial.
<esp-web-install-button manifest="../../firmware/release/doorman-nuki-bridge/manifest.json">
    <button slot="activate">
        <div class="custom-layout">
            <a class="btn">Install or Update Firmware</a>
        </div>
    </button>
    <div slot="unsupported">
        <div class="danger custom-block">
            <p class="custom-block-title">OH SNAP!</p>
            <p>Your Browser doesn't support Web Serial :(</p>
        </div>
    </div>
    <div slot="not-allowed">
        <div class="danger custom-block">
            <p class="custom-block-title">OH SNAP!</p>
            <p>You are not allowed to use this on HTTP :(</p>
        </div>
    </div>
</esp-web-install-button>

## Firmware YAML
This is the minimal ESPHome configuration yaml file, make sure to change the API key.

::: details Minimal Nuki Bridge Firmware
```yaml
<!--@include: ../../../firmware/examples/nuki-bridge.example.yaml-->
```
:::

## Pairing your Nuki Lock
Depending on the PCB revision, press the `FLASH` or `PRG` button on the Doorman PCB for 5 seconds until the Status LED starts flashing purple or turn on the `Nuki Pairing Mode` switch in Home Assistant. Press the Button on your Nuki Lock for 5 seconds until the light-ring turns on.

After a successfully pairing the Status LED will stay constantly turned on for 3 seconds.
::: warning
If you already paired your Nuki Lock this will unpair it!
:::

## Unpairing your Nuki Lock
You can use the `Nuki Unpair Device` Button in Home Assistant or the physical `FLASH` or `PRG` button on the Doorman PCB.

::: info Physical Button
If your Lock is already paired with Doorman, press the `FLASH` or `PRG` button on the Doorman PCB for 5 seconds until the Status LED starts flashing purple.
Your Nuki Lock is now unpaired. The pairing mode will time out after 30 seconds.
:::

<!--@include: ./additions.md-->
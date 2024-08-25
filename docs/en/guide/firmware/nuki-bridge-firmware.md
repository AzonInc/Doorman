# Nuki Bridge Firmware <Badge type="tip" text="Arduino Framework" />

This configuration inherits all features from the Stock Firmware and adds a Nuki Bridge component via BLE. Note that it does not support Improv via Bluetooth LE, as the BLE stack is used by the Nuki Bridge component.

### Update Firmware
There are several ways to update the firmware:
- ESPHome OTA <Badge type="warning" text="Requires ESPHome Dashboard" />
- HTTP OTA <Badge type="tip" text="Latest release build, no customization" />
- Web Serial <Badge type="tip" text="Latest release build, no customization" />

You can connect your Doorman via USB-C and click the button below to install the latest Doorman Nuki Bridge Firmware directly through Web Serial.
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
This is the minimal ESPHome configuration YAML file. Be sure to update the API key.

::: details Minimal Nuki Bridge Firmware
```yaml
<!--@include: ../../../firmware/examples/nuki-bridge.example.yaml-->
```
:::

## Pairing your Nuki Lock
Depending on the PCB revision, either press the FLASH or PRG button on the Doorman PCB for 5 seconds until the status LED starts flashing purple, or enable the Nuki Pairing Mode switch in Home Assistant. Then, press the button on your Nuki Lock for 5 seconds until the light ring turns on.

After a successful pairing, the status LED will remain steadily on for 3 seconds.

::: warning
If your Nuki Lock is already paired, this process will unpair it!
:::

## Unpairing your Nuki Lock
You can unpair your device using either the `Nuki Unpair Device` button in Home Assistant or the physical `FLASH` or `PRG` button on the Doorman PCB.

::: info Physical Button
If your lock is already paired with Doorman, press the `FLASH` or `PRG` button on the Doorman PCB for 5 seconds until the RGB status LED starts flashing purple. Your Nuki Lock will then be unpaired. Note that the pairing mode will time out after 30 seconds.
:::

<!--@include: ./additions.md-->
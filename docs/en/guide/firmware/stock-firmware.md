# Stock Firmware <Badge type="tip" text="ESP-IDF Framework" />

This is the preferred firmware as it works out of the box with the Doorman S3 and supports Improv via Bluetooth LE.

### Update Firmware
There are several ways to update the firmware:
- ESPHome OTA <Badge type="warning" text="Requires ESPHome Dashboard" />
- HTTP OTA <Badge type="tip" text="Latest release build, no customization" />
- Web Serial <Badge type="tip" text="Latest release build, no customization" />

You can connect your Doorman via USB-C and click the button below to install the latest Doorman Stock Firmware (Home Assistant) directly through Web Serial.
To use the MQTT firmware, adopt Doorman into your ESPHome Dashboard and apply the MQTT Example Firmware YAML.
<esp-web-install-button manifest="../../firmware/release/doorman-stock/manifest.json">
    <button slot="activate">
        <div class="custom-layout">
            <a class="btn">Install or Update Firmware (Home Assistant)</a>
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
This is the minimal ESPHome configuration YAML file for use with Home Assistant. Be sure to update the API key.
::: details Minimal Stock Firmware (Home Assistant)
```yaml
<!--@include: ../../../../firmware/examples/ha-stock.example.yaml-->
```
:::

This is the minimal ESPHome configuration YAML file for use with MQTT. Be sure to update the Broker details.
::: details Minimal Stock Firmware (MQTT)
```yaml
<!--@include: ../../../../firmware/examples/mqtt-stock.example.yaml-->
```
:::

<!--@include: ./additions.md-->

<!--@include: ./mqtt.md-->
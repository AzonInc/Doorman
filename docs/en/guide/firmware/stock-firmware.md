# Stock Firmware <Badge type="tip" text="ESP-IDF Framework" />

This is the preferred firmware as it works out of the box with the Doorman S3 and supports Improv via Bluetooth LE.

### Update Firmware
There are several ways to update the firmware:
- ESPHome OTA <Badge type="warning" text="Requires ESPHome Dashboard, full control" />
- HTTP OTA <Badge type="tip" text="Latest release build, no customization" />
- Web Serial <Badge type="tip" text="Latest release build, no customization" />

Connect your Doorman via USB-C and click the button below to directly install the latest Doorman Stock Firmware (Home Assistant) using Web Serial.
This allows you to easily apply community-provided updates later.
<esp-web-install-button manifest="../../firmware/release/esp32-s3-oct.ha.stock/manifest.json">
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

::: warning
Be aware that some **updates may introduce breaking changes** — always review the changelogs carefully to ensure your system functions reliably in critical situations.
:::

To fully customize the firmware and gain complete control, you can adopt it into your ESPHome Dashboard. If you're interested in using the MQTT firmware, simply apply the `Minimal Stock Firmware (MQTT)` example YAML below.

## Firmware YAML
This is the minimal ESPHome configuration YAML file for use with Home Assistant. Be sure to update the API key.
::: details Minimal Stock Firmware (Home Assistant)
```yaml
<!--@include: ../../../../firmware/examples/esp32-s3-oct.ha.stock.master.example.yaml-->
```
:::

This is the minimal ESPHome configuration YAML file for use with MQTT. Be sure to update the Broker details.
::: details Minimal Stock Firmware (MQTT)
```yaml
<!--@include: ../../../../firmware/examples/esp32-s3-oct.mqtt.stock.master.example.yaml-->
```
:::

<!--@include: ./additions.md-->

<!--@include: ./mqtt.md-->
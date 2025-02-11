# Stock Firmware <Badge type="tip" text="ESP-IDF Framework" />

This is the preferred firmware as it works out of the box with the Doorman S3 and supports Improv via Bluetooth LE.

## Install or Update Firmware

There are several ways to update the firmware.

::: warning
Be aware that some **updates may introduce breaking changes** — always review the changelogs carefully to ensure your system functions reliably in critical situations.
:::

#### Web Serial <Badge type="tip" text="Latest release build, no customization" />
Connect your **Doorman-S3** via USB-C and click the button below to install or update the latest firmware directly via Web Serial, enabling seamless integration with the Home Assistant API. This setup ensures you can effortlessly apply community-driven updates in the future.

<esp-web-install-button manifest="../../firmware/release/esp32-s3.ha.stock/manifest.json">
    <button slot="activate">
        <div class="custom-layout">
            <a class="btn">Install or Update Firmware (Doorman-S3)</a>
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

#### HTTP OTA Updates <Badge type="tip" text="Latest release build, no customization" />
By integrating Doorman with Home Assistant, you unlock the convenience of automated firmware builds and seamless updates to the latest releases. The Update will appear directly in your Update list, allowing you to initiate an OTA update with just a click of the Update button — simple, efficient, and hassle-free.

#### ESPHome CLI / Dashboard <Badge type="warning" text="Full customization" />
For full customization and complete control over the firmware, you can adopt it into your ESPHome Dashboard or utilize the ESPHome CLI by executing:  
`esphome run <yamlfile.yaml>`

If you prefer using the MQTT-only firmware, simply apply the provided MQTT example YAML tailored for your board below.

##### Board Configuration Options: 
::: details Doorman-S3 (1.5) / ESP32-S3 (Octal PSRAM)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32-s3.ha.stock.master.example.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/examples/esp32-s3.mqtt.stock.master.example.yaml-->
```
:::

::: details Doorman-S3 (1.4) / ESP32-S3 (Quad PSRAM)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32-s3-quad.ha.stock.master.example.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/examples/esp32-s3-quad.mqtt.stock.master.example.yaml-->
```
:::

::: details Generic ESP32
#### GPIO configuration
- **TC:BUS RX:** GPIO 22
- **TC:BUS TX:** GPIO 23
- **Status LED:** GPIO 2
- **RGB Status LED:** GPIO 4
- **Relay:** GPIO 21
- **External button:** GPIO 20
- **ADC Input:** GPIO 36

::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32.ha.stock.master.example.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/examples/esp32.mqtt.stock.master.example.yaml-->
```
:::

::: details Generic ESP8266
#### GPIO configuration
- **TC:BUS RX:** GPIO 12
- **TC:BUS TX:** GPIO 13
- **Status LED:** GPIO 2
- **RGB Status LED:** Dummy
- **Relay:** GPIO 14
- **External button:** GPIO 15
- **ADC Input:** A0

::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp8266.ha.stock.master.example.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/examples/esp8266.mqtt.stock.master.example.yaml-->
```
:::

<!--@include: ./additions.md-->

<!--@include: ./mqtt.md-->
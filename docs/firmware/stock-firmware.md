# Stock Firmware

This is the prefered Firmware as it works out of the Box with Doorman S3.
It supports Improv via Bluetooth LE.

### Update Firmware
There are a few ways to update the firmware:
- ESPHome OTA <Badge type="warning" text="Requires ESPHome Dashboard" />
- HTTP OTA <Badge type="tip" text="Latest release build" />
- Web Serial <Badge type="tip" text="Latest release build & custom firmware" />

To use Web Serial, connect your Doorman via USB-C and click the button below to install the latest and greatest Doorman Stock Firmware.
<esp-web-install-button manifest="https://doorman.azon.ai/firmware/release/doorman-stock-manifest.json">
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

## Entities

### Sensors
- Last Bus Command

### Binary Sensors
- Entrance Doorbell
- Apartment Doorbell
- Pick up phone <Badge type="info" text="Disabled by default" />
- Hang up phone <Badge type="info" text="Disabled by default" />
- External Button <Badge type="info" text="Disabled by default" />

### Switches
- Ring To Open
- Relay <Badge type="info" text="Disabled by default" />

### Buttons
- Open Entrance Door
- Open Second Door <Badge type="info" text="Disabled by default" />
- Restart <Badge type="info" text="Disabled by default" />
- Safe mode <Badge type="info" text="Disabled by default" />

### Events
- Doorbell Pattern
- Phone pickup Pattern

### Configuration Inputs
- Apartment Doorbell Command
- Entrance Doorbell Command
- Open Entrance Door Command
- Open Second Door Command <Badge type="info" text="Disabled by default" />
- Pick up phone Command <Badge type="info" text="Disabled by default" />
- Hang up phone Command <Badge type="info" text="Disabled by default" />
- Ring To Open Delay <Badge type="info" text="Disabled by default" />



## Firmware
::: details Minimal Stock Firmware
```yaml
<!--@include: ../../firmware/examples/stock.example.yaml-->
```
:::

<!--@include: ./additions.md-->
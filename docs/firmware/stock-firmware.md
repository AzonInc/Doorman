# Stock Firmware

This is the prefered Firmware as it works out of the Box with Doorman S3.
It supports Improv via Bluetooth LE.

### Update Firmware
Click on Connect to install the latest and greatest Doorman Stock Firmware.
<esp-web-install-button manifest="https://doorman.azon.ai/firmware/release/doorman-stock-manifest.json"></esp-web-install-button>

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
- Doorbell Pattern Event

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
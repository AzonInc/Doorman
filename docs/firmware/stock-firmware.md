# Stock Firmware <Badge type="tip" text="ESP-IDF Framework" />

This is the prefered Firmware as it works out of the Box with Doorman S3.
It supports Improv via Bluetooth LE.

### Update Firmware
There are a few ways to update the firmware:
- ESPHome OTA <Badge type="warning" text="Requires ESPHome Dashboard" />
- HTTP OTA <Badge type="tip" text="Latest release build" />
- Web Serial <Badge type="tip" text="Latest release build & custom firmware" />

You can connect your Doorman via USB-C and click the button below to install the latest and greatest Doorman Stock Firmware right here via Web Serial.
<esp-web-install-button manifest="https://doorman.azon.ai/firmware/release/doorman-stock/manifest.json">
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
- Last Bus Command `last_bus_command`

### Binary Sensors
- Entrance Doorbell `entrance_doorbell`
- Apartment Doorbell `apartment_doorbell`
- Pick up phone `pick_up_phone` <Badge type="info" text="Disabled by default" />
- Hang up phone `hang_up_phone` <Badge type="info" text="Disabled by default" />
- External Button `doorman_external_button` <Badge type="info" text="Disabled by default" />

### Switches
- Ring To Open `doorman_ring_to_open`
- Relay `doorman_relay` <Badge type="info" text="Disabled by default" />

### Buttons
- Open Entrance Door `open_entrance_door`
- Open Second Door `open_second_door` <Badge type="info" text="Disabled by default" />
- Restart `doorman_restart` <Badge type="info" text="Disabled by default" />
- Safe mode `doorman_safe_mode` <Badge type="info" text="Disabled by default" />
- Restore Factory Settings `doorman_factory_reset` <Badge type="info" text="Disabled by default" />

### Events
- Doorbell Pattern `doorbell_pattern`
- Phone pickup Pattern `phone_pickup_pattern`

### Configuration Inputs
- Apartment Doorbell Command `apartment_doorbell_command_input`
- Entrance Doorbell Command `entrance_doorbell_command_input`
- Open Entrance Door Command `open_entrance_door_command_input`
- Open Second Door Command `open_second_door_command_input` <Badge type="info" text="Disabled by default" />
- Pick up phone Command `pick_up_phone_command_input` <Badge type="info" text="Disabled by default" />
- Hang up phone Command `hang_up_phone_command_input` <Badge type="info" text="Disabled by default" />
- Ring To Open Toggle Command `ring_to_open_toggle_command` <Badge type="info" text="Disabled by default" />
- Ring To Open Delay `ring_to_open_delay` <Badge type="info" text="Disabled by default" />



## Firmware
::: details Minimal Stock Firmware
```yaml
<!--@include: ../../firmware/examples/stock.example.yaml-->
```
:::

<!--@include: ./additions.md-->
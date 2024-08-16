# Stock Firmware <Badge type="tip" text="ESP-IDF Framework" />

This is the prefered Firmware as it works out of the Box with Doorman S3.
It supports Improv via Bluetooth LE.

### Update Firmware
There are a few ways to update the firmware:
- ESPHome OTA <Badge type="warning" text="Requires ESPHome Dashboard" />
- HTTP OTA <Badge type="tip" text="Latest release build, no customization" />
- Web Serial <Badge type="tip" text="Latest release build, no customization" />

You can connect your Doorman via USB-C and click the button below to install the latest and greatest Doorman Stock Firmware right here via Web Serial.
<esp-web-install-button manifest="./release/doorman-stock/manifest.json">
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

::: details Minimal Stock Firmware
```yaml
<!--@include: ../../firmware/examples/stock.example.yaml-->
```
:::

<!--@include: ./additions.md-->

## Entity Overview

### Sensors
- Last Bus Command `last_bus_command`
- Uptime `doorman_uptime`

### Binary Sensors
- Entrance Doorbell `entrance_doorbell`
- Apartment Doorbell `apartment_doorbell`
- Pick up phone `pick_up_phone` <Badge type="info" text="Disabled by default" />
- Hang up phone `hang_up_phone` <Badge type="info" text="Disabled by default" />
- Ring To Open: Toggle `ring_to_open_toggle` <Badge type="info" text="Internal" />
- Turn on light: Trigger `turn_on_light_trigger` <Badge type="info" text="Internal" />
- External Button `doorman_external_button` <Badge type="info" text="Disabled by default" />

### Switches
- Ring To Open `doorman_ring_to_open`
- Ring To Open: Confirmation `doorman_ring_to_open_confirmation` <Badge type="info" text="Disabled by default" />
- Relay `doorman_relay` <Badge type="info" text="Disabled by default" />

### Buttons
- Open Entrance Door `open_entrance_door`
- Open Second Door `open_second_door` <Badge type="info" text="Disabled by default" />
- Turn on the light `turn_on_light` <Badge type="info" text="Disabled by default" />
- Restart `doorman_restart` <Badge type="info" text="Disabled by default" />
- Safe mode `doorman_safe_mode` <Badge type="info" text="Disabled by default" />
- Restore Factory Settings `doorman_factory_reset` <Badge type="info" text="Disabled by default" />

### Events
- Doorbell Pattern `doorbell_pattern`
- Phone pick up Pattern `phone_pick_up_pattern`

### Configuration Inputs
- Command: Apartment Doorbell `apartment_doorbell_command_input`
- Command: Entrance Doorbell `entrance_doorbell_command_input`
- Command: Open Entrance Door `open_entrance_door_command_input`
- Command: Open Second Door `open_second_door_command_input` <Badge type="info" text="Disabled by default" />
- Command: Pick up phone `pick_up_phone_command_input` <Badge type="info" text="Disabled by default" />
- Command: Hang up phone `hang_up_phone_command_input` <Badge type="info" text="Disabled by default" />
- Command: Turn on light `turn_on_light_input` <Badge type="info" text="Disabled by default" />
- Command: Ring To Open Toggle `ring_to_open_toggle_command_input` <Badge type="info" text="Disabled by default" />
- Ring To Open: Delay `doorman_ring_to_open_delay` <Badge type="info" text="Disabled by default" />
- Ring To Open: Timeout `doorman_ring_to_open_timeout_mode` <Badge type="info" text="Disabled by default" />

### Firmware Update
- Firmware (stable) `update_http_request_stable`
- Firmware (dev) `update_http_request_dev`
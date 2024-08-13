# Nuki Bridge Firmware <Badge type="tip" text="Arduino Framework" />

This Configuration inherits everything from the Stock Firmware and adds a Nuki Bridge Component via BLE on top.
It doesn't support Improv via Bluetooth LE because the BLE Stack is used by the Nuki Bridge Component.

### Update Firmware
There are a few ways to update the firmware:
- ESPHome OTA <Badge type="warning" text="Requires ESPHome Dashboard" />
- HTTP OTA <Badge type="tip" text="Latest release build, no customization" />
- Web Serial <Badge type="tip" text="Latest release build, no customization" />

You can connect your Doorman via USB-C and click the button below to install the latest and greatest Doorman Nuki Bridge Firmware right here via Web Serial.
<esp-web-install-button manifest="https://doorman.azon.ai/firmware/release/doorman-nuki-bridge/manifest.json">
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
<!--@include: ../../firmware/examples/nuki-bridge.example.yaml-->
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

## Entity Overview

### Locks
- Nuki Lock `nuki_smart_lock`

### Sensors
- Last Bus Command `last_bus_command`
- Nuki Battery Level `nuki_battery_level`

### Binary Sensors
- Entrance Doorbell `entrance_doorbell`
- Apartment Doorbell `apartment_doorbell`
- Pick up phone `pick_up_phone` <Badge type="info" text="Disabled by default" />
- Hang up phone `hang_up_phone` <Badge type="info" text="Disabled by default" />
- External Button `doorman_external_button` <Badge type="info" text="Disabled by default" />
- Nuki Connected `nuki_connected`
- Nuki Paired `nuki_paired`
- Nuki Battery Critical `nuki_battery_critical`
- Nuki Door Sensor `nuki_door_sensor` <Badge type="info" text="Disabled by default" />
- Nuki Door Sensor State `nuki_door_sensor_state` <Badge type="info" text="Disabled by default" />

### Switches
- Ring To Open `doorman_ring_to_open`
- Ring To Open: Confirmation `doorman_ring_to_open_confirmation` <Badge type="info" text="Disabled by default" />
- Relay `doorman_relay` <Badge type="info" text="Disabled by default" />
- Nuki Pairing Mode `nuki_pairing_mode` <Badge type="info" text="Disabled by default" />

### Buttons
- Open Entrance Door `open_entrance_door`
- Open Second Door `open_second_door` <Badge type="info" text="Disabled by default" />
- Restart `doorman_restart` <Badge type="info" text="Disabled by default" />
- Safe mode `doorman_safe_mode` <Badge type="info" text="Disabled by default" />
- Restore Factory Settings `doorman_factory_reset` <Badge type="info" text="Disabled by default" />
- Nuki Unpair Device `nuki_unpair_device` <Badge type="info" text="Disabled by default" />

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
- Command: Ring To Open Toggle `ring_to_open_toggle_command_input` <Badge type="info" text="Disabled by default" />
- Ring To Open: Delay `doorman_ring_to_open_delay` <Badge type="info" text="Disabled by default" />
- Ring To Open: Timeout `doorman_ring_to_open_timeout_mode` <Badge type="info" text="Disabled by default" />
# Nuki Bridge Firmware

This Configuration inherits everything from the Stock Firmware and adds a Nuki Bridge Component via BLE on top.
It doesn't support Improv via Bluetooth LE because the BLE Stack is used by the Nuki Bridge Component.

### Update Firmware
Click on Connect to install the latest and greatest Doorman Nuki Bridge Firmware.
<esp-web-install-button manifest="https://doorman.azon.ai/firmware/release/doorman-nuki-bridge-manifest.json"></esp-web-install-button>

## Entities

### Locks
- Nuki Lock

### Sensors
- Nuki Battery Level
- Last Bus Command

### Binary Sensors
- Nuki Connected
- Nuki Paired
- Nuki Battery Critical
- Nuki Door Sensor <Badge type="info" text="Disabled by default" />
- Nuki Door Sensor State <Badge type="info" text="Disabled by default" />
- Entrance Doorbell
- Apartment Doorbell
- Pick up phone <Badge type="info" text="Disabled by default" />
- Hang up phone <Badge type="info" text="Disabled by default" />
- External Button <Badge type="info" text="Disabled by default" />

### Switches
- Nuki Pairing Mode <Badge type="info" text="Disabled by default" />
- Ring To Open
- Relay <Badge type="info" text="Disabled by default" />

### Buttons
- Nuki Unpair Device <Badge type="info" text="Disabled by default" />
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


## Firmware
::: details Minimal Nuki Bridge Firmware
```yaml
<!--@include: ../../firmware/examples/nuki-bridge.example.yaml-->
```
:::

<!--@include: ./additions.md-->
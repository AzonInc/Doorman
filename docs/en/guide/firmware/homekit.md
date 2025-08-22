# HomeKit Integration

This page provides everything you need to set up and control your Doorman device with Apple HomeKit.

## Limitations
The HomeKit integration currently supports only the most essential and basic functions of your Doorman. Advanced settings and less commonly used features are not available through HomeKit and must still be managed via Home Assistant or the web interface.

### Supported Entites
- **Event:** Entrance Doorbell
- **Event:** Second Entrance Doorbell
- **Event:** Apartment Doorbell
- **Lock:** Entrance Door
- **Lock:** Second Entrance Door
- **Switch:** Ring To Open
- **Light:** Hallway Light

### Event Mapping
HomeKit supports only three event types, so we are limited to the following mappings:

- **single** → `SINGLE_PRESS`
- **double** → `SINGLE_PRESS`
- **long** → `LONG_PRESS`

All other events will default to `LONG_PRESS` due to limitations:

- **triple** → `LONG_PRESS`
- **quadruple** → `LONG_PRESS`

## Accessory Bundles
To improve usability and optimize internal memory usage, we offer the following predefined accessory bundles:

- **Entrance Doorbell** → *Entrance Door*, *Hallway Light*
- **Second Entrance Doorbell** → *Second Entrance Door*, *Hallway Light*

## Pairing
<!--@include: ./instructions/homekit.md-->

## Reset Pairing
To unpair all controllers currently paired with your Doorman,  
click the `Reset HomeKit Pairing` button in the `HomeKit Configuration` section of the Doorman web interface.
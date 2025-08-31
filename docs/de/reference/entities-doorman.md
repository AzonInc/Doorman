# Doorman Entities
On this page, you can view all the entities related to Doorman.

## Sensors

### Uptime <Badge type="tip" text="doorman_uptime" /> <Badge type="info" text="Disabled by default" />
Reports the uptime in seconds.

### WiFi Signal <Badge type="tip" text="doorman_wifi_signal" /> <Badge type="info" text="Disabled by default" />
Reports the wifi signal in percent.


## Text Sensors

### ESPHome Version <Badge type="tip" text="esphome_version" /> <Badge type="info" text="Disabled by default" />
Reports the installed base ESPHome version.

### Doorman Firmware Version <Badge type="tip" text="doorman_firmware_version" /> <Badge type="info" text="Disabled by default" />
Reports the installed Doorman firmware version and build time.

### Doorman Hardware <Badge type="tip" text="doorman_hardware_version" /> <Badge type="info" text="Disabled by default" />
Reports the Doorman hardware revision.


## Switches

### Relay <Badge type="tip" text="doorman_relay" /> <Badge type="info" text="Disabled by default" />
Controls the built-in relay.

### Status LED: Show Bus Activity <Badge type="tip" text="doorman_status_led_bus_activity" /> <Badge type="info" text="Disabled by default" />
When enabled, the status LED will briefly blink to indicate activity on the bus — useful for debugging or just keeping an eye on communication.

### Experimental Firmware <Badge type="tip" text="dev_firmware" />
Enables experimental updates, allowing you to easily switch between the master and dev branches to check for the latest changes.


## Buttons

### Install Update <Badge type="tip" text="update_install" />
Installs the latest available Doorman Firmware update.

### Restart <Badge type="tip" text="doorman_restart" /> <Badge type="info" text="Disabled by default" />
Restarts the Doorman unit, useful after configuration changes that require a restart.

### Safe mode <Badge type="tip" text="doorman_safe_mode" /> <Badge type="info" text="Disabled by default" />
Restarts the Doorman unit into safe mode, useful for OTA updates if the unit is too busy.

### Restore Factory Settings <Badge type="tip" text="doorman_factory_reset" /> <Badge type="info" text="Disabled by default" />
Restarts the Doorman unit and loads factory defaults.
::: danger USE WITH GREAT CAUTION
All credentials, global variables, counters, and saved states stored in non-volatile memory will be lost with no chance of recovery. Even raw reading of flash memory with esptool will not help, as data is physically erased from flash memory.

If you configured WiFi using the captive portal, Improv Serial, or Improv BLE, this will reset WiFi settings as well, making such devices offline. You'll need to reconfigure the device using a built-in WiFi access point and captive portal.
:::


## Selects

### Relay Mode <Badge type="tip" text="relay_mode" /> <Badge type="info" text="Disabled by default" />
Configure how the internal relay should be switched — either standalone or triggered when a telegram to open the entrance doors is received. For entrance doors, the relay is switched off again after 5 seconds (default).

#### Modes:
- **Standalone**
- **Entrance Door Opener**
- **Second Entrance Door Opener**
- **Any Door Opener**


## Events

### Flash Button <Badge type="tip" text="doorman_boot_button_event" /> <Badge type="info" text="Disabled by default" />
Triggers when the `FLASH` or `PRG` button on the PCB is pressed.

##### Event Types
- **single_press**
- **double_press**
- **long_press**

### External Button <Badge type="tip" text="doorman_external_button_event" /> <Badge type="info" text="Disabled by default" />
Triggers when the external button is pressed.

##### Event Types
- **single_press**
- **double_press**
- **long_press**


## Updates

### Doorman Firmware Update <Badge type="tip" text="update_http_request" />
Shows if an doorman firmware update is available and offers installation via the HTTP OTA update mechanism.


## Lights

### Status LED <Badge type="danger" text="Red" /> <Badge type="tip" text="doorman_status_led" />
A small LED on the Doorman's PCB controlled by the ESPHome [Status LED](https://esphome.io/components/light/status_led.html) component.  By default, this LED remains on when everything is functioning properly but will blink if there are issues.

You can turn off the LED if it becomes annoying. Even when turned off, it will still retain its functionality and blink to signal any issues.

### Status LED <Badge type="warning" text="RGB" /> <Badge type="tip" text="doorman_rgb_status_led" />
A small WS2812B RGB LED on the Doorman's PCB is used to indicate specific events. The LED can display various colors and patterns to provide visual feedback on different states and actions.

##### Event Types
- **Waiting for WiFi / AP Enabled** <Badge type="tip" text="Fast orange pulse" />
- **Waiting for Home Assistant / MQTT / HomeKit to Connect** <Badge type="tip" text="Slow blue pulse" />
- **Home Assistant / MQTT / HomeKit Connected** <Badge type="tip" text="Solid blue light (for 3 seconds)" />
- **MQTT setup required** <Badge type="tip" text="Red-blue pulse" /> <Badge type="info" text="MQTT integration only" />
- **HomeKit pairing required** <Badge type="tip" text="Red-blue pulse" /> <Badge type="info" text="HomeKit integration only" />
- **Setup Mode is Active** <Badge type="tip" text="Fast green-turquoise pulse" />
- **Setup Mode completed** <Badge type="tip" text="Solid green-turquoise (for 3 seconds)" />
- **Ring To Open is Active** <Badge type="tip" text="Slow yellow pulse" />
- **Nuki Bridge Pairing Mode is Active** <Badge type="tip" text="Slow purple pulse" /> <Badge type="info" text="Nuki Bridge only" />
- **Nuki Bridge Paired Successfully** <Badge type="tip" text="Solid purple light (for 3 seconds)" /> <Badge type="info" text="Nuki Bridge only" />
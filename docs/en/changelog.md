# Changelog

## 2024.10.0
### 🚀 What's new?
- **TC:BUS Protocol Support**: Take a look at the [supported commands](reference/esphome-component#command-types).
   - Introducing intuitive command types with parameters like `address`, `payload`, and `serial_number`.
- **Interactive Setup Enhancements**: During initial setup, the system now saves the serial number of the indoor station, while also automatically detecting the address of a second outdoor station.
- **Expanded Functionality**: The doorbell and phone pickup patterns now work for a second entrance.
- **New Memory Management Tools**:
   - You can now modify ringtones and adjust volumes on indoor stations.
   - Currently supports TCS ISH/ISW3030 models. For other models, memory must be analyzed and implemented.
- **Programming Mode Actions**: Simply toggle the control units programming mode via ESPHome.

### ✨ Improvements
- **Entity Grouping**: On the webserver, entities are now better grouped for easier navigation and control.
- **Simplified ESPHome Configuration**: The configuration is now divided into separate add-ons, making it more modular and easier to manage.

### 📝 Other Updates
- The `tcs_intercom` component is now called `tc_bus`, with all documentation now centralized in this repository.

### ⛑️ BREAKING CHANGES
- **Re-setup Required**: Due to the new protocol implementation based on serial numbers, existing stored commands will no longer work. You'll need to reconfigure by pressing the apartment or entrance doorbell. [See detailed setup guide](guide/getting-started#step-3-interactive-setup).
- **Simplified Service Names**: We’ve made changes to the Home Assistant service names to make them easier to understand and use:

   - **Old Format**:
     ```yaml
     service: esphome.doorman_s3_send_tcs_command
     data:
       command: 0x1C30BA41
     ```

   - **New Format**:
     ```yaml
     service: esphome.doorman_s3_send_tc_command_raw
     data:
       command: 0x1C30BA41
     ```
     This keeps the original command format but uses a more consistent name (`tc_command_raw`).

   - **New Simplified Option**: For most cases, you can now use the easier, more descriptive command structure:
     ```yaml
     service: esphome.doorman_s3_send_tc_command
     data:
       type: floor_call  # Command type (e.g., 'floor_call', 'door_open')
       address: 0        # Address, for example the Outdoor Station
       payload: 0        # Data payload
       serial_number: 0  # Indoor Station serial number
     ```
     This format is more readable and helps you specify the type of command directly, making it much easier to manage.

## 2024.8.5
### 🚀 What's new?
- The Interactive Setup process now allows for seamless capture and storage of bus commands. It will start automatically the first time you use the firmware, provided you haven't configured commands previously.

### ✨ Improvements
- Resolved issue with dashboard import file names.
- Status colors have been enhanced for better visibility and clarity.

### 📝 Other Updates
- Use the [tcs_intercom](<https://github.com/AzonInc/ESPHome_tcs_intercom>) development branch for development builds.

## 2024.8.3
### 🚀 What's new?
- Simplified initial setup via Access Point, Improv Serial, or Improv BLE
- [Runtime configuration](<reference/entities#texts>) for bus commands and various settings
- [Pattern Event](<guide/automation/pattern-events>) entites (e.g., entrance/apartment doorbell, lift handset)
- Enhanced [Ring To Open](<guide/automation/ring-to-open>) (Party Mode 2.0) with customizable timeout settings
- Introduced a Door Opener Button for your secondary door (if applicable)
- Added a button to activate the hallway light (if applicable)
- New HTTP OTA Update Option for seamless upgrades to the latest stable Doorman release via Home Assistant (without ESPHome Dashboard)

### ✨ Improvements
- Install or Update the [Stock Firmware](<guide/firmware/stock-firmware>) directly from the documentation page. You no longer need to compile the firmware using the ESPHome Dashboard if you’re just setting up commands.
- Several additional minor improvements

### 📝 Other Updates
- Enable Webserver
- Set the minimum required ESPHome version to `2024.8.0`

### ⛑️ BREAKING CHANGES
This version will reset your existing saved commands. You will need to reconfigure them using the new configuration input entities. 
# Release Notes & Changelog
Welcome to the latest updates! Here's a breakdown of all the **new features**, **improvements**, and important **changes** you need to know. Be sure to check out the **Breaking Changes** section for any actions needed to keep everything running smoothly.

## 2025.1.0
### 🚀 What's New?
- **Added a Switch to turn off the Status LED while Ring to Open is active**  
   If you don't want the Status LED to blink while Ring to Open is active, you can now easily turn it off.

- **Automatic Model Detection**  
   The setup mode now attempts to automatically identify the indoor station model. However, this process is not compatible with all models, as some do not support automatic detection.

- **Introduced a button to identify your indoor station model**  
   You can now effortlessly determine the correct model for your settings by simply pressing the "Identify Indoor Station" button, perfect for cases where you're unsure which model you own.

- **Expand Support for Model Settings**  
   Implemented settings compatibility for TCS TASTA (Koch TC60) IVW5xxx and ISW5xxx models.

### ✨ Improvements
- **Fix Parser Command Length**  
   Previously, the command length was not properly parsed, which occasionally led to 32-bit commands being misinterpreted. This issue has now been resolved.
   
- **Configure Entrance Outdoor Station ID**  
   It is now feasible to replace the entrance outdoor station in the exceptional instances where non-default addresses are utilized. The setup mode will also set the entrance outdoor station address.

- **Automatically Disable BLE Server When Not Needed**  
   The BLE Server is now automatically disabled once Wi-Fi is connected. Note: This behavior applies exclusively to the Stock Firmware.

- **Fixed Memory Reading for Some Intercom Models**  
   The memory will now be correctly read from your indoor station.

- **Automatic Intercom Memory Reading**  
   The intercom memory is now automatically read during boot and after model identification. Manual memory readings are still possible but no longer necessary.

- **Experimental Update Switch**   
   Instead of having two separate update entities, there's now a single one that checks for updates based on a new switch. This switch lets you easily toggle experimental updates from the dev branch on or off.

### 📝 Other Updates
- **Switch Nuki Component branch**  
   Migrated the `nuki_lock` component to the `improve-memory-footprint` branch. This update leverages Doorman-S3's PSRAM, potentially enhancing the overall performance of the `nuki_lock` component.

### 🚨 Breaking Changes
- **Hexadecimal Command-String Length changed**  
   With the command parser now fixed, the hexadecimal string representation has been updated to correctly display the `Last Bus Command` sensor.

- **Separate Event entities**  
   [Skaronator](https://github.com/AzonInc/Doorman/pull/37) introduced separate event entities for each physical doorbell button.  
   This enhancement enables event tracking on a per-button basis, providing more granular and precise support for doorbell interactions.  
   You will need to adjust your automations if you previously used the Doorbell Pattern Event Entity. Additionally, the event types have been changed.

   👉 **Check the [Entities](../reference/entities#events) for details!**

- **Intercom Models Renamed**  
   As part of streamlining the models for each manufacturer, you may need to reconfigure your intercom model.
   Now, you can also see the Koch and Scantron models.

   👉 **Check the [Model Setting availability](../reference/esphome-component#model-setting-availability) for details!**

- **Intercom Settings Updated**  
   To accommodate compatibility with new models, the settings `ringtone_door_call` and `volume_handset` have been renamed.  

   👉 **Refer to the [Setting Types](../reference/esphome-component#setting-types) for the updated names and additional settings!**

## 2024.11.2
### ✨ Improvements
- **Fixed open door command**  
   Use the short open door command instead of the long one (with serial number) as this seems to cause issues on some setups.

## 2024.11.1
### ✨ Improvements
- **Fixed dev branch name**  
   Updated the development branch name to resolve future issues.

### 📝 Other Updates
- **Switch Nuki Component branch**  
   Migrated the `nuki_lock` component to the main branch as the new features got merged.

## 2024.11.0
### 🚀 What's New?
- **TC:BUS Protocol Support**  
   Unleash more power with the **new protocol support**!
   You can now use intuitive command types with parameters like `address`, `payload`, and `serial_number`.  

   👉 **Explore the [Supported Commands](../reference/esphome-component#command-types)**

- **Interactive Setup Enhancements**  
   The system now **remembers** your indoor station's serial number and automatically detects any additional outdoor stations during setup.

- **Expanded Doorbell Functionality**  
   Got more than one entrance? No problem! Your **doorbell and phone pickup patterns** now work seamlessly for a **second entrance**.

- **New Memory Management Tools**  
   You can now **customize ringtones** and **adjust volumes** of indoor stations.  

   👉 **Check the [Supported Models and Settings](../reference/esphome-component#model-setting-availability)**

- **Programming Mode Actions**  
   Just **toggle the control unit’s programming mode** using ESPHome for instant actions.

- **New Nuki Lock Settings**  
   With this release, we fully harness the expanded settings options of the Nuki Lock component, unlocking enhanced customization and control for an elevated user experience.

### ✨ Improvements
- **Better Entity Grouping**  
   Navigate with ease! **Entities** are now more logically grouped on the webserver for effortless control and access.

- **Simplified ESPHome Configuration**  
   We’ve made the ESPHome config files **more modular**! Configurations are now split into separate add-ons for smoother management.

- **Doorman Discovery**  
   We've introduced a new custom protocol that enables seamless discovery of other devices running this firmware for troubleshooting, as well as those utilizing the [custom Doorman firmware](../guide/firmware/custom-firmware) .

### 📝 Other Updates
- **Component Name Change**  
   The `tcs_intercom` component has been renamed to **`tc_bus`** and merged into this repository. All documentation is now centralized [here](../reference/esphome-component).

### 🚨 Breaking Changes
- **Re-setup Required!**  
   The new protocol means you’ll need to **reconfigure** your system. Old stored commands won't work anymore. Simply **press the apartment or entrance doorbell** to start the setup again.  

   👉 **Check the [setup guide](../guide/getting-started#step-3-interactive-setup) for details!**

- **New Service Names**  
   We’ve made changes to the Home Assistant service names to make them easier to understand and use:
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
   - **New User-Friendly Option**:
     ```yaml
     service: esphome.doorman_s3_send_tc_command
     data:
       type: floor_call  # Command type (e.g., 'floor_call', 'door_open')
       address: 0        # Address, for example the Outdoor Station
       payload: 0        # Data payload
       serial_number: 0  # Indoor Station serial number
     ```
     **Why it’s awesome**: It’s now **more readable** and lets you **easily specify** command types!


## 2024.8.5
### 🚀 What's New?
- **Enhanced Interactive Setup**  
   Setup is even easier! **Automatically capture and store bus commands** during first-time setup if no previous commands are detected.

### ✨ Improvements
- **Dashboard Import Fix**  
   Fixed those pesky import file name issues—no more confusion!

- **Better Status Colors**  
   We’ve revamped the **status colors** to improve visibility and clarity, making it easier to track everything at a glance.

### 📝 Other Updates
- **Development Builds**  
   We now use the [tcs_intercom](<https://github.com/AzonInc/ESPHome_tcs_intercom>) development branch for development builds.


## 2024.8.3
### 🚀 What's New?
- **Simplified Setup**  
   Initial setup is now a breeze with options like **Access Point**, **Improv Serial**, or **Improv BLE**.

- **Runtime Configuration**  
   Easily configure **bus commands** and settings on the fly!

- **New Event Entities**  
   New entities for doorbell and handset patterns (like **entrance or apartment doorbell**, and **lift handset**).

- **Ring to Open 2.0**  
   Enjoy an enhanced **Ring to Open** mode with fully customizable timeout settings.

- **New Door Opener Button**  
   Added a **button for secondary door** controls (if applicable).

- **Hallway Light Button**  
   Turn on your hallway lights with the new **dedicated button** (if applicable).

- **HTTP OTA Updates**  
   Seamlessly upgrade to the latest Doorman firmware via **Home Assistant**—no need for ESPHome Dashboard!

### ✨ Improvements
- **Direct Stock Firmware Updates**  
   Now you can **install or update** the stock firmware directly from the documentation page. No need to compile it separately!

- **General Tweaks**  
   Several small but useful improvements for a smoother experience.

### 📝 **Other Updates**
- **Webserver Enabled**  
   The webserver feature is now enabled for quicker access.

- **Minimum ESPHome Version Set**  
   We’ve bumped the minimum required version to **2024.8.0**.

### 🚨 Breaking Changes
- **Command Reset**  
   Due to the simplified setup process and configurable commands, the substituations are not used anymore. Thus you’ll need to **reconfigure** them using the new input entities.
# Release Notes & Changelog
Welcome to the latest updates! Here's a breakdown of all the **new features**, **improvements**, and important **changes** you need to know. Be sure to check out the **Breaking Changes** section for any actions needed to keep everything running smoothly.

## 2025.6.0 <Badge type="warning" text="Next" />
### 🚨 IMPORTANT
Please carefully review the breaking changes listed below before updating!  
This release **will impact your current setup** and **requires** you to go through the **setup process again**.

### 🚀 What's New?
- **Expanded Support for Model-Specific Settings**  
   Settings compatibility has been added for TCS TASTA / Koch TC60 (`IVW5xxx`, `ISW5xxx`) and Ecoos (`IVW22xx`) models.

- **Added Support for Acknowledgment Messages**  
   Acknowledgment messages are now properly handled, following additional investigation into previously unsupported cases.

- **Streamlined Door Control with Unified Lock Entities**  
  The `Open Door` buttons have been replaced by new, standardized lock entities.  
  You can now use the `lock.unlock` action in Home Assistant for seamless door control.  
  This change simplifies dashboard integration and delivers a more consistent, intuitive experience.

- **Use Light Entity instead of Button for Hallway Light**  
  The `Turn on the light` button has been replaced by the `Hallway Light` light entity.  
  You can now use the `light.turn_on` action in Home Assistant to turn on the hallway light. This change simplifies dashboard integration and delivers a more consistent, intuitive experience.

- **Extended Ring to Open to Apartment Doorbells**  
   [Ring To Open](https://doorman.azon.ai/guide/features/ring-to-open) now also works with your apartment doorbell when paired with a Nuki Smart Lock — making access even more convenient and seamless.

- **MQTT Integration**  
  The configuration assistant now allows you to select the [MQTT integration](https://doorman.azon.ai/guide/firmware/mqtt).  
  After flashing, just enter your broker credentials via the built-in web interface.

- **HomeKit Integration**  
  To enable native control through Apple's Home app and Siri, you can now choose the [HomeKit integration](https://doorman.azon.ai/guide/firmware/homekit) in the configuration assistant.

- **Pre-Open Telegrams**  
   In some setups, it’s necessary to send one or more preparatory telegrams to select the correct door. This feature allows you to define and send a sequence of telegrams before the `open_door` telegram is executed.

- **Door Opener Mode**  
   It is now possible to activate the integrated relay instead of sending a BUS telegram to open the entrance door.  
   This can be configured using the `Door Opener Mode` setting and is especially useful for older analog systems.

- **Show Bus Activity via Status LED**  
   Added a new switch that, when enabled, makes the status LED blink briefly to indicate bus activity. Helpful for debugging and verifying communication with the door system. Disabled by default.

### ✨ Improvements
- **Refined Configuration for Ring to Open**  
   The configuration options have been separated per door, giving you greater flexibility to customize the [Ring To Open](https://doorman.azon.ai/guide/features/ring-to-open) behavior according to your specific requirements.

   Additionally, the Delay settings are now presented as dropdown menus, making it more intuitive to use the random delay feature—which was previously hidden behind the 60-second setting.

- **Smarter Ring to Open with Pattern Recognition**  
  The door now opens only when a configured doorbell ring pattern is detected, offering more precise control over access.

- **Quadruple Doorbell Pattern**  
  A new quadruple-press pattern has been added, with slightly refined timing to improve reliability and avoid overlap with other patterns.

- **Extended Model Detection Support**  
   Detection capabilities now include additional older models like the `TTC-xx` and `TTS-xx`.

- **Enhanced Reliability with Remote Peripheral**  
   Data reading is now based on RMT, providing more reliability compared to the previous interrupt-based method.

- **Telegram Queue**  
   Outgoing telegrams are now queued to prevent data loss.

- **Install Update Button**  
   A new button lets you install the latest available Doorman firmware version directly from the web interface.

- **Wi-Fi Signal Sensor**  
   The default firmware now includes a Wi-Fi signal strength sensor, which was previously only available in the developer version.

### 📝 Other Updates
- **Firmware Configuration Assistant**  
   The new assistant makes flashing the firmware easier than ever — not much technical experience required.

- **Telegram Binary Sensors Disabled by Default**  
   Since event entities offer the most convenient way to work with the doorbell, binary sensors are now disabled by default when adding Doorman to Home Assistant. You can still enable them manually at any time if needed.

- **Enhanced Captive Portal UX**  
   Building upon the ESPHome foundation, Doorman now features a custom-branded captive portal, offering a more consistent UX for Wi-Fi configuration.

- **More Readable Log Messages**  
   Log messages have been refined for improved clarity and readability.

### 🚨 Breaking Changes
- **Renamed `command` to `telegram`**
  The `command` terminology has been replaced with `telegram`. If you're using the `tc_bus.send` action, you must update your configuration accordingly.  
  Additionally, the `CommandData` class has been renamed to `TelegramData`.  
  For full migration details, please refer to the updated ESPHome component documentation.

- **Transition to RMT Components**  
   The `TC:BUS` component no longer supports the `rx_pin` and `tx_pin` options.  
   Instead, you must now define the `remote_receiver` and `remote_transmitter` components when using your own custom config.

- **Removed Open Door and Light Buttons**  
  The `Turn on the light` and `Open Door` buttons have been removed in favor of more efficient control methods.

- **Renamed Ring to Open Entities**  
  The [Ring To Open](https://doorman.azon.ai/guide/features/ring-to-open) entities are now labeled as `RTO: Door Name – Setting` to reflect the updated configuration structure, improving clarity and consistency.

- **Renamed Intercom Model Entity**  
  The `Intercom Model` setting has been renamed to `Indoor Station Model` for more consistency with the documentation and to better reflect its purpose.

- **Renamed Nuki Lock**  
  The `Nuki Lock` entity has been renamed to `Apartment Door` for better alignment with the naming convention used for other doors.

- **Minimum ESPHome Version Set**  
   The minimum required version has been updated to **2025.5.0**.

## 2025.3.0 <Badge type="tip" text="Stable" />
### ✨ Improvements
- **Fix Update Process**  
   Firmware updates are now correctly identified and retrieved for each specific firmware variant.

- **Ring to Open Toggle Trigger**  
   You can now choose which trigger will toggle the Ring To Open automation as some models use the button for specific purposes.

- **Expanded Support for Model Detection**  
   Now, models without configuration settings are also detected.
   
- **Setup mode delays**  
   By introducing some delay, we allow the ringtone to finish on setup mode.

### 📝 Other Updates
- **Nuki Component**  
   The [ESPHome_nuki_lock](https://github.com/uriyacovy/ESPHome_nuki_lock) component now runs on IDF, removing the need for the Arduino framework.

- **Configuration YAML Files Restructured**  
   The configuration files have been reorganized to enhance modularity and enable support for additional host platforms.

- **Precompiled binaries for ESP32-S3, ESP32**  
   The Doorman ESPHome firmware is available for these platforms.

### 🚨 Breaking Changes
- **Ring to Open Toggle Trigger**  
   The new option is set to `Manual` by default. You'll need to set it to `Function Button` to restore the previous behavior.

- **Minimum ESPHome Version Set**  
   We’ve bumped the minimum required version to **2025.2.0**.

## 2025.2.0
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
- **Fix Parser Telegram Length**  
   Previously, the telegram length was not properly parsed, which occasionally led to 32-bit telegrams being misinterpreted. This issue has now been resolved.
   
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
- **Nuki Component**  
   The [ESPHome_nuki_lock](https://github.com/uriyacovy/ESPHome_nuki_lock) component now leverages Doorman-S3's PSRAM, potentially enhancing the overall performance.

### 🚨 Breaking Changes
- **PSRAM Compatibility**  
   Some users, particularly those with Revision 1.4 PCBs, may encounter issues due to the newly added PSRAM component. This is because certain Revision 1.4 boards use the N16R2 variant of the ESP32S3, which requires a different configuration for proper PSRAM booting.
   
   For assistance, please contact me via [Discord](https://discord.gg/t2d34dvmBf) or open an issue on [GitHub](https://github.com/azoninc/doorman/issues).

- **Hexadecimal Telegram-String Length changed**  
   With the telegram parser now fixed, the hexadecimal string representation has been updated to correctly display the [Last Bus Telegram](https://doorman.azon.ai/reference/entities#last-bus-telegram) sensor.

- **Separate Event entities**  
   [Skaronator](https://github.com/azoninc/doorman/pull/37) introduced separate event entities for each physical doorbell button.  
   This enhancement enables event tracking on a per-button basis, providing more granular and precise support for doorbell interactions.  
   You will need to adjust your automations if you previously used the Doorbell Pattern Event Entity. Additionally, the event types have been changed.

   👉 **Check the [Entities](https://doorman.azon.ai/reference/entities#events) for details!**

- **Intercom Models Renamed**  
   As part of streamlining the models for each manufacturer, you may need to reconfigure your intercom model.
   Now, you can also see the Koch and Scantron models.

   👉 **Check the [Model Setting availability](https://doorman.azon.ai/reference/esphome-component#model-setting-availability) for details!**

- **Intercom Settings Updated**  
   To accommodate compatibility with new models, the settings `ringtone_door_call` and `volume_handset` have been renamed.  

   👉 **Refer to the [Setting Types](https://doorman.azon.ai/reference/esphome-component#setting-types) for the updated names and additional settings!**

## 2024.11.2
### ✨ Improvements
- **Fixed open door telegram**  
   Use the short open door telegram instead of the long one (with serial number) as this seems to cause issues on some setups.

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
   You can now use intuitive telegram types with parameters like `address`, `payload`, and `serial_number`.  

   👉 **Explore the [Supported Telegrams](https://doorman.azon.ai/reference/esphome-component#telegram-types)**

- **Interactive Setup Enhancements**  
   The system now **remembers** your indoor station's serial number and automatically detects any additional outdoor stations during setup.

- **Expanded Doorbell Functionality**  
   Got more than one entrance? No problem! Your **doorbell and phone pickup patterns** now work seamlessly for a **second entrance**.

- **New Memory Management Tools**  
   You can now **customize ringtones** and **adjust volumes** of indoor stations.  

   👉 **Check the [Supported Models and Settings](https://doorman.azon.ai/reference/esphome-component#model-setting-availability)**

- **Programming Mode Actions**  
   Just **toggle the control unit’s programming mode** using ESPHome for instant actions.

- **New Nuki Lock Settings**  
   With this release, we fully harness the expanded settings options of the Nuki Lock component, unlocking enhanced customization and control for an elevated user experience.

### ✨ Improvements
- **Better Entity Grouping**  
   Navigate with ease! **Entities** are now more logically grouped on the web interface for effortless control and access.

- **Simplified ESPHome Configuration**  
   We’ve made the ESPHome config files **more modular**! Configurations are now split into separate add-ons for smoother management.

- **Doorman Discovery**  
   We've introduced a new custom protocol that enables seamless discovery of other devices running this firmware for troubleshooting, as well as those utilizing the [custom Doorman firmware](https://doorman.azon.ai/guide/firmware/custom-firmware) .

### 📝 Other Updates
- **Component Name Change**  
   The `tcs_intercom` component has been renamed to **`tc_bus`** and merged into this repository. All documentation is now centralized [here](https://doorman.azon.ai/reference/esphome-component).

### 🚨 Breaking Changes
- **Re-setup Required!**  
   The new protocol means you’ll need to **reconfigure** your system. Old stored telegrams won't work anymore. Simply **press the apartment or entrance doorbell** to start the setup again.  

   👉 **Check the [setup guide](https://doorman.azon.ai/guide/getting-started#interactive-setup) for details!**

- **New Service Names**  
   We’ve made changes to the Home Assistant service names to make them easier to understand and use:
   - **Old Format**:
     ```yaml
     service: esphome.doorman_s3_send_tcs_telegram
     data:
       telegram: 0x1C30BA41
     ```
   - **New Format**:
     ```yaml
     service: esphome.doorman_s3_send_tc_telegram_raw
     data:
       telegram: 0x1C30BA41
     ```
   - **New User-Friendly Option**:
     ```yaml
     service: esphome.doorman_s3_send_tc_telegram
     data:
       type: floor_call  # Telegram type (e.g., 'floor_call', 'door_open')
       address: 0        # Address, for example the Outdoor Station
       payload: 0        # Data payload
       serial_number: 0  # Indoor Station serial number
     ```
     **Why it’s awesome**: It’s now **more readable** and lets you **easily specify** telegram types!


## 2024.8.5
### 🚀 What's New?
- **Enhanced Interactive Setup**  
   Setup is even easier! **Automatically capture and store bus telegrams** during first-time setup if no previous telegrams are detected.

### ✨ Improvements
- **Dashboard Import Fix**  
   Fixed those pesky import file name issues—no more confusion!

- **Better Status Colors**  
   We’ve revamped the **status colors** to improve visibility and clarity, making it easier to track everything at a glance.

### 📝 Other Updates
- **Development Builds**  
   We now use the [tcs_intercom](<https://github.com/azoninc/esphome_tcs_intercom>) development branch for development builds.


## 2024.8.3
### 🚀 What's New?
- **Simplified Setup**  
   Initial setup is now a breeze with options like **Access Point**, **Improv Serial**, or **Improv BLE**.

- **Runtime Configuration**  
   Easily configure **bus telegrams** and settings on the fly!

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

### 🚨 Breaking Changes
- **Telegram Reset**  
   Due to the simplified setup process and configurable telegrams, the substituations are not used anymore. Thus you’ll need to **reconfigure** them using the new input entities.
   
- **Minimum ESPHome Version Set**  
   We’ve bumped the minimum required version to **2024.8.0**.

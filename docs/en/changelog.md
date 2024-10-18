# **Release Notes & Changelog**  
Welcome to the latest updates! Here's a breakdown of all the **new features**, **improvements**, and important **changes** you need to know. Be sure to check out the **Breaking Changes** section for any actions needed to keep everything running smoothly.

## 2024.10.0
### 🚀 What's New?
- **TC:BUS Protocol Support**  
   Unleash more power with the **new protocol support**!
   You can now use intuitive command types with parameters like `address`, `payload`, and `serial_number`.  

   👉 **Explore the [Supported Commands](reference/esphome-component#command-types)**

- **Interactive Setup Enhancements**  
   The system now **remembers** your indoor station's serial number and automatically detects any additional outdoor stations during setup.

- **Expanded Doorbell Functionality**  
   Got more than one entrance? No problem! Your **doorbell and phone pickup patterns** now work seamlessly for a **second entrance**.

- **New Memory Management Tools**  
   You can now **customize ringtones** and **adjust volumes** of indoor stations.  

   👉 **Check the [Supported Models and Settings](reference/esphome-component#model-setting-availability)**

- **Programming Mode Actions**  
   Just **toggle the control unit’s programming mode** using ESPHome for instant actions.

### ✨ **Improvements**
- **Better Entity Grouping**  
   Navigate with ease! **Entities** are now more logically grouped on the webserver for effortless control and access.

- **Simplified ESPHome Configuration**  
   We’ve made the ESPHome config files **more modular**! Configurations are now split into separate add-ons for smoother management.

### 📝 **Other Updates**
- **Component Name Change**  
   The `tcs_intercom` component has been renamed to **`tc_bus`** and merged into this repository. All documentation is now centralized [here](reference/esphome-component).

### 🚨 **Breaking Changes**
- **Re-setup Required!**  
   The new protocol means you’ll need to **reconfigure** your system. Old stored commands won't work anymore. Simply **press the apartment or entrance doorbell** to start the setup again.  

   👉 **Check the [setup guide](guide/getting-started#step-3-interactive-setup) for details!**

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

### ✨ **Improvements**  
- **Dashboard Import Fix**  
   Fixed those pesky import file name issues—no more confusion!

- **Better Status Colors**  
   We’ve revamped the **status colors** to improve visibility and clarity, making it easier to track everything at a glance.

### 📝 **Other Updates**
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

### ✨ **Improvements**
- **Direct Stock Firmware Updates**  
   Now you can **install or update** the stock firmware directly from the documentation page. No need to compile it separately!

- **General Tweaks**  
   Several small but useful improvements for a smoother experience.

### 📝 **Other Updates**
- **Webserver Enabled**  
   The webserver feature is now enabled for quicker access.

- **Minimum ESPHome Version Set**  
   We’ve bumped the minimum required version to **2024.8.0**.

### 🚨 **Breaking Changes**
- **Command Reset**  
   Due to the simplified setup process and configurable commands, the substituations are not used anymore. Thus you’ll need to **reconfigure** them using the new input entities.
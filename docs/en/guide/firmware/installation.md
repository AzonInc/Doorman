<script setup>
import { inBrowser } from 'vitepress'
</script>
<script>
export default {
    data() {
        return {
            test: '',
            baseUrl: '../../firmware/release/',
            platform: '',
            integration: '',
            variant: '',
            platform_options: [
                {
                    key: 'esp32-s3',
                    name: 'ESP32-S3 <span class="VPBadge tip">Octal</span>',
                    icon: '',
                    iconColor: '',
                    details: 'Recommended for the <b>Doorman-S3</b> and all ESP32-S3 boards with at least 8&nbsp;MB PSRAM.',
                },
                {
                    key: 'esp32-s3-quad',
                    name: 'ESP32-S3 <span class="VPBadge tip">Quad</span>',
                    icon: '',
                    iconColor: '',
                    details: 'Recommended for <b>Doorman-S3 (Revision 1.4)</b> and all ESP32-S3 boards with up to 4&nbsp;MB PSRAM.',
                },
                {
                    key: 'esp32',
                    name: 'ESP32',
                    icon: '',
                    iconColor: '',
                    details: 'Recommended for <b>all ESP32 boards without PSRAM</b>.',
                },
            ],
            integration_options: [
                {
                    key: 'ha',
                    name: 'Home Assistant',
                    icon: '<img src="/icons/home-assistant.svg" />',
                    iconColor: '',
                    details: 'I use Home Assistant and want the easiest way to integrate Doorman with my smart home.',
                },
                {
                    key: 'mqtt',
                    name: 'MQTT',
                    icon: '<img src="/icons/mqtt-ver-transp.svg" />',
                    iconColor: '#606',
                    details: 'I want Doorman to connect directly to my MQTT broker to use it with other platforms.',
                },
                {
                    key: 'homekit',
                    name: 'Apple HomeKit',
                    icon: '<img src="/icons/homekit.png" />',
                    iconColor: '',
                    details: 'I use Apple Home and want to connect Doorman easily through HomeKit.',
                },
                {
                    key: 'custom',
                    name: 'Standalone',
                    icon: '<img src="/icons/fluent-emoji-sparkles.png" />',
                    iconColor: '',
                    details: 'I don\'t use a smart home system — I just want Doorman to work on its own over Wi-Fi.',
                },
            ],
            variant_options: [
                {
                    key: 'stock',
                    name: 'Standard',
                    icon: '',
                    iconColor: '',
                    details: 'I only need the basic features to control my intercom — nothing extra.',
                },
                {
                    key: 'nuki-bridge',
                    name: 'Nuki Bridge',
                    icon: '',
                    iconColor: '',
                    details: 'I also want to control my Nuki Smart Lock and use Ring to Open with the apartment doorbell.',
                },
            ],
        }
    },
    watch: {
        platform(newPlatform, oldPlatform) {
            localStorage.setItem("fw_platform", newPlatform);
        },
        integration(newIntegration, oldIntegration) {
            localStorage.setItem("fw_integration", newIntegration);
            if (!this.is_variant_allowed(this.variant)) {
                this.variant = 'stock';
            }
        },
        variant(newVariant, oldVariant) {
            localStorage.setItem("fw_variant", newVariant);
        }
    },
    computed: {
        fw_string() {
            return [this.platform, this.integration, this.variant].join('.');
        },
        manifest_file() {
            return this.baseUrl + this.fw_string + '/manifest.json';
        },
        valid_manifest() {
            return this.platform && this.integration && this.variant;
        },
        notes() {
            if(this.integration == 'mqtt') {
                return 'The firmware requires further setup after flashing! Please take a look at the <a href="./mqtt#setup">MQTT Setup instructions</a>.';
            }
            else if(this.integration == 'homekit') {
                return 'Please take a look at the <a href="./homekit#pairing">HomeKit pairing instructions</a> after flashing.';
            }
            return '';
        },
        is_variant_allowed() {
            const incompat = {
                mqtt: ['nuki-bridge'],
                homekit: ['nuki-bridge'],
            };
            return (variant) => {
                if (!this.integration) return true;
                return !(incompat[this.integration] && incompat[this.integration].includes(variant));
            };
        },
        variant_incompatibility_message() {
            const integrationName = this.integration_options.find(opt => opt.key === this.integration)?.name || this.integration;
            return `Unfortunately this option can't be used together with <b>${integrationName}</b> because of technical limitations.`;
        },
        web_serial_available() {
            if(inBrowser) {
                return navigator.serial != undefined;
            }
            return false;
        }
    },
    async mounted() {
        if (!this.platform && localStorage.getItem("fw_platform") != null) {
            this.platform = localStorage.getItem("fw_platform");
        }
        if (!this.integration && localStorage.getItem("fw_integration") != null) {
            this.integration = localStorage.getItem("fw_integration");
        }
        if (!this.variant && localStorage.getItem("fw_variant") != null) {
            this.variant = localStorage.getItem("fw_variant");
        }
    }
}
</script>

# Install or Update Firmware

On this page, you'll find all available methods to install or update the Doorman firmware. Whether you're flashing the firmware for the first time or applying the latest update, the instructions here will guide you step-by-step — including direct installation via Web Serial, updates through the firmware web interface, and integration with Home Assistant's update system.

::: tip IMPORTANT
Be aware that some **updates may introduce breaking changes** — always review the changelogs carefully to ensure your system functions reliably in critical situations.
:::

## Web Serial <Badge type="tip" text="Latest release build, no customization" />
Connect your **Doorman-S3** via USB-C and use the configuration assistant below to install the latest firmware directly via Web Serial — perfect for first-time setup.

This guided process ensures seamless integration with the Home Assistant API and makes future community-driven updates easy to apply.

<div v-if="web_serial_available">
    <h5 class="firmware_title_row"><icon-ph-cpu-bold /> Which hardware do you use?</h5>
    <div class="firmware_option_row">
        <label class="firmware_option" v-for="fw_platform in platform_options" :key="fw_platform.key">
            <input type="radio" class="reset_default" v-model="platform" :value="fw_platform.key">
            <span class="checkmark">
                <div class="icon" v-if="fw_platform.icon" v-html="fw_platform.icon"></div>
                <div class="title" v-html="fw_platform.name"></div>
                <div class="details" v-html="fw_platform.details"></div>
            </span>
        </label>
    </div>
    <div v-if="platform">
        <h5 class="firmware_title_row"><icon-ic-round-other-houses /> Do you use any smart home system?</h5>
        <div class="firmware_option_row">
            <label class="firmware_option" v-for="fw_integration in integration_options" :key="fw_integration.key">
                <input type="radio" class="reset_default" v-model="integration" :value="fw_integration.key">
                <span class="checkmark">
                    <div class="icon" :style="{ 'background-color': fw_integration.iconColor }" v-if="fw_integration.icon" v-html="fw_integration.icon"></div>
                    <div class="title" v-html="fw_integration.name"></div>
                    <div class="details" v-html="fw_integration.details"></div>
                </span>
            </label>
        </div>
    </div>
    <div v-if="integration">
        <h5 class="firmware_title_row"><icon-mdi-package-variant-plus /> What about some extras?</h5>
        <div class="firmware_option_row">
            <label class="firmware_option" v-for="fw_variant in variant_options" :key="fw_variant.key">
                <input type="radio" class="reset_default" :disabled="!is_variant_allowed(fw_variant.key)" v-model="variant" :value="fw_variant.key">
                <span class="checkmark">
                    <div class="icon" v-if="fw_variant.icon" v-html="fw_variant.icon"></div>
                    <div class="title" v-html="fw_variant.name"></div>
                    <div class="details" v-html="is_variant_allowed(fw_variant.key) ? fw_variant.details : variant_incompatibility_message"></div>
                </span>
            </label>
        </div>
    </div>
    <div v-if="valid_manifest">
        <h5 class="firmware_title_row"><icon-line-md-uploading-loop /> Let's summon the firmware spirits!</h5>
        <div v-if="notes" class="note custom-block">
            <p class="custom-block-title">NOTE</p>
            <p v-html="notes"></p>
        </div>
        <esp-web-install-button :manifest="manifest_file">
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
    </div>
</div>
<div v-else>
    <div class="danger custom-block">
        <p class="custom-block-title">BROWSER INCOMPATIBLE</p>
        <p>Since your browser doesn't support Web Serial, the configuration assistant isn't available. Please use a Chromium-based browser instead.</p>
    </div>
</div>

## Integrated OTA Updates <Badge type="tip" text="Latest release build, no customization" />
The firmware supports OTA updates right out of the box, so you can install the latest version with a single click — quick, easy, and hassle-free.

When integrated with Home Assistant, Doorman updates will even show up directly in your Updates list, making it even more convenient to stay up to date.

To update, simply click the `Install Update` button in the firmware's web interface or use the Update list in Home Assistant.

## ESPHome CLI / Dashboard <Badge type="warning" text="Full customization" />
If you want full customization and the ability to add your own sensors, automations, or features, this is your way to go — ideal for power users who prefer complete control over their setup.

### ESPHome Dashboard
You can adopt Doorman to your [ESPHome Dashboard](https://my.home-assistant.io/redirect/supervisor_ingress/?addon=5c53de3b_esphome) and flash a customized firmware variant.

The adopted configuration could look like this:
```yaml
<!--@include: ../firmware/minimal.example.yaml-->
```

### ESPHome CLI
If you choose this option, you likely know what you're doing — just run the following telegram with your YAML configuration file.

If you prefer using the MQTT-only firmware, simply apply the provided MQTT example YAML tailored for your board below.

```sh
esphome run <yamlfile.yaml>
```

## Board Configuration Files

### Default GPIO configuration
| Component      | ESP32-S3 | ESP32    |
| -------------- | :------: | :------: |
| TC:BUS RX      | GPIO 9   | GPIO 22  |
| TC:BUS TX      | GPIO 8   | GPIO 23  |
| Status LED     | GPIO 1   | GPIO 2   |
| RGB Status LED | GPIO 2   | GPIO 4   |
| Relay          | GPIO 42  | GPIO 21  |
| External Button| GPIO 41  | GPIO 20  |
| ADC Input      | GPIO 10  | GPIO 36  |

### Stock Firmware
::: details ESP32-S3 (Octal PSRAM) / Doorman-S3 (1.5)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32-s3.ha.stock.master.example.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/examples/esp32-s3.mqtt.stock.master.example.yaml-->
```
```yaml [HomeKit]
<!--@include: ../../../../firmware/examples/esp32-s3.homekit.stock.master.example.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/examples/esp32-s3.custom.stock.master.example.yaml-->
```
:::

::: details ESP32-S3 (Quad PSRAM) / Doorman-S3 (1.4)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32-s3-quad.ha.stock.master.example.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/examples/esp32-s3-quad.mqtt.stock.master.example.yaml-->
```
```yaml [HomeKit]
<!--@include: ../../../../firmware/examples/esp32-s3-quad.homekit.stock.master.example.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/examples/esp32-s3-quad.custom.stock.master.example.yaml-->
```
:::

::: details Generic ESP32
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32.ha.stock.master.example.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/examples/esp32.mqtt.stock.master.example.yaml-->
```
```yaml [HomeKit]
<!--@include: ../../../../firmware/examples/esp32.homekit.stock.master.example.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/examples/esp32.custom.stock.master.example.yaml-->
```
:::

### Nuki Bridge Firmware
::: details ESP32-S3 (Octal PSRAM) / Doorman-S3 (1.5)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32-s3.ha.nuki-bridge.master.example.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/examples/esp32-s3.custom.nuki-bridge.master.example.yaml-->
```
:::

::: details ESP32-S3 (Quad PSRAM) / Doorman-S3 (1.4)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32-s3-quad.ha.nuki-bridge.master.example.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/examples/esp32-s3-quad.custom.nuki-bridge.master.example.yaml-->
```
:::

::: details Generic ESP32
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32.ha.nuki-bridge.master.example.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/examples/esp32.custom.nuki-bridge.master.example.yaml-->
```
:::

## Firmware Alternatives

### Doorman <Badge type="tip" text="Arduino Framework" />
If you're looking for a firmware that's more **customizable through code**, there's a great alternative developed by [@peteh](https://github.com/peteh). Check it out on [GitHub](https://github.com/peteh/doorman).

::: tip
This version is ideal for developers or advanced users who want full control over the logic and behavior of their Doorman setup.
:::
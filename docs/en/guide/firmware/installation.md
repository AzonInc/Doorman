---
description: Schritt-für-Schritt-Anleitung zur Installation oder Aktualisierung der Doorman-Firmware, inklusive Web Serial, Web-Oberfläche und Home Assistant Integration.
---

<script setup>
import { ref, watch, nextTick } from 'vue';
import { inBrowser } from 'vitepress'
import pkg from '../../../package.json';
</script>
<script>
export default {
    data() {
        return {
            test: '',
            baseUrl: '../../firmware/release/',
            platform: '',
            extension: '',
            integration: '',
            variant: '',
            platform_options: [
                {
                    key: 'doorman-s3-rev2',
                    name: 'Doorman S3 <span class="VPBadge tip">2.x.x</span>',
                    icon: '',
                    iconColor: '',
                    details: 'Recommended for the <b>Doorman S3</b> revision <code>2.0.0</code> or later with the extension board connector.',
                    extensions: [
                        {
                            key: 'none',
                            name: 'No Extension',
                            icon: '',
                            iconColor: '',
                            details: 'Only the Doorman Core Board.',
                        },
                        {
                            key: 'audio',
                            name: 'Audio Extension',
                            icon: '',
                            iconColor: '',
                            details: 'I have an Audio Extension Board installed.',
                        }
                    ]
                },
                {
                    key: 'doorman-s3',
                    name: 'Doorman S3 <span class="VPBadge tip">1.x.x</span>',
                    icon: '',
                    iconColor: '',
                    details: 'Recommended for the <b>Doorman S3</b> revison <code>1.x.x</code> except <code>1.4.0</code>.',
                    extensions: null
                },
                {
                    key: 'doorman-s3-quad',
                    name: 'Doorman S3 <span class="VPBadge tip">1.4.0</span>',
                    icon: '',
                    iconColor: '',
                    details: 'Recommended for <b>Doorman S3</b> revision <code>1.4.0</code> only.',
                    extensions: null
                },
                {
                    key: 'esp32-s3',
                    name: 'ESP32-S3 <span class="VPBadge tip">Octal</span>',
                    icon: '',
                    iconColor: '',
                    details: 'Recommended for all ESP32-S3 boards with at least 8&nbsp;MB PSRAM.',
                    extensions: null
                },
                {
                    key: 'esp32-s3-quad',
                    name: 'ESP32-S3 <span class="VPBadge tip">Quad</span>',
                    icon: '',
                    iconColor: '',
                    details: 'Recommended for all ESP32-S3 boards with up to 4&nbsp;MB PSRAM.',
                    extensions: null
                },
                {
                    key: 'esp32',
                    name: 'ESP32',
                    icon: '',
                    iconColor: '',
                    details: 'Recommended for <b>all ESP32 boards (no variant) without PSRAM</b>.',
                    extensions: null
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
                    key: 'standard',
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
                }
            ],
        }
    },
    watch: {
        platform(newPlatform, oldPlatform) {
            localStorage.setItem("fw_platform", newPlatform);

            const selectedPlatform = this.platform_options.find(p => p.key === newPlatform);

            if (!selectedPlatform?.extensions) {
                this.extension = 'none';
            } else if (!selectedPlatform.extensions.find(e => e.key === this.extension)) {
                this.extension = 'none';
            }
        },
        integration(newIntegration, oldIntegration) {
            localStorage.setItem("fw_integration", newIntegration);
            if (!this.is_variant_allowed(this.variant)) {
                this.variant = 'standard';
            }
        },
        variant(newVariant, oldVariant) {
            localStorage.setItem("fw_variant", newVariant);
        },
        valid_manifest(newVariant, oldVariant) {
            this.applyWebInstallOverrides();
        },
        manifest_file(newVariant, oldVariant) {
            this.applyWebInstallOverrides();
        }
    },
    computed: {
        selected_platform() {
            return this.platform_options.find(p => p.key === this.platform);
        },
        platform_extensions() {
            return this.selected_platform?.extensions || null;
        },
        platform_string() {
            if (!this.extension || this.extension === 'none') {
                return this.platform;
            }

            return `${this.platform}-${this.extension}`;
        },
        fw_string() {
            return [
                this.platform_string,
                this.integration,
                this.variant
            ].join('.');
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
    methods: {
        async applyWebInstallOverrides() {
            await this.$nextTick();

            const button = this.$refs.webInstallBtn;
            button.overrides = {
                checkSameFirmware(manifest, improvInfo) {
                    const manifestFirmware = manifest?.name?.toLowerCase() ?? '';
                    const deviceFirmware = improvInfo?.firmware?.toLowerCase() ?? '';
                    const firmwareName = deviceFirmware.split('.')[1];

                    return manifestFirmware.includes(firmwareName);
                }
            };
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

# Install or Update Firmware <Badge type="tip" :text="pkg.version" />

On this page, you'll find all available methods to install or update the Doorman firmware. Whether you're flashing the firmware for the first time or applying the latest update, the instructions here will guide you step-by-step — including direct installation via Web Serial, updates through the firmware web interface, and integration with Home Assistant's update system.

::: tip IMPORTANT
Be aware that some **updates may introduce breaking changes** — always review the [changelogs](../../changelog/firmware.md) carefully to ensure your system functions reliably in critical situations.
:::

## Web Serial <Badge type="tip" text="Latest release build, no customization" />
Connect your **Doorman S3** via USB-C and use the configuration assistant below to install the latest firmware directly via Web Serial — perfect for first-time setup.

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
    <div v-if="platform_extensions">
        <h5 class="firmware_title_row"><icon-mdi-package-variant-plus /> Do you have an extension board?</h5>
        <div class="firmware_option_row">
            <label class="firmware_option" v-for="fw_extension in platform_extensions" :key="fw_extension.key">
                <input type="radio" class="reset_default" v-model="extension" :value="fw_extension.key">
                <span class="checkmark">
                    <div class="icon" v-if="fw_extension.icon" v-html="fw_extension.icon"></div>
                    <div class="title" v-html="fw_extension.name"></div>
                    <div class="details" v-html="fw_extension.details"></div>
                </span>
            </label>
        </div>
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
                    <p>Your Browser doesn't support Web Serial :( Please use a Chromium-based browser instead.</p>
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

To update, simply click the `Update` button in the firmware's web interface or use the Update list in Home Assistant.

## ESPHome CLI / Dashboard <Badge type="warning" text="Full customization" />
If you want full customization and the ability to add your own sensors, automations, or features, this is your way to go — ideal for power users who prefer complete control over their setup.

### ESPHome Dashboard
You can adopt Doorman to your [ESPHome Dashboard](https://my.home-assistant.io/redirect/supervisor_ingress/?addon=5c53de3b_esphome) and flash a customized firmware variant.

::: warning RESTRICTION
The Dashboard import does only work if you have flashed the `Home Assistant` Smart Home integration firmware.
:::

The adopted configuration could look like this:
```yaml
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2.ha.standard.master.yaml-->
```

### ESPHome CLI
If you choose this option, you likely know what you're doing — just run the following command with your YAML configuration file.

If you prefer using the MQTT-only firmware, simply apply the provided MQTT example YAML tailored for your board below.

```sh
esphome run <yamlfile.yaml>
```

## Board Configuration Files

### Default GPIO configuration
| Component      | Doorman 2.x.x | Doorman 1.x.x | ESP32-S3 | ESP32    |
| -------------- | :-----------: | :-----------: | :------: | :------: |
| Status LED     | GPIO 1        | GPIO 1        | GPIO 1   | GPIO 2   |
| RGB Status LED | GPIO 2        | GPIO 2        | GPIO 2   | GPIO 4   |
| SIEDLE ERT     | GPIO 4        | /             | /        | /        |
| SIEDLE:IHB TX Carrier | GPIO 5        | /             | /        | /        |
| SIEDLE:IHB TX Data    | GPIO 6        | /             | /        | /        |
| TC:BUS TX      | GPIO 8        | GPIO 8        | GPIO 8   | GPIO 23  |
| TC:BUS RX      | GPIO 9        | GPIO 9        | GPIO 9   | GPIO 22  |
| ADC Input      | GPIO 10       | GPIO 10       | GPIO 10  | GPIO 36  |
| SIEDLE:IHB RX Data | GPIO 11        | /             | /        | /        |
| Relay          | GPIO 42       | GPIO 42       | GPIO 42  | GPIO 21  |
| External Button| GPIO 40       | GPIO 41       | GPIO 41  | GPIO 20  |


### Standard Firmware
::: details Doorman S3 (2.0+)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2.ha.standard.master.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2.mqtt.standard.master.yaml-->
```
```yaml [HomeKit]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2.homekit.standard.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2.custom.standard.master.yaml-->
```
:::

::: details Doorman S3 (2.0+) + Audio Extension Board
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2-audio.ha.standard.master.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2-audio.mqtt.standard.master.yaml-->
```
```yaml [HomeKit]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2-audio.homekit.standard.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2-audio.custom.standard.master.yaml-->
```
:::

::: details Doorman S3 (1.5+)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/doorman-s3.ha.standard.master.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/configurations/doorman-s3.mqtt.standard.master.yaml-->
```
```yaml [HomeKit]
<!--@include: ../../../../firmware/configurations/doorman-s3.homekit.standard.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/doorman-s3.custom.standard.master.yaml-->
```
:::

::: details Doorman S3 (1.4)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/doorman-s3-quad.ha.standard.master.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/configurations/doorman-s3-quad.mqtt.standard.master.yaml-->
```
```yaml [HomeKit]
<!--@include: ../../../../firmware/configurations/doorman-s3-quad.homekit.standard.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/doorman-s3-quad.custom.standard.master.yaml-->
```
:::

::: details ESP32-S3 (Octal PSRAM)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/esp32-s3.ha.standard.master.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/configurations/esp32-s3.mqtt.standard.master.yaml-->
```
```yaml [HomeKit]
<!--@include: ../../../../firmware/configurations/esp32-s3.homekit.standard.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/esp32-s3.custom.standard.master.yaml-->
```
:::

::: details ESP32-S3 (Quad PSRAM)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/esp32-s3-quad.ha.standard.master.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/configurations/esp32-s3-quad.mqtt.standard.master.yaml-->
```
```yaml [HomeKit]
<!--@include: ../../../../firmware/configurations/esp32-s3-quad.homekit.standard.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/esp32-s3-quad.custom.standard.master.yaml-->
```
:::

::: details Generic ESP32
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/esp32.ha.standard.master.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/configurations/esp32.mqtt.standard.master.yaml-->
```
```yaml [HomeKit]
<!--@include: ../../../../firmware/configurations/esp32.homekit.standard.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/esp32.custom.standard.master.yaml-->
```
:::

### Nuki Bridge Firmware
::: details Doorman S3 (2.0+)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2.ha.nuki-bridge.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2.custom.nuki-bridge.master.yaml-->
```
:::

::: details Doorman S3 (2.0+) + Audio Extension Board
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2-audio.ha.nuki-bridge.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/doorman-s3-rev2-audio.custom.nuki-bridge.master.yaml-->
```
:::

::: details Doorman S3 (1.5+)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/doorman-s3.ha.nuki-bridge.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/doorman-s3.custom.nuki-bridge.master.yaml-->
```
:::

::: details Doorman S3 (1.4)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/doorman-s3-quad.ha.nuki-bridge.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/doorman-s3-quad.custom.nuki-bridge.master.yaml-->
```
:::

::: details ESP32-S3 (Octal PSRAM)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/esp32-s3.ha.nuki-bridge.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/esp32-s3.custom.nuki-bridge.master.yaml-->
```
:::

::: details ESP32-S3 (Quad PSRAM)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/esp32-s3-quad.ha.nuki-bridge.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/esp32-s3-quad.custom.nuki-bridge.master.yaml-->
```
:::

::: details Generic ESP32
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/configurations/esp32.ha.nuki-bridge.master.yaml-->
```
```yaml [Custom]
<!--@include: ../../../../firmware/configurations/esp32.custom.nuki-bridge.master.yaml-->
```
:::

## Experimental Firmware

You can switch to the development branch by enabling the `Experimental Firmware` option in the Doorman web interface or Home Assistant to access the latest features and fixes.

::: tip HELP WANTED
Please report any issues you encounter - your feedback helps us improve stability and deliver a reliable firmware in the future.
:::

## Firmware Alternatives

### Doorman <Badge type="tip" text="Arduino Framework" />
If you're looking for a firmware that's more **customizable through code**, there's a great alternative developed by [@peteh](https://github.com/peteh). Check it out on [GitHub](https://github.com/peteh/doorman).

::: tip
This version is ideal for developers or advanced users who want full control over the logic and behavior of their Doorman setup.
:::
<script setup>
import { inBrowser } from 'vitepress'
</script>
<script>
export default {
    data() {
        return {
            baseUrl: '../../../firmware/release/',
            platform: '',
            integration: '',
            variant: '',
            platform_options: [
                {
                    key: 'esp32-s3',
                    name: 'ESP32-S3 <span class="VPBadge tip">Octal</span>',
                    icon: '',
                    iconColor: '',
                    details: 'Empfohlen für den <b>Doorman-S3</b> sowie ESP32-S3 Boards mit mindestens 8&nbsp;MB PSRAM.',
                },
                {
                    key: 'esp32-s3-quad',
                    name: 'ESP32-S3 <span class="VPBadge tip">Quad</span>',
                    icon: '',
                    iconColor: '',
                    details: 'Empfohlen für den <b>Doorman-S3 (Revision 1.4)</b> sowie ESP32-S3 Boards mit bis zu 4&nbsp;MB PSRAM.',
                },
                {
                    key: 'esp32',
                    name: 'ESP32',
                    icon: '',
                    iconColor: '',
                    details: 'Empfohlen für <b>alle ESP32-Boards ohne PSRAM</b>.',
                },
            ],
            integration_options: [
                {
                    key: 'ha',
                    name: 'Home Assistant',
                    icon: '<img src="/icons/home-assistant.svg" />',
                    iconColor: '',
                    details: 'Ich nutze Home Assistant und will Doorman möglichst einfach in mein Smart Home einbinden.',
                },
                {
                    key: 'mqtt',
                    name: 'MQTT',
                    icon: '<img src="/icons/mqtt-ver-transp.svg" />',
                    iconColor: '#606',
                    details: 'Ich möchte Doorman direkt mit meinem MQTT-Broker verbinden und auf anderen Plattformen nutzen.',
                },
                {
                    key: 'homekit',
                    name: 'HomeKit',
                    icon: '<img src="/icons/homekit.png" />',
                    iconColor: '',
                    details: 'Ich verwende Apple Home und möchte Doorman einfach und bequem über HomeKit einbinden.',
                },
                {
                    key: 'custom',
                    name: 'Standalone',
                    icon: '<img src="/icons/fluent-emoji-sparkles.png" />',
                    iconColor: '',
                    details: 'Ich nutze kein Smart Home – Doorman soll einfach eigenständig über WLAN funktionieren.',
                },
            ],
            variant_options: [
                {
                    key: 'stock',
                    name: 'Standard',
                    icon: '',
                    iconColor: '',
                    details: 'Ich brauche nur die Grundfunktionen, um meine Gegensprechanlage zu steuern – mehr nicht.',
                },
                {
                    key: 'nuki-bridge',
                    name: 'Nuki Bridge',
                    icon: '',
                    iconColor: '',
                    details: 'Ich will zusätzlich mein Nuki Smart Lock steuern und Ring To Open auch über die Wohnungsklingel nutzen.',
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
                return 'Die Firmware benötigt nach dem flashen noch etwas Handarbeit. Schau dir dazu bitte die Anleitung zur <a href="./mqtt#setup">MQTT Einrichtung</a> an.';
            }
            else if(this.integration == 'homekit') {
                return 'Schau dir nach dem flashen bitte die Anleitung zur <a href="./homekit#koppeln">HomeKit Kopplung</a> an.';
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
            return `Diese Option lässt sich wegen technischer Einschränkungen leider nicht mit <b>${integrationName}</b> kombinieren.`;
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

# Installation und Updates

Auf dieser Seite findest du alle verfügbaren Methoden, um die Doorman-Firmware zu installieren oder zu aktualisieren. Egal, ob du die Firmware zum ersten Mal flashst oder ein Update einspielst – die Anleitungen führen dich Schritt für Schritt durch den Prozess. Dazu gehören die direkte Installation per Web Serial, Updates über die Web-Oberfläche der Firmware und die Integration in das Update-System von Home Assistant.

::: tip WICHTIG
Manche **Updates können Breaking Changes mit sich bringen** — lies dir die Changelogs immer genau durch, damit dein System auch in wichtigen Situationen zuverlässig funktioniert.  
:::

## Web Serial <Badge type="tip" text="Neueste Release-Version, keine Anpassungen" />
Verbinde deinen **Doorman-S3** per USB-C und nutze den Konfigurationsassistenten unten, um die neueste Firmware direkt über Web Serial zu installieren — ideal für die erstmalige Einrichtung.

Dieser geführte Prozess sorgt für eine nahtlose Integration mit der Home Assistant API und ermöglicht es dir, zukünftige Community-Updates ganz einfach zu übernehmen.

<div v-if="web_serial_available">
    <h5 class="firmware_title_row"><icon-ph-cpu-bold /> Welche Hardware nutzt du?</h5>
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
        <h5 class="firmware_title_row"><icon-ic-round-other-houses /> Nutzt du irgendein Smart Home System?</h5>
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
        <h5 class="firmware_title_row"><icon-mdi-package-variant-plus /> Darf's vielleicht etwas mehr sein?</h5>
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
        <h5 class="firmware_title_row"><icon-line-md-uploading-loop /> Jetzt wird geflasht – und zwar mit Stil!</h5>
        <div v-if="notes" class="note custom-block">
            <p class="custom-block-title">HINWEIS</p>
            <p v-html="notes"></p>
        </div>
        <esp-web-install-button :manifest="manifest_file">
            <button slot="activate">
                <div class="custom-layout">
                    <a class="btn">Firmware installieren oder updaten</a>
                </div>
            </button>
            <div slot="unsupported">
                <div class="danger custom-block">
                    <p class="custom-block-title">OH NEIN!</p>
                    <p>Dein Browser unterstützt leider kein Web Serial :(</p>
                </div>
            </div>
            <div slot="not-allowed">
                <div class="danger custom-block">
                    <p class="custom-block-title">OH NEIN!</p>
                    <p>Das funktioniert mit HTTP nicht :(</p>
                </div>
            </div>
        </esp-web-install-button>
    </div>
</div>
<div v-else>
    <div class="danger custom-block">
        <p class="custom-block-title">COMPUTER SAGT NEIN</p>
        <p>Da dein Browser kein Web Serial unterstützt, ist der Konfigurations Assistent leider nicht verfügbar! Bitte verwende einen Chromium-basierten Browser.</p>
    </div>
</div>

## Integrierte OTA-Updates <Badge type="tip" text="Neueste Release-Version, keine Anpassungen" />  
Die Firmware unterstützt direkt OTA-Updates – du kannst die neueste Version mit nur einem Klick installieren – schnell, einfach und stressfrei.

Wenn Doorman in Home Assistant integriert ist, erscheinen verfügbare Updates sogar direkt in deiner Update-Liste – so bleibst du noch bequemer auf dem neuesten Stand.

Zum Aktualisieren klickst du einfach auf den Button `Install Update` im Web-Interface der Firmware oder nutzt die Update-Liste in Home Assistant.

## ESPHome CLI / Dashboard <Badge type="warning" text="Vollständige Anpassung" />
Wenn du volle Kontrolle möchtest und eigene Sensoren, Automationen oder Funktionen hinzufügen willst, ist das der richtige Weg für dich — ideal für Power-User, die ihre Einrichtung komplett selbst gestalten wollen.

### ESPHome Dashboard
Du kannst Doorman in dein [ESPHome-Dashboard](https://my.home-assistant.io/redirect/supervisor_ingress/?addon=5c53de3b_esphome) übernehmen und eine individuell angepasste Firmware flashen.

Die übernommene Konfiguration könnte zum Beispiel so aussehen:
```yaml
<!--@include: ../firmware/minimal.example.yaml-->
```

### ESPHome CLI
Wenn du diese Option wählst, weißt du wahrscheinlich, was du tust – führe einfach den folgenden Befehl mit deiner YAML-Konfigurationsdatei aus.

Falls du lieber die reine MQTT-Firmware verwenden möchtest, kannst du einfach das unten bereitgestellte MQTT-Beispiel-YAML für dein Board übernehmen.

```sh
esphome run <yamlfile.yaml>
```

## Board Konfigurationsdateien

### Standard GPIO Belegung
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

## Firmware Alternativen

### Doorman <Badge type="tip" text="Arduino Framework" />
Wenn du nach einer Firmware suchst, bei der du wirklich alles anpassen kannst wie du es magst, gibt es eine coole Alternative von [@peteh](https://github.com/peteh). Schau sie dir auf [GitHub](https://github.com/peteh/doorman) an.

::: tip
Diese Version eignet sich besonders für Entwickler oder fortgeschrittene Nutzer, die die Logik und das Verhalten ihres Doorman-Setups vollständig kontrollieren möchten.
:::

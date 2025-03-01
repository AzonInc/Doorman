# Stock Firmware <Badge type="tip" text="ESP-IDF Framework" />

Dies ist die bevorzugte Firmware, da sie direkt mit dem Doorman S3 funktioniert und Improv über Bluetooth LE unterstützt.

## Firmware installieren oder aktualisieren

Es gibt verschiedene Möglichkeiten, die Firmware zu aktualisieren.

::: warning WARNUNG
Manche **Updates können Breaking Changes mit sich bringen** — lies dir die Changelogs immer genau durch, damit dein System auch in wichtigen Situationen zuverlässig funktioniert.  
:::

#### Web Serial <Badge type="tip" text="Neueste Release-Version, keine Anpassungen" />
Verbinde deinen **Doorman-S3** per USB-C und klicke auf den Button unten, um die neueste Firmware direkt über Web Serial zu installieren oder zu aktualisieren. So kannst du das Gerät nahtlos mit der Home Assistant API nutzen und zukünftig einfach Community-Updates einspielen.

<esp-web-install-button manifest="../../../firmware/release/esp32-s3.ha.stock/manifest.json">
    <button slot="activate">
        <div class="custom-layout">
            <a class="btn">Firmware (Doorman-S3) installieren oder updaten</a>
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

#### HTTP OTA-Updates <Badge type="tip" text="Neueste Release-Version, keine Anpassungen" />  
Wenn du Doorman in Home Assistant hinzugefügt hast, kannst du von automatisierten Firmware-Builds und einfachen Updates profitieren. Das Update erscheint direkt in deiner Update-Liste – ein Klick auf den Update-Button genügt, und das OTA-Update startet sofort. Einfach, effizient und stressfrei.

#### ESPHome CLI / Dashboard <Badge type="warning" text="Vollständige Anpassung" />  
Für maximale Kontrolle und individuelle Anpassungen kannst du die Firmware in dein ESPHome Dashboard übernehmen oder das ESPHome CLI nutzen, indem du folgenden Befehl ausführst:  
`esphome run <yamlfile.yaml>`  

Falls du lieber die MQTT-Only-Firmware verwenden möchtest, nutze einfach das passende MQTT-Beispiel-YAML für dein Board unten.

##### Board Konfigurationsoptionen:

::: details Doorman-S3 (1.5) / ESP32-S3 (Octal PSRAM)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32-s3.ha.stock.master.example.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/examples/esp32-s3.mqtt.stock.master.example.yaml-->
```
:::

::: details Doorman-S3 (1.4) / ESP32-S3 (Quad PSRAM)
::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32-s3-quad.ha.stock.master.example.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/examples/esp32-s3-quad.mqtt.stock.master.example.yaml-->
```
:::

::: details Generic ESP32
#### GPIO Konfiguration
- **TC:BUS RX:** GPIO 22
- **TC:BUS TX:** GPIO 23
- **Status LED:** GPIO 2
- **RGB Status LED:** GPIO 4
- **Relay:** GPIO 21
- **External button:** GPIO 20
- **ADC Input:** GPIO 36

::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp32.ha.stock.master.example.yaml-->
```
```yaml [MQTT]
<!--@include: ../../../../firmware/examples/esp32.mqtt.stock.master.example.yaml-->
```
:::

::: details Generic ESP8266
#### GPIO Konfiguration
- **TC:BUS RX:** GPIO 12
- **TC:BUS TX:** GPIO 13
- **Status LED:** GPIO 2
- **RGB Status LED:** Dummy
- **Relay:** GPIO 14
- **External button:** GPIO 15
- **ADC Input:** A0

::: code-group
```yaml [Home Assistant]
<!--@include: ../../../../firmware/examples/esp8266.ha.stock.master.example.yaml-->  
```
```yaml [MQTT]  
<!--@include: ../../../../firmware/examples/esp8266.mqtt.stock.master.example.yaml-->  
```
:::

<!--@include: ./additions.md-->

<!--@include: ./mqtt.md-->
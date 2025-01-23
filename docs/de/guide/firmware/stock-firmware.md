# Stock Firmware <Badge type="tip" text="ESP-IDF Framework" />

Dies ist die bevorzugte Firmware, da sie direkt mit dem Doorman S3 funktioniert und Improv über Bluetooth LE unterstützt.

### Firmware Update
Es gibt mehrere Möglichkeiten, die Firmware zu aktualisieren:
- ESPHome OTA <Badge type="warning" text="Erfordert ESPHome-Dashboard" />
- HTTP OTA <Badge type="tip" text="Neueste Release-Version, keine Anpassung" />
- Web Serial <Badge type="tip" text="Neueste Release-Version, keine Anpassung" />

Du kannst deinen Doorman über USB-C anschließen und auf den untenstehenden Button klicken, um die neueste Doorman Stock Firmware (Home Assistant) direkt über Web Serial zu installieren.
<esp-web-install-button manifest="../../../firmware/release/doorman-stock/manifest.json">
    <button slot="activate">
        <div class="custom-layout">
            <a class="btn">Firmware (Home Assistant) installieren oder updaten</a>
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

## Firmware YAML für Home Assistant

Dies ist die minimale ESPHome-Konfigurations-YAML-Datei für die Verwendung mit Home Assistant. Vergiss nicht, den API-Schlüssel zu aktualisieren.

::: details Minimale Stock Firmware (Home Assistant)
```yaml
<!--@include: ../../../../firmware/examples/ha-stock.example.yaml-->
```
:::

## Firmware YAML für MQTT

Dies ist die minimale ESPHome-Konfigurations-YAML-Datei für die Verwendung mit MQTT. Vergiss nicht, die MQTT Broker Daten zu aktualisieren.

::: details Minimale Stock Firmware (MQTT)
```yaml
<!--@include: ../../../../firmware/examples/mqtt-stock.example.yaml-->
```
:::

<!--@include: ./additions.md-->
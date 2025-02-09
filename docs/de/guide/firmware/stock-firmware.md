# Stock Firmware <Badge type="tip" text="ESP-IDF Framework" />

Dies ist die bevorzugte Firmware, da sie direkt mit dem Doorman S3 funktioniert und Improv über Bluetooth LE unterstützt.

### Firmware Update
Es gibt mehrere Möglichkeiten, die Firmware zu aktualisieren:
- ESPHome OTA <Badge type="warning" text="Erfordert ESPHome-Dashboard, volle Kontrolle" />
- HTTP OTA <Badge type="tip" text="Neueste Release-Version, keine Anpassung" />
- Web Serial <Badge type="tip" text="Neueste Release-Version, keine Anpassung" />

Schließ deinen Doorman per USB-C an und klick unten auf den Button, um die neueste Doorman Stock Firmware (Home Assistant) direkt über Web Serial zu installieren. Damit kannst du später problemlos Updates aus der Community installieren.
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

::: warning WARNUNG
Manche **Updates können Änderungen mit sich bringen, die Sachen kaputtmachen** — check also immer die Changelogs, damit dein System in wichtigen Situationen zuverlässig läuft.
:::

Um die Firmware vollständig anzupassen und die volle Kontrolle zu übernehmen, kannst du sie in dein ESPHome Dashboard integrieren. Falls du die MQTT-Firmware nutzen möchtest, wende einfach das unten stehende `Minimale Stock Firmware (MQTT)` Beispiel-YAML an.

## Firmware YAML

Dies ist die minimale ESPHome-Konfigurations-YAML-Datei für die Verwendung mit Home Assistant. Vergiss nicht, den API-Schlüssel zu aktualisieren.
::: details Minimale Stock Firmware (Home Assistant)
```yaml
<!--@include: ../../../../firmware/examples/ha-stock.example.yaml-->
```
:::

Dies ist die minimale ESPHome-Konfigurations-YAML-Datei für die Verwendung mit MQTT. Vergiss nicht, die MQTT Broker Daten zu aktualisieren.
::: details Minimale Stock Firmware (MQTT)
```yaml
<!--@include: ../../../../firmware/examples/mqtt-stock.example.yaml-->
```
:::

<!--@include: ./additions.md-->

<!--@include: ./mqtt.md-->
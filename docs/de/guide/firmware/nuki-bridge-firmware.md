# Nuki Bridge Firmware <Badge type="tip" text="Arduino Framework" />

Diese Konfiguration übernimmt alle Funktionen der Stock Firmware und fügt eine Nuki Bridge-Komponente über BLE hinzu. Beachte, dass Improv über Bluetooth LE nicht unterstützt wird, da der BLE-Stack von der Nuki Bridge-Komponente verwendet wird.

### Firmware Update
Es gibt mehrere Möglichkeiten, die Firmware zu aktualisieren:
- ESPHome OTA <Badge type="warning" text="Erfordert ESPHome-Dashboard" />
- HTTP OTA <Badge type="tip" text="Neueste Release-Version, keine Anpassung" />
- Web Serial <Badge type="tip" text="Neueste Release-Version, keine Anpassung" />

Du kannst deinen Doorman über USB-C anschließen und auf den untenstehenden Button klicken, um die neueste Doorman Nuki-Bridge Firmware (Home Assistant) direkt über Web Serial zu installieren.
Wenn du MQTT ohne Home Assistant nutzen möchtest, kannst du Doorman in dein ESPHome Dashboard aufnehmen und das untenstehende Beispiel `Minimale Nuki-Bridge Firmware (MQTT)` verwenden.
<esp-web-install-button manifest="../../../firmware/release/doorman-nuki-bridge/manifest.json">
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

## Firmware YAML

Dies ist die minimale ESPHome-Konfigurations-YAML-Datei für die Verwendung mit Home Assistant. Vergiss nicht, den API-Schlüssel zu aktualisieren.
::: details Minimale Nuki-Bridge Firmware (Home Assistant)
```yaml
<!--@include: ../../../../firmware/examples/ha-nuki-bridge.example.yaml-->
```
:::

Dies ist die minimale ESPHome-Konfigurations-YAML-Datei für die Verwendung mit MQTT. Vergiss nicht, die MQTT Broker Daten zu aktualisieren.
::: details Minimale Nuki-Bridge Firmware (MQTT)
```yaml
<!--@include: ../../../../firmware/examples/mqtt-nuki-bridge.example.yaml-->
```
:::

## Nuki Lock koppeln

Je nach PCB-Revision drücke entweder die FLASH- oder PRG-Taste auf der Doorman-Platine für 5 Sekunden, bis die Status-LED lila zu blinken beginnt, oder aktiviere den Nuki Pairing Mode-Schalter in Home Assistant. Drücke dann die Taste an deinem Nuki Lock für 5 Sekunden, bis der Leuchtring aufleuchtet.

Nach erfolgreicher Kopplung bleibt die Status-LED für 3 Sekunden konstant an und schaltet sich dann aus.

::: warning
Wenn dein Nuki Lock bereits gekoppelt ist, wird es durch diesen Vorgang entkoppelt!
:::

## Nuki Lock entkoppeln

Du kannst dein Gerät entweder über den `Nuki Unpair Device`-Button in Home Assistant oder die physische `FLASH`- oder `PRG`-Taste auf der Doorman-Platine entkoppeln.

::: info Physische Taste
Falls dein Schloss bereits mit Doorman gekoppelt ist, drücke die `FLASH`- oder `PRG`-Taste auf der Doorman-Platine für 5 Sekunden, bis die RGB-Status-LED lila zu blinken beginnt. Dein Nuki Lock wird dann entkoppelt. Beachte, dass der Pairing-Modus nach 30 Sekunden abläuft.
:::

<!--@include: ./additions.md-->
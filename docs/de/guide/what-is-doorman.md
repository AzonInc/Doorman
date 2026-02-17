# Was ist Doorman?

Doorman ermöglicht es dir, deine [TCS](https://www.tcsag.de/) oder [Koch](https://www.kochag.ch/) Gegensprechanlage mit jedem Heimautomatisierungssystem zu verbinden. Zusätzlich sind einige umgebrandete [Niko](https://www.niko.eu/), [Scantron](https://scantron.dk/) und ältere [Jung](https://www.jung-group.com/) Gegensprechanlagen Gegenstücke zu den TCS- und Koch-Modellen.

<div style="display: flex; flex-direction: row; flex-wrap: wrap; justify-content: space-between; gap: 20px;align-items: center;" markdown>
   <div style="min-width:260px;flex: 1 1 260px;" markdown>

### Was ist mit anderen Gegensprechanlagen?
Doorman ist nicht auf TCS- oder Koch-Gegensprechanlagen beschränkt. Du kannst ihn auch verwenden, um einen Tastendruck zu simulieren und damit einen Türöffner oder Licht mit dem integrierten Solid-State-Relais (bis zu 40V, max. 2,5A) auszulösen.

Wenn deine Gegensprechanlage auf einem 2-Draht-Bus im Bereich von 14-24V DC arbeitet, könnte es möglich sein, auch andere Protokolle zu implementieren.

Alternativ kannst du den Spannungspegel auf älteren Gegensprechanlagen (14-24V DC) mithilfe des internen ADC überwachen und spezifische Aktionen basierend auf den Messwerten auslösen.

   </div>
   <div style="min-width: 185px;max-width:200px;flex: 1 1 185px;" markdown>
        <img style="" src="/enclosure-base-board.png">
   </div>
</div>

<div class="tip custom-block" style="padding-top: 8px">

Du hast gerade deinen Doorman erhalten? Geh direkt zur [Quickstart](getting-started) Anleitung.

</div>

## Features

- ESP32-S3 mit Dual-Core-CPU, integriertem PSRAM und Bluetooth 
- USB-C Anschluss zum einfachen Flashen (und weil USB-C einfach cool ist 😎)
- Kompaktes Design, das fast überall passt
- Einstellbare WS2812B RGB LED zur Anzeige bestimmter Ereignisse
- Zwei zusätzliche GPIOs zur einfachen Erweiterung mit eigenen Sensoren und Geräten (z.B. I²C)
- Solid-State-Relais zum Schalten von Lasten bis zu 40V (max. 2,5A), ideal für ältere Gegensprechanlagen
- Externer Button (G + BTN)
- Brückengleichrichter zur Korrektur der Bus-Leitung-Polarität
- TC:BUS-Kommunikation für Funktionen wie das Öffnen der Haustür und das Erkennen der Klingel
- Kann als Nuki Bridge mit dem [Nuki Bridge Addon](firmware/nuki-bridge) verwendet werden.

## Möchtest du auch einen?

Wenn du an einem Doorman S3 interessiert bist, nutze einfach den Button unten, um eine **unverbindliche Anfrage** zu senden.

Wenn du Fragen hast oder direkten Kontakt wünschst, kannst du mich gerne auf [Discord](https://discord.gg/t2d34dvmBf) oder per [E-Mail](mailto:flo@azon.ai?subject=Doorman) anschreiben.

<div class="custom-layout">
    <a class="btn" href="../order">Anfrage senden</a>
</div>
Nachdem deinen Doorman mit dem WLAN verbunden ist, blinkt die LED langsam blau. Das bedeutet, er wartet darauf, dass sich Home Assistant verbindet.

<div style="display: flex; flex-direction: row; flex-wrap: wrap; justify-content: space-between; gap: 20px;align-items: center;" markdown>
   <div style="min-width:260px;flex: 1 1 260px;" markdown>

   Dein Doorman sollte automatisch in Home Assistant erscheinen. Falls nicht, blockiert dein Netzwerk möglicherweise mDNS (Multicast DNS), das für die automatische Geräteerkennung notwendig ist.

   1. Stelle sicher, dass deine **Home Assistant**-Installation auf dem neuesten Stand ist, um die volle Kompatibilität mit deinem Doorman-Gerät zu gewährleisten.

   2. Öffne die **Home Assistant**-App auf deinem mobilen Gerät und gehe zu **Einstellungen → Geräte & Dienste**.

   3. Suche nach einem neuen Gerät mit der Bezeichnung **`Doorman S3` (ESPHome)** und tippe auf **Hinzufügen**.

   4. Nach erfolgreicher Verbindung leuchtet die LED für 3 Sekunden durchgehend blau. Dein **Doorman** ist jetzt in **Home Assistant** eingebunden und einsatzbereit.

   </div>
   <div style="min-width: 185px;max-width:200px;flex: 1 1 185px;" markdown>
      <img style="" src="/de/guide/images/discovery_ha.png">
   </div>
</div>
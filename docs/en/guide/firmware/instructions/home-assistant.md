After you connect your Doorman to your Wi-Fi network, it will start blinking slowly in blue. This means it's waiting for Home Assistant to connect.

<div style="display: flex; flex-direction: row; flex-wrap: wrap; justify-content: space-between; gap: 20px;align-items: center;" markdown>
   <div style="min-width:260px;flex: 1 1 260px;" markdown>

   It should appear automatically in Home Assistant. If it doesn't, your network may be blocking mDNS (Multicast DNS), which is required for device discovery.

   1. Make sure your **Home Assistant** instance is **updated** to the latest version to ensure compatibility with your Doorman device.

   2. Open the **Home Assistant** app on your mobile device and navigate to **Settings → Devices & Services**.

   3. Look for a new device labeled **`Doorman S3` (ESPHome)** and tap **Add**.

   4. After a successful connection, it will light up solid blue for 3 seconds. Your **Doorman** is now added to **Home Assistant** and ready to use.

   </div>
   <div style="min-width: 185px;max-width:200px;flex: 1 1 185px;" markdown>
      <img style="" src="/en/guide/images/discovery_ha.png">
   </div>
</div>
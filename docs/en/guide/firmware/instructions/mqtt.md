After you connect your Doorman to your Wi-Fi network, it will **pulse red and blue**. This means the MQTT broker is not yet configured.

1. Open the web interface and go to the `MQTT Configuration` section.

2. Enter your MQTT broker credentials.

   ![broker-credentials](/en/guide/images/mqtt-details.png)

   If you don't see these settings, scroll to the bottom and click "Show all".

3. Save the settings — the firmware will then try to connect to your MQTT broker.

4. If it connects successfully, the status LED will light up **solid blue for 3 seconds** to confirm.
Nachdem du deinen Doorman mit dem WLAN verbunden hast, wird die LED **rot und blau pulsieren**. Das bedeutet, der MQTT-Broker ist noch nicht konfiguriert.

1. Öffne die Weboberfläche deines Doormans und gehe zum Abschnitt **`MQTT-Konfiguration`**.

2. Gib deine MQTT-Broker-Zugangsdaten ein.

   ![broker-credentials](/de/guide/images/mqtt-details.png)

   Wenn du die Einstellungen nicht findest, scrolle ganz nach unten und klicke auf "Alle anzeigen".

3. Speichere die Einstellungen — die Firmware versucht dann, eine Verbindung zu deinem MQTT-Broker herzustellen.

4. Wenn die Verbindung erfolgreich ist, leuchtet die Status-LED zur Bestätigung **3 Sekunden lang durchgehend blau**.
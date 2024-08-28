# Ring To Open <Badge type="warning" text="Party Mode" />

Die Doorman-Firmware bietet eine **Ring To Open**-Automatisierung, die früher als **Party Mode** bekannt war.

### Was macht es?
Wie der Name schon sagt, öffnet sich die Eingangstür automatisch, wenn jemand die Türklingel betätigt. Diese Funktion ist besonders nützlich während einer Party, da Gäste einfach durch Klingeln das Gebäude betreten können.

### Wie benutze ich es?
Du kannst die Ring To Open-Automatisierung entweder über den Schalter in Home Assistant umschalten oder durch Ausführen des `Function Button` Commands.

#### Konfigurationsoptionen:
::: info **Confirmation**
Wenn du diese Einstellung aktivierst, wird deine Wohnungs-Klingel jedes Mal klingeln, wenn du die Ring To Open-Automatisierung per Befehl (Funktionstaste) aktivierst.
:::

::: info **Delay**
Diese Einstellung erlaubt es dir, eine Verzögerung für den `Open Entrance Door Command` zu konfigurieren. Wenn du die Verzögerung auf 60 Sekunden einstellst, wird bei jeder Ausführung der Automatisierung zufällig eine Verzögerung zwischen 5 und 15 Sekunden ausgewählt.
:::

::: info **Timeout**
Du kannst die Ring To Open-Automatisierung nach 5 bis 60 Minuten, nach einem einzigen Klingeln oder nie automatisch deaktivieren.
:::
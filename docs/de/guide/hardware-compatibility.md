# Kann ich Doorman benutzen?

Wenn deine [TCS](https://www.tcsag.de/) oder [Koch](https://www.kochag.ch/) Gegensprechanlagen Innenstation die Anschlussklemmen `a`, `b` und `P` hat und mit 24V DC betrieben wird, ist sie wahrscheinlich kompatibel.

Einige umgelabelte [Niko](https://www.niko.eu/), [Scantron](https://scantron.dk/) und ältere [Jung](https://www.jung-group.com/) Innenstationen, die den TCS- und Koch-Modellen entsprechen, könnten ebenfalls passen.

:::danger WICHTIG - BEVOR DU BESTELLST
Da dieses Produkt durch Reverse Engineering entwickelt wurde, gibt es keine Garantie, dass deine Sprechanlage ohne Tests funktioniert.

1. Überprüfe die Verkabelung und miss die Spannung, bevor du Doorman bestellst.
2. Es gibt keine Garantie, dass Doorman mit deiner Innenstation funktioniert, da die Verkabelung in jedem Gebäude einzigartig ist.
3. Andere Geräte an der `P`-Leitung (z.B. Geräte deiner Nachbarn) könnten zu viel Strom ziehen und Doorman beeinträchtigen.

**Verstehe diese Risiken, bevor du einen bestellst.** Wenn du Fragen hast, kontaktiere mich gerne und ich helfe dir weiter!
:::

## Verkabelung
**2-Draht-Modus:**\
Wenn deine Innenstation im 2-Draht-Modus angeschlossen ist und du keine separate 24V `P`-Leitung hast, verwende etweder die `BUS PWR` Jumper (ab rev. 1.5), eine externe Stromversorgung über USB-C oder die Schraubklemmen `P` (+5V bis +30V DC) und `G` (Ground).

**3-Draht-Modus:**\
Wenn sie im 3-Draht-Modus angeschlossen ist, solltest du in den meisten Fällen die `P`-Leitung zur Stromversorgung vom Doorman nutzen können.

::: tip
Bei einigen Installationen sind die `a`- und `b`-Drähte verdreht, das wird aber von dem integriertem Brückengleichrichter korrigiert.

Berücksichtige dies beim Überprüfen der Spannung auf den Leitungen `a`, `b` und `P`.
:::

#### Erklärung der Innenstation-Anschlussklemmen
- **a:** In der Regel die Bus-Leitung (+24V)
- **b:** Normalerweise Ground
- **P:** Entweder die +24V-Leitung im 3-Draht-Modus oder der Klingelknopf der Wohnung im 2-Draht-Modus. Oftmals gibt es trotzdem eine 24V Leitung irgendwo versteckt.

::: danger VORSICHT
**BENUTZE NICHT DIE +24V `P`-LEITUNG UND DEN USB-C PORT GLEICHZEITIG!**

**SCHLIEßE AUF KEINEN FALL EINE STROMQUELLE AN DIE TC:BUS-TERMINALE AN, SONST KÖNNTE DAS RICHTIG SCHADEN ANRICHTEN!**
:::

## Überprüfe die Stromversorgung
Verwende ein Multimeter, um die DC-Spannung zu messen.\
Miss zuerst die Polarität, da die `a`- und `b`-Leitungen verdreht sein können.

#### Erwartete Spannung
- Zwischen `a` und `b`: ~ 24,0V
- Zwischen `P` und `b`: ~ 23,2V

::: warning
Die Stromversorgung der Gegensprechanlage **muss mindestens 60mA Ausgangsstrom liefern**, um stabil zu funktionieren.

Wenn andere Geräte an der `P`-Leitung zu viel Strom ziehen, reicht es möglicherweise nicht aus, um Doorman zu betreiben.

**Verwende eine externe Stromversorgung (USB-C oder Schraubklemme), wenn die Stromversorgung deiner Sprechanlage weniger als 60mA liefert oder wenn du eine instabile Funktion bemerkst.**
:::

## Bekannte Probleme

Innenstationen mit einem TTC1-B-1 Anschlussmodul verursachen Probleme, da die `a`- und `P`-Anschlüsse verbunden sind.
Ab Hardware Revision `1.5` gibt es eine neue Anschlussmethode, welche nur noch ein leises, aber dennoch wahrnehmbares Rauschen im Lautsprecher erzeugt.

::: danger Problem von älteren Hardware revisionen vor 1.5
Es tritt ein lautes Piepen auf, wenn jemand spricht.
:::

![ttc1-b-1-module](./images/incompatible-ttc1-b-1.png){width=300px}

## Verifizierte Modelle

Schau dir die [unterstützten Modelle und Einstellungen](../../reference/esphome-component.md#model-setting-availability) an.  
Modelle mit verfügbaren Einstellungen sind auf jeden Fall kompatibel. Falls dein Modell nicht dabei ist, erstelle bitte ein Issue, um die Kompatibilität zu überprüfen.
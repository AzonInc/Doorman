### Kann ich Doorman benutzen?

Wenn deine [TCS](https://www.tcsag.de/) oder [Koch](https://www.kochag.ch/) Gegensprechanlage die Anschlussklemmen `a`, `b` und `P` hat und mit 24V DC betrieben wird, ist sie wahrscheinlich kompatibel.

Einige umgelabelte [Niko](https://www.niko.eu/) und [Scantron](https://scantron.dk/) Gegensprechanlagen, die den TCS- und Koch-Modellen entsprechen, könnten ebenfalls passen.

:::danger WICHTIG - BEVOR DU BESTELLST
Da dieses Produkt durch Reverse Engineering entwickelt wurde, gibt es keine Garantie, dass deine Sprechanlage ohne Tests funktioniert.

1. Überprüfe die Verkabelung und miss die Spannung, bevor du Doorman bestellst.
2. Es gibt keine Garantie, dass Doorman mit deiner Gegensprechanlage funktioniert, da die Verkabelung in jedem Gebäude einzigartig ist.
3. Andere Geräte an der `P`-Leitung (z.B. Geräte deiner Nachbarn) könnten zu viel Strom ziehen und Doorman beeinträchtigen.

**Verstehe diese Risiken, bevor du einen bestellst.** Wenn du Fragen hast, kontaktiere mich gerne und ich helfe dir weiter!
:::

## Verkabelung
**2-Draht-Modus:**\
Wenn deine Gegensprechanlage im 2-Draht-Modus angeschlossen ist und du keine separate 24V `P`-Leitung hast, verwende eine externe Stromversorgung über USB-C oder die Schraubklemmen `P` (+5V bis +30V DC) und `G` (Ground).

**3-Draht-Modus:**\
Wenn sie im 3-Draht-Modus angeschlossen ist, solltest du in den meisten Fällen die `P`-Leitung zur Stromversorgung vom Doorman nutzen können.

::: tip
In einigen Installationen sind die `a`- und `b`-Drähte verdreht, aber das wird von Doormans integriertem Brückengleichrichter berücksichtigt. Berücksichtige dies beim Überprüfen der Spannung auf den Leitungen `a`, `b` und `P`.
:::

#### Erklärung der Gegensprechanlagen-Anschlussklemmen
- **a:** Typischerweise die Bus-Leitung (+24V)
- **b:** Typischerweise Ground
- **P:** Die +24V-Leitung im 3-Draht-Modus oder der Klingelknopf der Wohnung im 2-Draht-Modus

::: danger
**NICHT GLEICHZEITIG DIE +24V `P`-LEITUNG UND USB-C VERWENDEN**
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

Einige Gegensprechanlagen mit einem TTC1-B-1 Anschlussmodul verursachen Probleme, da die `a`- und `P`-Anschlüsse irgendwie verbunden sind.
Welche Modelle betroffen sind, ist noch unklar.

::: danger Problem
Es tritt ein lautes Piepen auf, wenn jemand spricht.
:::

![ttc1-b-1-module](./images/incompatible-ttc1-b-1.png){width=300px}

## Verifizierte Modelle

::: info Hinweis
Wenn deine Gegensprechanlage hier nicht aufgeführt ist, bedeutet das nicht unbedingt, dass sie nicht kompatibel ist; es heißt nur, dass sie noch nicht getestet wurde.
:::

- TCS BVS20-SG / Koch BVS20
- TCS ISH 3030 / ISH 3130
- TCS ISW 3030 / ISW 3130
- TCS HST 1030
- TCS IMM 1300
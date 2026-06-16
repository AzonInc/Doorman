---
description: Explore the complete history of PCB revisions, including new features, improvements, and changes in each update.
---

# Audio Extension Changelog
This page highlights the new features, improvements, and changes made in each revision.

## 2.3.0 <Badge type="warning" text="Next" />
- Replace `TLV9002IDSGR` with `TS972IQT` for more output current

## 2.2.0 <Badge type="danger" text="Fail" />
- Add `TLV9002IDSGR` Op-Amp to buffer the audio output

#### Note
C15 and C16 cause problems. The Op-Amp generates further noise but does not improve the overall throughput.

## 2.1.0 <Badge type="danger" text="Fail" />
- Silkscreen improvement
- Change C4, C12, C8 and C14 to 50V 1210
- Add C15 and C16 additionally to C8 and C14 to double the capacity

#### Note
Audio output is limited, only 50% reach the bus.

## 2.0.0
- Change Extension Connector pinout
- Use `ES8311` mono codec instead of `ES8388` codec
- Differential audio in- and outputs
- Remove VCC from extension connector
- Remove second channel in- and outputs

#### Note
Audio output is limited, only 50% reach the bus.

## 1.0.0 <Badge type="info" text="Initial release" /> <Badge type="danger" text="Fail" />
No changelog available

#### Note
Audio output and input does not work, reason unknown.
ES8388 is now discontinued by Everest Semiconductor.
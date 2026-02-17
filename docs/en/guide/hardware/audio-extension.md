---
description: Audio extension board for the Doorman S3
---

# Audio Extension Board

<div style="display: flex; flex-direction: row; flex-wrap: wrap; justify-content: space-between; gap: 20px;align-items: center;" markdown>
   <div style="min-width:260px;flex: 1 1 260px;" markdown>

This extension board adds full audio capability to your Doorman S3. It allows you to both listen and transmit audio back through the intercom bus.

The board is built around the [ES8388](http://www.everest-semi.com/pdf/ES8388%20DS.pdf) audio codec, providing high-quality analog-to-digital and digital-to-analog conversion for reliable two-way audio communication.

   </div>
   <div style="min-width: 185px;max-width:200px;flex: 1 1 185px;" markdown>
        <img style="" src="/audio-extension-alt.png">
   </div>
</div>

::: danger LIMITATIONS
The extension board connector on the Doorman base board was introduced in revision `2.0.0` and is unfortunately not available on earlier versions. A Doorman base board of revision `2.0.0` or later is required in order to connect extension boards.
:::

## Audio channels

The extension board provides two audio channels: the first handles intercom bus audio through the `L1` and `R1` inputs and `L1` output, with `L1` output also feeding directly into `R1` input, which acts as a differential signal to filter the output and reduce noise. `R1` output can also serve independently as a line-level output. The second channel offers `L2` and `R2` outputs for line-level audio and `L2` and `R2` inputs for additional line-level sources.

## Assembly

Simply plug the extension board into the 2×7-pin connector on the Doorman base board. It's very easy — just follow the video guide below.

<video controls autoplay loop src="./video.mp4" />
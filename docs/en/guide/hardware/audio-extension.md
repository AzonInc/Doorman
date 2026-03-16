---
description: Audio extension board for the Doorman S3
---

# Audio Extension Board

<div style="display: flex; flex-direction: row; flex-wrap: wrap; justify-content: space-between; gap: 20px;align-items: center;" markdown>
   <div style="min-width:260px;flex: 1 1 260px;" markdown>

This extension board adds full audio capability to your Doorman S3. It allows you to both listen and transmit audio back through the intercom bus.

The board is built around the [ES8311](http://www.everest-semi.com/pdf/ES8311%20PB.pdf) audio codec, providing high-quality analog-to-digital and digital-to-analog conversion for reliable two-way audio communication with echo cancellation.

   </div>
   <div style="min-width: 185px;max-width:200px;flex: 1 1 185px;" markdown>
        <img style="" src="/audio-extension-alt.png">
   </div>
</div>

::: danger LIMITATIONS
The extension board connector on the Doorman core board was introduced in revision `2.0.0` and is unfortunately not available on earlier versions. A Doorman core board of revision `2.0.0` or later is required in order to connect extension boards.
:::

## Assembly

Simply plug the extension board into the 2×7-pin connector on the Doorman core board. It's very easy — just follow the video guide below.

<video controls autoplay loop src="./video.mp4" />
---
# https://vitepress.dev/reference/default-theme-home-page
layout: home
description: Doorman S3 lets you connect your TCS or Koch intercom to any home automation system.

hero:
  name: Doorman
  text: Gateway for TCS and Koch intercoms
  tagline: Made for ESPHome and Home Assistant
  image:
    src: /enclosure-base-board.png
    alt: Doorman
  actions:
    - theme: brand
      text: What is Doorman?
      link: /guide/what-is-doorman

    - theme: alt
      text: Quickstart
      link: /guide/getting-started

    - theme: alt
      text: GitHub
      link: https://github.com/azoninc/doorman

features:
  - icon:
      src: /icons/fluent-emoji-rocket.png
      wrap: true
      height: 28px
      width: 28px
    title: Powerful
    details: Powered by the ESP32-S3, featuring a dual-core processor, integrated PSRAM, and Bluetooth connectivity for versatile and reliable operation.

  - icon:
      src: /icons/fluent-emoji-pinching-hand.png
      wrap: true
      height: 28px
      width: 28px
    title: Perfect Size
    details: Its compact design allows it to fit almost anywhere, making it suitable for a wide range of installation scenarios.

  - icon:
      src: /icons/fluent-emoji-sparkles.png
      wrap: true
      height: 28px
      width: 28px
    title: Expandable
    details: Additional GPIOs are accessible via the screw terminal. Furthermore, an expansion board connector provides access to eight additional GPIOs, as well as the bus lines, 3.3V, and Ground.

  - icon:
      src: /icons/fluent-emoji-party-popper.png
      wrap: true
      height: 28px
      width: 28px
    title: Compatible
    details: The solid state relay allows you to switch up to 40V (max. 2.5A), perfect for integrating with old-school intercom systems.
---
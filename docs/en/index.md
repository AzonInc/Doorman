---
# https://vitepress.dev/reference/default-theme-home-page
layout: home

hero:
  name: Doorman
  text: Gateway for TCS and Koch intercoms
  tagline: Made for ESPHome and Home Assistant
  image:
    src: /enclosure.png
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
  - icon: 🚀
    title: Powerful
    details: Powered by the ESP32-S3, featuring a dual-core CPU, integrated PSRAM, and Bluetooth, making it both powerful and versatile.

  - icon: 🤏​
    title: Perfect Size
    details: A compact design allows it to fit almost anywhere, making it ideal for a wide range of installation scenarios.

  - icon: ✨​
    title: Expandable
    details: Two additional GPIOs and an external button allow for effortless expansion with your own sensors and devices.

  - icon: 🎉​
    title: Compatible
    details: The solid state relay allows you to switch up to 40V (max. 2.5A), perfect for integrating with old-school intercom systems.
---
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
      link: https://github.com/AzonInc/Doorman

features:
  - icon: 🚀
    title: Powerful
    details: It's powered by the ESP32-S3 with dual core CPU, integrated PSRAM, and Bluetooth, making it highly powerful and versatile.

  - icon: 🤏​
    title: Perfect Size
    details: It's so compact that it fits almost anywhere, making it ideal for a wide range of installation scenarios.

  - icon: ✨​
    title: Expandable
    details: Two additional GPIOs and an external button enable effortless expansion with your own sensors and devices.

  - icon: 🎉​
    title: Compatible
    details: The solid state relay allows you to switch up to 40V (max. 2.5A), perfect for integrating with old-school intercom systems.
---

<style>
:root {
  --vp-home-hero-name-color: transparent;
  --vp-home-hero-name-background: -webkit-linear-gradient(120deg, #bd34fe 30%, #41d1ff);

  --vp-home-hero-image-background-image: linear-gradient(-45deg, #bd34fe 50%, #47caff 50%);
  --vp-home-hero-image-filter: blur(44px);
}

@media (min-width: 640px) {
  :root {
    --vp-home-hero-image-filter: blur(56px);
  }
}

@media (min-width: 960px) {
  :root {
    --vp-home-hero-image-filter: blur(68px);
  }
}
</style>


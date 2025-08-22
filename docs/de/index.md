---
# https://vitepress.dev/reference/default-theme-home-page
layout: home

hero:
  name: Doorman
  text: Gateway für TCS und Koch intercoms
  tagline: Für ESPHome und Home Assistant gemacht
  image:
    src: /enclosure.png
    alt: Doorman
  actions:
    - theme: brand
      text: Was ist Doorman?
      link: /de/guide/what-is-doorman
    - theme: alt
      text: Quickstart
      link: /de/guide/getting-started

    - theme: alt
      text: GitHub
      link: https://github.com/azoninc/doorman

features:
  - icon:
      src: /icons/fluent-emoji-rocket.png
      wrap: true
      height: 28px
      width: 28px
    title: Leistungsfähig
    details: Angetrieben vom ESP32-S3 mit Dual-Core-CPU, integriertem PSRAM und Bluetooth, vereint er beeindruckende Leistung mit vielseitiger Einsatzfähigkeit.

  - icon:
      src: /icons/fluent-emoji-pinching-hand.png
      wrap: true
      height: 28px
      width: 28px
    title: Kompakt
    details: Dank seines durchdachten Designs findet er fast überall Platz und ist somit ideal für eine Vielzahl von Installationsszenarien geeignet.

  - icon:
      src: /icons/fluent-emoji-sparkles.png
      wrap: true
      height: 28px
      width: 28px
    title: Erweiterbar
    details: Zwei zusätzliche GPIOs und ein externer Button ermöglichen eine flexible und einfache Erweiterung mit eigenen Sensoren und Geräten, z. B. über I²C.

  - icon:
      src: /icons/fluent-emoji-party-popper.png
      wrap: true
      height: 28px
      width: 28px
    title: Kompatibel
    details: Das Solid-State-Relais ermöglicht das Schalten von bis zu 40V (max. 2,5A) und ist somit ideal für die Integration in ältere Gegensprechanlagen geeignet.
---
<script setup>
import { VPTeamMembers } from 'vitepress/theme'

const members = [
  {
    avatar: 'https://www.github.com/AzonInc.png',
    name: 'AzonInc',
    title: 'Creator of the Doorman-S3 PCB and ESPHome-based firmware',
    links: [
      { icon: 'github', link: 'https://github.com/AzonInc' },
    ],
    sponsor: 'https://github.com/sponsors/AzonInc',
  },
  {
    avatar: 'https://www.github.com/peteh.png',
    name: 'peteh',
    title: 'Creator of the original Doorman custom firmware',
    links: [
      { icon: 'github', link: 'https://github.com/peteh' },
    ],
    sponsor: 'https://github.com/sponsors/peteh',
  },
  {
    avatar: 'https://www.github.com/AStrehlau.png',
    name: 'AStrehlau',
    title: 'Electronics Engineer',
    links: [
      { icon: 'github', link: 'https://github.com/AStrehlau' },
    ]
  },
  {
    avatar: 'https://www.github.com/atc1441.png',
    name: 'atc1441',
    title: 'Reverse engineering of TCS Bus communication',
    links: [
      { icon: 'github', link: 'https://github.com/atc1441' },
    ],
    sponsor: 'https://paypal.me/hoverboard1'
  },
  {
    avatar: 'https://www.github.com/uriyacovy.png',
    name: 'uriyacovy',
    title: 'Creator of the ESPHome Nuki Lock component',
    links: [
      { icon: 'github', link: 'https://github.com/uriyacovy' },
    ]
  }
]
</script>

# Danke ❤️

Doorman basiert auf dem Code und den Informationen der folgenden Projekte und Personen. Ohne eure Beiträge wäre dieses Projekt nicht möglich gewesen.

### Coole Leute

<VPTeamMembers size="small" :members="members" />


### Projekte

**[TCSIntercomArduino](https://github.com/atc1441/TCSintercomArduino)**\
Verschiedene Methoden zum Lesen und Schreiben auf dem TCS-Bus.\
Wenn du interessiert bist, kannst du dir gerne das [Reverse Engineering Video](https://www.youtube.com/watch?v=xFLoauqj9yA) ansehen.

**[tcs-monitor](https://github.com/Syralist/tcs-monitor)**\
Ein MQTT-Monitor zum Überwachen des TCS-Bus.\
Weitere Informationen findest du in diesem [Blog Post](https://blog.syralist.de/posts/smarthome/klingel/).

**[Doorman](https://github.com/peteh/doorman)**\
Ein wesentlicher Teil des Doorman-Projekts basiert auf der Arbeit von peteh.\
Das Hauptziel war es, ein robustes PCB für sein Doorman-Projekt bereitzustellen.

**[ESPHome_nuki_lock](https://github.com/uriyacovy/ESPHome_nuki_lock)**\
Diese Komponente erstellt eine ESPHome-Schlossplattform für das Nuki Smartlock (nuki_lock).
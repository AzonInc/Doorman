<script setup>
import { VPTeamMembers } from 'vitepress/theme'
import { data } from '../contributors.data.js'

const members = [
  {
    avatar: 'https://www.github.com/azoninc.png',
    name: 'AzonInc',
    title: 'Ersteller vom Doorman S3 PCB und der ESPHome-basierten Firmware',
    links: [
      { icon: 'github', link: 'https://github.com/azoninc' },
    ],
    sponsor: 'https://github.com/sponsors/azoninc',
  },
  {
    avatar: 'https://www.github.com/peteh.png',
    name: 'peteh',
    title: 'Entwickler der original Doorman custom Firmware',
    links: [
      { icon: 'github', link: 'https://github.com/peteh' },
    ],
    sponsor: 'https://github.com/sponsors/peteh',
  },
  {
    avatar: 'https://www.github.com/astrehlau.png',
    name: 'AStrehlau',
    title: 'Electronics Engineer',
    links: [
      { icon: 'github', link: 'https://github.com/astrehlau' },
    ]
  },
  {
    avatar: 'https://www.github.com/mstirner.png',
    name: 'mStirner',
    title: 'Audio Unterstützung',
    links: [
      { icon: 'github', link: 'https://github.com/mStirner' },
    ]
  },
  {
    avatar: 'https://www.github.com/calibanorg.png',
    name: 'calibanorg',
    title: 'Sponsor für die Entwicklung',
    links: [
      { icon: 'github', link: 'https://github.com/calibanorg' },
    ]
  },
  {
    avatar: 'https://www.github.com/atc1441.png',
    name: 'atc1441',
    title: 'Reverse engineering der TCS Bus Kommunikation',
    links: [
      { icon: 'github', link: 'https://github.com/atc1441' },
    ],
    sponsor: 'https://paypal.me/hoverboard1'
  },
  {
    avatar: 'https://www.github.com/uriyacovy.png',
    name: 'uriyacovy',
    title: 'Entwickler der ESPHome Nuki Lock Komponente',
    links: [
      { icon: 'github', link: 'https://github.com/uriyacovy' },
    ]
  }
]

data.map((x) => {
  x.title = 'Verfasser von ' + x.contributions + ' Commit ' + (x.contributions != 1 ? 's' : '');
  return x;
});

const mergedMembers = [
  ...members,
  ...data.filter(d => !members.some(m => m.name === d.name))
];
</script>

# Danke ❤️

Doorman basiert auf dem Code und den Informationen der folgenden Projekte und Personen. Ohne eure Beiträge wäre dieses Projekt nicht möglich gewesen.

### Coole Leute

<VPTeamMembers size="small" :members="mergedMembers" />


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
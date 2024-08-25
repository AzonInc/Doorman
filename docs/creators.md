<script setup>
import { VPTeamMembers } from 'vitepress/theme'

const members = [
  {
    avatar: 'https://www.github.com/AzonInc.png',
    name: 'AzonInc',
    title: 'Creator of the Doorman-S3 PCB and ESPHome-based firmware',
    links: [
      { icon: 'github', link: 'https://github.com/AzonInc' },
    ]
  },
  {
    avatar: 'https://www.github.com/peteh.png',
    name: 'peteh',
    title: 'Creator of the original Doorman custom firmware',
    links: [
      { icon: 'github', link: 'https://github.com/peteh' },
    ]
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
    ]
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

# Thank you ❤️

Doorman is built upon the code and information from the following projects and individuals. This project would not have been possible without their contributions.

### Amazing People

<VPTeamMembers size="small" :members="members" />


### Projects

**[TCSIntercomArduino](https://github.com/atc1441/TCSintercomArduino)**\
Different methods to read from and write to the TCS Bus.\
If you're interested, feel free to watch the [Reverse Engineering Video](https://www.youtube.com/watch?v=xFLoauqj9yA).

**[tcs-monitor](https://github.com/Syralist/tcs-monitor)**\
A MQTT monitor for listening to the TCS Bus.\
You can find more information in this [blog post](https://blog.syralist.de/posts/smarthome/klingel/).

**[Doorman](https://github.com/peteh/doorman)**\
A significant portion of the Doorman project is based on peteh's work.\
The primary goal was to provide a robust PCB for his Doorman project.

**[ESPHome_nuki_lock](https://github.com/uriyacovy/ESPHome_nuki_lock)**\
This module builds an ESPHome lock platform for Nuki Smartlock (nuki_lock).
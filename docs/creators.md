<script setup>
import { VPTeamMembers } from 'vitepress/theme'

const members = [
  {
    avatar: 'https://www.github.com/AzonInc.png',
    name: 'AzonInc',
    title: 'Creator of the Doorman-S3 PCB and ESPHome based Firmware',
    links: [
      { icon: 'github', link: 'https://github.com/AzonInc' },
    ]
  },
  {
    avatar: 'https://www.github.com/peteh.png',
    name: 'peteh',
    title: 'Creator of the original Doorman Custom Firmware',
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
  }
]
</script>

# Thank you ❤️

Doorman is heavily built on the code and the information of the following projects and people.
This project would not have been possible without all of them. 

### Amazing People

<VPTeamMembers size="small" :members="members" />


### Projects

**[TCSIntercomArduino](https://github.com/atc1441/TCSintercomArduino)**\
Different Methods to read from and write to the TCS Bus.\
Feel free to watch the [Reverse Engineering Video](https://www.youtube.com/watch?v=xFLoauqj9yA) if you're interested.

**[tcs-monitor](https://github.com/Syralist/tcs-monitor)**\
An mqtt monitor for listening to the TCS Bus.\
You can find more information in this [Blog Post](https://blog.syralist.de/posts/smarthome/klingel/).

**[Doorman](https://github.com/peteh/doorman)**\
A lot of this Doorman project is based on peteh's one.\
The main goal is to provide a proper PCB for his Doorman Project.
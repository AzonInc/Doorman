import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  title: "Doorman",
  description: "Doorman S3 lets you connect your TCS or Koch intercom to any home automation system.",
  
  /* prettier-ignore */
  head: [
    ['link', { rel: 'icon', type: 'image/png', href: '/logo.png' }],
    ['meta', { name: 'theme-color', content: '#5f67ee' }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:locale', content: 'en' }],
    ['meta', { property: 'og:title', content: 'Doorman | Gateway for TCS and Koch intercoms' }],
    ['meta', { property: 'og:description', content: 'Doorman S3 lets you connect your TCS or Koch intercom to any home automation system.' }],
    ['meta', { property: 'og:site_name', content: 'Doorman' }],
    ['meta', { property: 'og:image', content: 'https://doorman.azon.ai/doorman-og.jpg' }],
    ['meta', { property: 'og:url', content: 'https://doorman.azon.ai/' }],
    [
      'script',
      {
        async: '',
        type: 'module',
        src: 'https://unpkg.com/esp-web-tools@10/dist/web/install-button.js?module'
      }
    ]
  ],

  cleanUrls: true,
  
  themeConfig: {

    logo: {
      src: '/logo.png', width: 24, height: 24
    },

    // https://vitepress.dev/reference/default-theme-config
    nav: [
      { text: 'Guide', link: '/guide/what-is-doorman' },
      { text: 'Creators', link: '/creators' }
    ],

    editLink: {
      pattern: 'https://github.com/AzonInc/Doorman/edit/master/docs/:path'
    },

    search: {
      provider: 'local'
    },

    sidebar: [
      {
        text: 'Introduction',
        items: [
          { text: 'What is Doorman?', link: '/guide/what-is-doorman' },
          { text: 'Getting Started', link: '/guide/getting-started' },
        ]
      },
      {
        text: 'Compatibility',
        items: [
          { text: 'Can I use Doorman?', link: '/compatibility/hardware-compatibility' },
          { text: 'Doorman PCB', link: '/compatibility/specifications' }
        ]
      },
      {
        text: 'Firmware',
        items: [
          { text: 'Stock Firmware', link: '/firmware/stock-firmware' },
          { text: 'Nuki Bridge Firmware', link: '/firmware/nuki-bridge-firmware' },
          { text: 'Custom Firmware', link: '/firmware/custom-firmware' }
        ]
      },
      {
        text: 'Automations',
        items: [
          { text: 'Pattern Events', link: '/automation/pattern-events' },
          { text: 'Ring To Open', link: '/automation/ring-to-open' }
        ]
      },
      {
        text: 'Enclosure',
        items: [
          { text: '3D printing', link: '/enclosure/3d-printing' }
        ]
      }
    ],

    socialLinks: [
      { icon: 'github', link: 'https://github.com/AzonInc/Doorman' },
      { icon: 'discord', link: 'https://discord.gg/t2d34dvmBf' }
    ]
  }
})

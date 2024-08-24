import { createRequire } from 'module'
import { defineConfig, type DefaultTheme } from 'vitepress'

const require = createRequire(import.meta.url)
const pkg = require('../../package.json')

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
    ],
    [
      'script',
      {
        async: '',
        type: 'module',
        src: 'https://kicanvas.org/kicanvas/kicanvas.js'
      }
    ],
    [
      'link',
      {
        async: '',
        rel: 'stylesheet',
        crossorigin: 'anonymous',
        href: 'https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@48,400,0,0&family=Nunito:wght@300;400;500;600;700&display=swap'
      }
    ]
  ],

  cleanUrls: true,
  appearance: 'dark',
  lastUpdated: true,

  themeConfig: {

    logo: {
      src: '/logo.png', width: 24, height: 24
    },

    // https://vitepress.dev/reference/default-theme-config
    nav: [
      {
        text: 'Guide',
        link: '/guide/what-is-doorman',
        activeMatch: '/guide/'
      },
      {
        text: 'Reference',
        link: '/reference/entities',
        activeMatch: '/reference/'
      },
      {
        text: pkg.version,
        items: [
          {
            text: !pkg.version.includes('dev') ? 'Switch to Development Docs' : 'Switch to Stable Docs',
            link: !pkg.version.includes('dev') ? 'https://doorman-dev.surge.sh/' : 'https://doorman.azon.ai/',
            target: '_self'
          }
        ]
      }
    ],

    editLink: {
      pattern: 'https://github.com/AzonInc/Doorman/edit/master/docs/:path'
    },

    search: {
      provider: 'local'
    },

    sidebar: {
      '/guide/': { base: '/guide/', items: sidebarGuide() },
      '/reference/': { base: '/reference/', items: sidebarReference() }
    },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/AzonInc/Doorman' },
      { icon: 'discord', link: 'https://discord.gg/t2d34dvmBf' }
    ],

    footer: {
      message: 'Released under the GPL 3.0 License.',
      copyright: 'Made with ❤️ by the <a href="creators">Creators</a> of Doorman.'
    }
  }
})

function sidebarGuide(): DefaultTheme.SidebarItem[] {
  return [
    {
      text: 'Introduction',
      items: [
        { text: 'What is Doorman?', link: 'what-is-doorman' },
        { text: 'Can I use Doorman?', link: 'hardware-compatibility' },
        { text: 'Getting Started', link: 'getting-started' }
      ]
    },
    {
      text: 'Firmware',
      items: [
        { text: 'Stock Firmware', link: 'stock-firmware', base: '/guide/firmware/' },
        { text: 'Nuki Bridge Firmware', link: 'nuki-bridge-firmware', base: '/guide/firmware/' },
        { text: 'Custom Firmware', link: 'custom-firmware', base: '/guide/firmware/' }
      ]
    },
    {
      text: 'Automations',
      items: [
        { text: 'Pattern Events', link: 'pattern-events', base: '/guide/automation/' },
        { text: 'Ring To Open', link: 'ring-to-open', base: '/guide/automation/' },
        { text: 'Blueprints', link: 'blueprints', base: '/guide/automation/' }
      ]
    },
    {
      text: 'Enclosure',
      items: [
        { text: '3D printing', link: '3d-printing', base: '/guide/enclosure/' }
      ]
    },
    {
      text: 'Support',
      link: 'support',
      base: '/guide/support/',
      collapsed: true,
      items: [
        { text: 'Troubleshooting', link: 'troubleshooting', base: '/guide/support/' },
      ]
    }
  ];
}

function sidebarReference(): DefaultTheme.SidebarItem[] {
  return [
    {
      text: 'Reference',
      items: [
        { text: 'Entities', link: 'entities' },
        { text: 'GPIO Pinout', link: 'gpio' },
        { text: 'Schematics', link: 'schematics' }
      ]
    }
  ];
}
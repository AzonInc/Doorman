import { createRequire } from 'module'
import { defineConfig, type DefaultTheme } from 'vitepress'

const require = createRequire(import.meta.url)
const pkg = require('../../package.json')

export const en = defineConfig({
    lang: 'en-US',
    description: "Doorman S3 lets you connect your TCS or Koch intercom to any home automation system.",
  
    /* prettier-ignore */
    head: [
        ['meta', { property: 'og:locale', content: 'en' }],
        ['meta', { property: 'og:title', content: 'Doorman | Gateway for TCS and Koch intercoms' }],
        ['meta', { property: 'og:description', content: 'Doorman S3 lets you connect your TCS or Koch intercom to any home automation system.' }],
    ],

  themeConfig: {

    nav: nav(),

    sidebar: {
      '/guide/': { base: '/guide/', items: sidebarGuide() },
      '/reference/': { base: '/reference/', items: sidebarReference() }
    },

    editLink: {
        pattern: 'https://github.com/AzonInc/Doorman/edit/master/docs/:path'
    },

    footer: {
      message: 'Released under the GPL 3.0 License.',
      copyright: 'Made with ❤️ by the <a href="creators">Creators</a> of Doorman.'
    }
  }
})

function nav(): DefaultTheme.NavItem[] {
    return [
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
            { text: 'Issues', link: 'https://github.com/AzonInc/Doorman/issues' },
            { text: 'Changelog', link: 'changelog' },
            {
              text: 'Docs',
              items: [
                {
                  text: !pkg.version.includes('dev') ? 'Switch to development' : 'Switch to current',
                  link: !pkg.version.includes('dev') ? 'https://doorman-dev.surge.sh/' : 'https://doorman.azon.ai/',
                  target: '_self'
                }
              ]
            }
          ]
        }
      ]
  }

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
        {
          text: 'Entities',
          link: 'entities',
          base: '/reference/',
        },
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
        { text: 'Schematics', link: 'schematics' },
        { text: 'Specifications', link: 'specifications' },
        { text: 'ESPHome Component', link: 'esphome-component' }
      ]
    }
  ];
}
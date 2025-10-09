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
      '/reference/': { base: '/reference/', items: sidebarReference() },
      '/changelog': { base: '/changelog/', items: sidebarChangelog() }
    },

    editLink: {
        pattern: 'https://github.com/azoninc/doorman/edit/master/docs/:path'
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
          text: 'Order Doorman',
          link: '/order',
          activeMatch: '/order',
        },
        {
          text: pkg.version,
          items: [
            { text: 'Issues', link: 'https://github.com/azoninc/doorman/issues' },
            {
              text: 'Changelog',
              link: '/changelog/firmware',
              activeMatch: '/changelog/'
            },
            {
              text: 'Docs',
              items: [
                {
                  text: !pkg.version.includes('dev') ? 'Switch to development' : 'Switch to current',
                  link: !pkg.version.includes('dev') ? 'https://dev.doorman.azon.ai/' : 'https://doorman.azon.ai/',
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
      text: 'Features',
      items: [
        { text: 'Ring To Open', link: 'ring-to-open', base: '/guide/features/' },
        { text: 'Indoor Station Settings', link: 'intercom-settings', base: '/guide/features/' },
        { text: 'Pattern Events', link: 'pattern-events', base: '/guide/features/' },
      ]
    },
    {
      text: 'Firmware',
      items: [
        { text: 'Install or Update', link: 'installation', base: '/guide/firmware/' },
        {
          text: 'Smart Home Integrations',
          collapsed: true,
          items: [
            { text: 'Home Assistant', link: 'home-assistant', base: '/guide/firmware/' },
            { text: 'MQTT', link: 'mqtt', base: '/guide/firmware/' },
            { text: 'HomeKit', link: 'homekit', base: '/guide/firmware/' },
          ]
        },
        {
          text: 'Addons',
          collapsed: true,
          items: [
            { text: 'Nuki Bridge', link: 'nuki-bridge', base: '/guide/firmware/' },
          ]
        },
        { text: 'Customization', link: 'customization', base: '/guide/firmware/' },
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
      text: 'Hardware',
      items: [
        { text: 'Schematics', link: 'schematics' },
        { text: 'Specifications', link: 'specifications' },
        { text: 'GPIO Pinout', link: 'gpio' },
      ]
    },
    {
      text: 'ESPHome',
      items: [
        { text: 'TC:BUS Component', link: 'esphome-component' }
      ]
    },
    {
      text: 'Entities',
      link: 'entities',
      items: [
        { text: 'Doorman', link: 'entities-doorman' },
        { text: 'TC:BUS', link: 'entities-tc-bus' },
        { text: 'MQTT Integration', link: 'entities-mqtt' },
        { text: 'HomeKit Integration', link: 'entities-homekit' },
        { text: 'Nuki Bridge', link: 'entities-nuki-bridge' },
        { text: 'Nuki Lock', link: 'entities-nuki-lock' },
      ]
    }
  ];
}

function sidebarChangelog(): DefaultTheme.SidebarItem[] {
  return [
    {
      text: 'Changelog',
      items: [
        { text: 'Firmware', link: 'firmware' },
        { text: 'Hardware', link: 'hardware' }
      ]
    }
  ];
}
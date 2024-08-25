import { createRequire } from 'module'
import { defineConfig, type DefaultTheme } from 'vitepress'

const require = createRequire(import.meta.url)
const pkg = require('../../package.json')

export const de = defineConfig({
    lang: 'de-DE',
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
        '/de/guide/': { base: '/de/guide/', items: sidebarGuide() },
        '/de/reference/': { base: '/de/reference/', items: sidebarReference() }
    },

    editLink: {
        text: 'Diese Seite auf GitHub bearbeiten'
    },
    
    footer: {
        message: 'Veröffentlicht unter der GPL 3.0 Lizenz.',
        copyright: 'Mit ❤️ von den Doorman <a href="creators">Entwicklern</a> entwickelt.'
    },
    
    docFooter: {
        prev: 'Vorherige',
        next: 'Nächste'
    },
    
    outline: {
        label: 'Auf dieser Seite'
    },
    
    lastUpdated: {
        text: 'Zuletzt aktualisiert am',
    },
    
    langMenuLabel: 'Sprache ändern',
    returnToTopLabel: 'Nach oben zurückkehren',
    sidebarMenuLabel: 'Seitenmenü',
    darkModeSwitchLabel: 'Dunkelmodus',
    lightModeSwitchTitle: 'Zum Hellmodus wechseln',
    darkModeSwitchTitle: 'Zum Dunkelmodus wechseln'
  }
})

function nav(): DefaultTheme.NavItem[] {
    return [
        {
          text: 'Guide',
          link: '/de/guide/what-is-doorman',
          activeMatch: '/de/guide/'
        },
        {
          text: 'Referenzen',
          link: '/de/reference/entities',
          activeMatch: '/de/reference/'
        },
        {
          text: pkg.version,
          items: [
            {
              text: !pkg.version.includes('dev') ? 'Wechsle zu Development Docs' : 'Wechsle zu Stable Docs',
              link: !pkg.version.includes('dev') ? 'https://doorman-dev.surge.sh/' : 'https://doorman.azon.ai/',
              target: '_self'
            }
          ]
        }
      ]
  }

function sidebarGuide(): DefaultTheme.SidebarItem[] {
  return [
    {
      text: 'Einführung',
      items: [
        { text: 'Was ist Doorman?', link: 'what-is-doorman' },
        { text: 'Kann ich Doorman verwenden?', link: 'hardware-compatibility' },
        { text: 'Erste Schritte', link: 'getting-started' }
      ]
    },
    {
      text: 'Firmware',
      items: [
        { text: 'Stock Firmware', link: 'stock-firmware', base: '/de/guide/firmware/' },
        { text: 'Nuki-Bridge Firmware', link: 'nuki-bridge-firmware', base: '/de/guide/firmware/' },
        {
          text: 'Entities',
          link: 'entities',
          base: '/de/reference/',
        },
        { text: 'Custom Firmware', link: 'custom-firmware', base: '/de/guide/firmware/' }
      ]
    },
    {
      text: 'Automationen',
      items: [
        { text: 'Pattern Events', link: 'pattern-events', base: '/de/guide/automation/' },
        { text: 'Ring To Open', link: 'ring-to-open', base: '/de/guide/automation/' },
        { text: 'Blueprints', link: 'blueprints', base: '/de/guide/automation/' }
      ]
    },
    {
      text: 'Gehäuse',
      items: [
        { text: '3D-Druck', link: '3d-printing', base: '/de/guide/enclosure/' }
      ]
    },
    {
      text: 'Support',
      link: 'support',
      base: '/de/guide/support/',
      collapsed: true,
      items: [
        { text: 'Fehlerbehebung', link: 'troubleshooting', base: '/de/guide/support/' },
      ]
    }
  ];
}

function sidebarReference(): DefaultTheme.SidebarItem[] {
  return [
    {
      text: 'Referenzen',
      items: [
        { text: 'Entities', link: 'entities' },
        { text: 'GPIO Pinout', link: 'gpio' },
        { text: 'Schaltpläne', link: 'schematics' }
      ]
    }
  ];
}

export const search: DefaultTheme.LocalSearchOptions['locales'] = {
  de: {
    translations: {
      button: {
        buttonText: 'Suchen',
        buttonAriaLabel: 'Suchen'
      },
      modal: {
        displayDetails: 'Details anzeigen',
        resetButtonTitle: 'Suche zurücksetzen',
        backButtonTitle: 'Suche schließen',
        noResultsText: 'Keine Ergebnisse',
        footer: {
          selectText: 'Auswählen',
          selectKeyAriaLabel: 'Eingabe',
          navigateText: 'Navigieren',
          navigateUpKeyAriaLabel: 'Pfeil nach oben',
          navigateDownKeyAriaLabel: 'Pfeil nach unten',
          closeText: 'Schließen',
          closeKeyAriaLabel: 'esc'
        }
      }
    }
  }
}
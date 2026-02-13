import { createRequire } from 'module'
import { defineConfig, type DefaultTheme } from 'vitepress'

const require = createRequire(import.meta.url)
const pkg = require('../../package.json')

const branch = pkg.version.includes('dev') ? 'dev' : 'master';

export const de = defineConfig({
  lang: 'de-DE',
  description: "Doorman S3 lets you connect your TCS or Koch intercom to any home automation system.",

  /* prettier-ignore */
  head: [
    ['meta', { property: 'og:locale', content: 'de' }],
    ['meta', { property: 'og:title', content: 'Doorman | Gateway für TCS und Koch Gegensprechanlagen' }],
    ['meta', { property: 'og:description', content: 'Mit dem Doorman S3 Gateway kannst du deine TCS oder Koch Gegensprechanlage an jedes Heimautomatisierungssystem anbinden.' }],
    ['meta', { name: 'og:description', content: 'Mit dem Doorman S3 Gateway kannst du deine TCS oder Koch Gegensprechanlage an jedes Heimautomatisierungssystem anbinden.' }],
  ],

  themeConfig: {
    nav: nav(),

    sidebar: {
      '/de/guide/': { base: '/de/guide/', items: sidebarGuide() },
      '/de/reference/': { base: '/de/reference/', items: sidebarReference() },
      '/de/changelog/': { base: '/de/changelog/', items: sidebarChangelog() }
    },

    editLink: {
      pattern: 'https://github.com/azoninc/doorman/edit/' + branch  + '/docs/:path',
      text: 'Diese Seite bearbeiten'
    },

    notFound: {
      title: 'SEITE NICHT GEFUNDEN',
      linkText: 'Bring mich nach Hause',
      linkLabel: 'Label',
      quote: 'Aber wenn du deine Richtung nicht änderst und weiterhin in dieselbe Richtung schaust, könntest du am Ende genau dort landen, wohin du steuerrst.'
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
      text: 'Referenz',
      link: '/de/reference/entities',
      activeMatch: '/de/reference/'
    },
    {
      text: 'Hol dir deinen Doorman',
      link: '/de/order',
      activeMatch: '/de/order'
    },
    {
      text: pkg.version,
      items: [
        { text: 'Issues', link: 'https://github.com/azoninc/doorman/issues' },
        {
          text: 'Changelog',
          link: '/changelog/firmware',
          activeMatch: '/de/changelog/'
        },
        {
          text: 'Docs',
          items: [
            {
              text: branch != 'dev' ? 'Wechsle zur Entwicklungs Version' : 'Wechsle zu aktuellen Version',
              link: branch != 'dev' ? 'https://dev.doorman.azon.ai/' : 'https://doorman.azon.ai/',
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
      text: 'Einführung',
      items: [
        { text: 'Was ist Doorman?', link: 'what-is-doorman' },
        { text: 'Kann ich den verwenden?', link: 'hardware-compatibility' },
        { text: 'Erste Schritte', link: 'getting-started' }
      ]
    },
    {
      text: 'Features',
      items: [
        { text: 'Ring To Open', link: 'ring-to-open', base: '/de/guide/features/' },
        { text: 'Innenstation Einstellungen', link: 'intercom-settings', base: '/de/guide/features/' },
        { text: 'Muster Ereignisse', link: 'pattern-events', base: '/de/guide/features/' },
      ]
    },
    {
      text: 'Firmware',
      items: [
        { text: 'Installation und Updates', link: 'installation', base: '/de/guide/firmware/' },
        {
          text: 'Smart Home Integrationen',
          collapsed: true,
          items: [
            { text: 'Home Assistant', link: 'home-assistant', base: '/de/guide/firmware/' },
            { text: 'MQTT', link: 'mqtt', base: '/de/guide/firmware/' },
            { text: 'HomeKit', link: 'homekit', base: '/de/guide/firmware/' },
          ]
        },
        {
          text: 'Erweiterungen',
          collapsed: true,
          items: [
            { text: 'Nuki Bridge', link: 'nuki-bridge', base: '/de/guide/firmware/' },
          ]
        },
        { text: 'Anpassungen', link: 'customization', base: '/de/guide/firmware/' }
      ]
    },
    {
      text: 'Hardware',
      items: [
        { text: 'Audio Erweiterung', link: 'audio-extension', base: '/de/guide/hardware/' }
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
      text: 'Hardware',
      items: [
        { text: 'Schaltpläne', link: 'schematics' },
        { text: 'Spezifikationen', link: 'specifications' },
        { text: 'GPIO Pinbelegung', link: 'gpio' },
      ]
    },
    {
      text: 'ESPHome Komponenten',
      items: [
        { text: 'TC:BUS', link: 'esphome-component' },
        { text: 'TC:BUS Device', link: 'esphome-component-device' },
        { text: 'TC:BUS Serial', link: 'esphome-component-serial' },
        { text: 'Hardware', link: 'esphome-component-hardware' }
      ]
    },
    {
      text: 'Entitäten',
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
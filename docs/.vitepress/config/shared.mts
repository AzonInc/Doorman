import { createRequire } from 'module'
import { defineConfig } from 'vitepress'
import { search as deSearch } from './de.mts'

const require = createRequire(import.meta.url)
const pkg = require('../../package.json')

export const shared = defineConfig({
  title: "Doorman",

  rewrites: {
    'en/:rest*': ':rest*'
  },

  cleanUrls: true,
  appearance: 'dark',
  lastUpdated: true,
  metaChunk: true,

  sitemap: {
    hostname: pkg.version.includes('dev') ? 'https://doorman-dev.surge.sh' : 'https://doorman.azon.ai',
    transformItems(items) {
      return items.filter((item) => !item.url.includes('migration'))
    }
  },
  
  /* prettier-ignore */
  head: [
    ['link', { rel: 'icon', type: 'image/png', href: '/logo.png' }],
    ['meta', { name: 'theme-color', content: '#5f67ee' }],
    ['meta', { property: 'og:type', content: 'website' }],
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

  

  themeConfig: {

    logo: {
      src: '/logo.png', width: 24, height: 24
    },

    lastUpdated: {
        formatOptions: {
            dateStyle: 'short',
            timeStyle: 'medium'
        }
    },

    editLink: {
        pattern: 'https://github.com/AzonInc/Doorman/edit/master/docs/:path',
    },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/AzonInc/Doorman' },
      { icon: 'discord', link: 'https://discord.gg/t2d34dvmBf' }
    ],

    search: {
      provider: 'local',
      options: {
        locales: {
          ...deSearch,
        }
      }
    }
  }
})
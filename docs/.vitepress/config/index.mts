import { defineConfig } from 'vitepress'
import { tabsMarkdownPlugin } from 'vitepress-plugin-tabs'
import { groupIconMdPlugin, groupIconVitePlugin, localIconLoader } from 'vitepress-plugin-group-icons'

import { shared } from './shared.mts'
import { en } from './en.mts'
import { de } from './de.mts'

export default defineConfig({
  ...shared,
  locales: {
    root: { label: 'English', ...en },
    de: { label: 'Deutsch', ...de },
  },
  markdown: {
    config(md) {
      md.use(tabsMarkdownPlugin)
      md.use(groupIconMdPlugin)
    }
  },
  /*vue: {
    template: {
      compilerOptions: {
        isCustomElement: tag => [
          'esp-web-install-button',
          'improv-wifi-launch-button',
          'improv-wifi-serial-launch-button'
        ].includes(tag)
      }
    }
  },*/
  vite: {
    plugins: [
      groupIconVitePlugin({
        customIcon: {
          'homekit': localIconLoader(import.meta.url, '../../public/icons/homekit-white.svg'),
          'home assistant': localIconLoader(import.meta.url, '../../public/icons/home-assistant.svg'),
          'mqtt': localIconLoader(import.meta.url, '../../public/icons/mqtt-icon-transparent.svg'),
        },
      })
    ],
  }
})
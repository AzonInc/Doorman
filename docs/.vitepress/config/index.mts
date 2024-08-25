import { defineConfig } from 'vitepress'
import { shared } from './shared.mts'
import { en } from './en.mts'
import { de } from './de.mts'

export default defineConfig({
  ...shared,
  locales: {
    root: { label: 'English', ...en },
    de: { label: 'Deutsch', ...de },
  }
})
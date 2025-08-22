import { defineConfig } from 'vite'
import svgLoader from 'vite-svg-loader'
import Icons from 'unplugin-icons/vite'
import IconsResolver from 'unplugin-icons/resolver'
import Components from 'unplugin-vue-components/vite'

export default defineConfig({
  plugins: [
    svgLoader(),
    Icons({
      compiler: 'vue3',
      customCollections: {
        
      },
      autoInstall: true,
      webComponents: {
        autoDefine: true
      }
    }),
    Components({
      // allow auto load markdown components under `.vitepress/theme/components`
      dirs: ['.vitepress/theme/components'],
      extensions: ['vue', 'md'],
      // allow auto import and register components used in markdown
      include: [/\.vue$/, /\.vue\?vue/, /\.md$/],
      dts: 'components.d.ts',
      resolvers: [
        IconsResolver({
          prefix: 'icon',
        }),
      ],
    }),
  ],
})
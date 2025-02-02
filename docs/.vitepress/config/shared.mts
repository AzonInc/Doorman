

export default defineConfig({
  head: [
    // og:site_name, og:type, og:image:width, og:image:height, twitter:card go here
  ],
  
  // …
});


import { createRequire } from 'module'
import { defineConfig } from 'vitepress'
import { search as deSearch } from './de.mts'
import { createTitle, normalize } from "vitepress/dist/client/shared.js";

const ORIGIN = "https://doorman.azon.ai";
const FALLBACK_META_IMAGE = "doorman-og.jpg";

function href(path = "") {
  return new URL(normalize(path), ORIGIN).href;
}

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

  ignoreDeadLinks: [
    /ibom/,
  ],

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

  transformPageData(pageData, ctx) {
    let pageDescription = pageData.frontmatter?.description;
    const pageHref = href(pageData.relativePath);
    const pageImage = href(
      pageData.frontmatter?.metaImage ?? FALLBACK_META_IMAGE,
    );
    const pageTitle = createTitle(ctx.siteConfig.site, pageData);

    if (!pageDescription) {
      pageDescription = ctx.siteConfig.site?.description;

      // If no page-specific description and not homepage, prepend the site title to the description
      if (pageDescription && pageHref !== href()) {
        pageDescription = [ctx.siteConfig.site?.title, pageDescription]
          .filter((v) => Boolean(v))
          .join(": ");
      }
    }

    pageData.frontmatter.head ??= [];

    pageData.frontmatter.head.push(
      [
        "meta",
        {
          property: "og:image",
          content: pageImage,
        },
      ],
      [
        "meta",
        {
          name: "og:title",
          content: pageTitle,
        },
      ],
      [
        "meta",
        {
          property: "og:title",
          content: pageTitle,
        },
      ],
      [
        "meta",
        {
          property: "og:url",
          content: pageHref,
        },
      ],
      [
        "meta",
        {
          name: "og:image",
          content: pageImage,
        },
      ],
      [
        "meta",
        {
          name: "twitter:title",
          content: pageTitle,
        },
      ],
    );

    if (pageDescription) {
      pageData.frontmatter.head.push(
        [
          "meta",
          {
            name: "og:description",
            content: pageDescription,
          },
        ],
        [
          "meta",
          {
            name: "twitter:description",
            content: pageDescription,
          },
        ],
      );
    }
  },

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
  },
  vue: {
    template: {
      compilerOptions: {
        isCustomElement: tag => tag.startsWith('Tres') && tag !== 'TresCanvas',
      },
    },
  },
})
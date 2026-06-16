import { defineConfig } from "vite";
import gzipPlugin from "rollup-plugin-gzip";
import { viteSingleFile } from "vite-plugin-singlefile";

import minifyHTML from "rollup-plugin-minify-html-template-literals";
import { minifyHtml as ViteMinifyHtml } from "vite-plugin-html";

export default defineConfig({
  clearScreen: false,
  plugins: [
    viteSingleFile(),
    { ...minifyHTML(), enforce: "pre", apply: "build" },
    ViteMinifyHtml(),
    {
      ...gzipPlugin({ filter: /\.(html)$/ }),
      enforce: "post",
      apply: "build",
    },
  ],
  css: {
    postcss: {},
  },
  build: {
    brotliSize: false,
    cssCodeSplit: false,
    outDir: "./dist",
    assetsInlineLimit: 100000000,
    polyfillModulePreload: false,
  },
  server: {
    open: "/", // auto open browser
  },
});

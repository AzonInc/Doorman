# captive-portal
Source code to build esphome captive portal. Output is `captive_index.h` file to be included by the Captive Portal Component https://esphome.io/components/captive_portal.html

###  Features

- All assets (css, svg and js) are inlined and served from index.html
- index.html is gzipped, and stored in flash compressed saving ~1K of flash memory from previous version
- ssid scan result is returned via `/config.json` api request


development
===========

```
git clone https://github.com/esphome/esphome-webserver.git
cd captive-portal
pnpm install
```

`npm run start`
Starts a dev server on http://localhost:3000

build
=====
`npm run build`
The build files are copied to `dist` folder. `captive_index.h` is built to be deployed to https://github.com/esphome/esphome/tree/dev/esphome/components/captive_portal


serve
=====
`npm run server`
Starts a production test server on http://localhost:5001

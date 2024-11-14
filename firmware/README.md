# Firmware Config Files

This directory contains [ESPHome](https://esphome.io) config files for the Doorman.

It is organized like so:

- `base.yaml`: The base essential configuration, which contains components for reading from and writing to the TC/TCS Bus 
- `doorman-stock.yaml`: Inherits everything from `base.yaml`.
- `doorman-stock.dev.yaml`: Inherits everything from `doorman-stock.yaml` and replaces stable components with dev.
- `doorman-nuki-bridge.yaml`: Inherits everything from `base.yaml` and adds the Nuki Bridge Components
- `doorman-nuki-bridge.dev.yaml`: Inherits everything from `doorman-nuki-bridge.yaml` and replaces stable components with dev.

If you just want to get started with Doorman you should take a look at `stock.example.yaml` in the examples directory.\
For further documentation of the tc_bus component [check the docs](https://doorman.azon.ai/reference/esphome-component).
# Firmware Config Files

This directory contains [ESPHome](https://esphome.io) config files for the Doorman.

It is organized like so:

- `base.yaml`: The base essential configuration, which contains components for reading from and writing to the TCS Bus 
- `doorman-stock.yaml`: Inherits everything from `base.yaml`.
- `doorman-nuki-bridge.yaml`: Inherits everything from `base.yaml` and adds the Nuki Bridge Components

If you just want to get started with Doorman you should take a look at `stock.example.yaml` in the examples directory.\
For further documentation of the tcs_intercom component look [here](https://github.com/AzonInc/Doorman/tree/master/components).
# GPIO Pinout

The Doorman-S3 uses certain GPIO pins for specific functions and provides additional GPIO pins for your own sensors and devices.

| GPIO | Purpose |
| :----- | :-----|
| GPIO01 | Red Status LED |
| GPIO02 | WS2812B RGB Status LED |
| GPIO08 | TC:BUS TX - Shorts the Bus to Ground |
| GPIO09 | TC:BUS RX - Reads Bus Data / ADC input (with onboard Voltage Divider - 1M+160K) |
| GPIO40 | Free I/O |
| GPIO41 | 10K Onboard Resistor for External Button |
| GPIO42 | Relay for Analog Door Opener or Light |
| GPIO48 | Free I/O |
# Doorman Hardware specifications
You can find a few information here about how to use 

## Power supply
Thanks to the wide voltage converter, the Doorman S3 can operate with a supply voltage ranging from 5 to 30V DC.

## Bridge Rectifier
The Bridge Rectifier will correct the Bus wire polarity. You don't have to worry about it.

## Relay
The relay is a solid-state type, capable of switching loads up to 40V with a maximum current of 2.5A.

## External Button
It is a pull-up button with a 10 kΩ resistor. You can connect the button to BTN and G screw terminals.

## GPIO Pin assignment

Doorman needs some GPIO for a specific purpose and offers free GPIO for your sensors.

| GPIO | Purpose |
| :----- | :-----|
| GPIO01 | Red Status LED |
| GPIO02 | WS2812B RGB Status LED |
| GPIO08 | TCS Bus TX - Shorts the Bus to Ground |
| GPIO09 | TCS Bus RX - Reading Bus Data / ADC input (onboard Voltage Divider - 1M+160K) |
| GPIO40 | Free I/O |
| GPIO41 | 10K onboard resistor for external button |
| GPIO42 | Relay for analog door opener |
| GPIO48 | Free I/O |
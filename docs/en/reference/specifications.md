# Doorman Hardware Specifications
You can find a few information here about how to use 

## Power Supply
Thanks to the wide voltage converter, the Doorman S3 can operate with a supply voltage ranging from 5 to 30V DC.

## Power Consumption
The following tables outline the power consumption of different firmware variants when operating with a **24 V DC-input**.

::: tip NOTE
The integrated RGB status LED draws approximately **4 mA** when active. This is in addition to the values shown below.
:::

### Stock Firmware
| Integration | Power Consumption |
| :----- | :-----: |
| Home Assistant | TBD |
| MQTT | TBD |
| HomeKit | TBD |
| Custom | TBD |

### Nuki-Bridge Firmware
| Integration | Power Consumption |
| :----- | :-----: |
| Home Assistant | 20-22mA |
| Custom | TBD |

## Bridge Rectifier
The Bridge Rectifier will correct the Bus wire polarity. You don't have to worry about it.

## Relay
The relay is a solid-state type, capable of switching loads up to 40V with a maximum current of 2.5A.

## External Button
It is a pull-up button with a 10 kΩ resistor. You can connect the button to BTN and G screw terminals.
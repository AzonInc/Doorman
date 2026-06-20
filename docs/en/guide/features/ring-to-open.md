---
description: Ring To Open (RTO) is a Doorman feature that automates entrance doors, allowing them to open when someone rings the doorbell.
---

# Ring To Open <Badge type="warning" text="RTO" />

The Doorman firmware features a Ring To Open automation.

### What does it do?
As the name implies, the entrance door will open automatically when someone rings the doorbell.
This feature is especially useful during a party, allowing guests to enter the building simply by ringing the bell.

### How do I use it?
You can activate or deactivate the Ring to Open function individually for each door.
There are separate switches available, such as `RTO: Entrance Door`, `RTO: Second Entrance Door`, and `RTO: Apartment Door`.

You can also configure an additional trigger, like the function button or the external button.

Additionally, there's a central switch called `Ring to Open`, which can control multiple doors at once.
You can configure which doors are affected via the setting `RTO: Central Toggle – Door Selection`.

::: tip INFO
Most settings are hidden by default in the interface.

To view them, click `SHOW ALL` in the Doorman web interface or enable the corresponding entity in Home Assistant.
:::

#### General Configuration:
::: info Confirmation
If this option is enabled, your indoor station will ring each time the Ring-to-Open automation is triggered by a physical button — depending on which physical button you've configured as the trigger.
:::

::: info RGB Status LED: Show Ring To Open Status
You can choose whether the status LED should blink while Ring to Open is active for any door.
:::

::: info Central Toggle - Trigger
Define what toggles the central `Ring to Open` switch.
For example, you can set the function button of your indoor station to activate or deactivate the central switch.
:::

::: info Central Toggle - Door Selection
Specify which doors are controlled by the central `Ring to Open` switch.
:::

#### Configuration per door:
::: info Delay
This setting allows you to configure a delay for the door opener.
:::

::: info Timeout
You can define whether Ring to Open should automatically deactivate after a certain time (between 5 and 60 minutes), after a single ring, or not at all.
:::

::: info Pattern Condition
Select which ringing pattern should trigger the automation.
:::
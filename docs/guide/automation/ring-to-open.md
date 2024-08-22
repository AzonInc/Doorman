# Ring To Open <Badge type="warning" text="Party Mode" />

The Doorman Firmware offers a `Ring To Open` automation, also known as `Party Mode`.

### What does it do?
As the name already says, the entrance door will be opened as soon as someone rings the entrance doorbell.
It could be useful when you have a party. That way your guests can just enter the building by ringing.

### How do I use it?
You can enable and disable the `Ring To Open` automation with the switch in Home Assistant or the `Ring To Open Toggle Command`.

#### Configuration Options:

::: info Toggle Command
This setting is useful when you have an unused function button on your intercom phone.\
The button can quickly toggle the `Ring To Open` automation.
:::

::: info Confirmation
If you enable this setting, your apartment doorbell will ring whenever you enable the `Ring To Open` automation via command.
:::

::: info Delay
This setting allows you to configure a delay for the `Open Entrance Door Command`.\
If you set the delay to 60 seconds it will choose a random delay between 5 and 15 seconds.
:::

::: info Timeout
You have the options to disable the `Ring To Open` automation after 5 to 60 minutes, after a single ring or never.
:::
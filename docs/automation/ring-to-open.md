# Ring To Open <Badge type="warning" text="Party Mode" />

The Doorman Firmware offers a `Ring To Open` automation, also known as `Party Mode`.

### What does it do?
As the name already says, the entrance door will be opened as soon as someone rings the entrance doorbell.
It could be useful when you have a party. That way your guests can just enter the building by ringing.

### How do I use it?
You can enable and disable the `Ring To Open` automation with either the switch in Home Assistant or the TCS `Ring To Open Toggle Command`.

The latter is useful when you have an unused function button on your intercom phone.

::: tip
It is also possible to configure a delay for the opener command with the `Ring To Open Delay` Number Input in the Configuration section.\
If you set the delay to 60 seconds it will choose a random time between 5 and 15 seconds.

You can also use the `Ring To Open Confirmation` setting to let the doorbell ring once after enabling the `Ring To Open` automation.
:::
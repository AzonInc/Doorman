# Siedle In-Home Bus Component

ESPHome component for receiving and sending messages on the Siedle In-Home Bus (2-wire bus system).

## Hardware

The Siedle In-Home Bus uses a 2-wire bus. A transmission is signalled when the bus level drops to carrier level (~7V). Four GPIO pins are required:

| Pin | Direction | Function |
|-----|-----------|----------|
| `carrier_pin` | Input | Detects bus activity (HIGH = carrier ~7V present) |
| `rx_pin` | Input | Reads data bits from the bus |
| `load_pin` | Output | Pulls bus to carrier level (~7V) when sending |
| `tx_pin` | Output | Encodes data bits (LOW = 1, HIGH = 0) |

## Message Format

Each message is a 32-bit value with the following bit layout:

```
Bit 31..29  Prolog        (3 bits, always 0b010)
Bit 28..23  Command       (6 bits, 0x00–0x3F)
Bit 22..18  Destination   (5 bits, 0x00–0x1F)
Bit 17..14  Destination Bus (4 bits, 0x0–0xF, default 0x8)
Bit 13..11  Middle        (3 bits, always 0b010)
Bit 10..6   Source        (5 bits, 0x00–0x1F)
Bit  5..2   Source Bus    (4 bits, 0x0–0xF, default 0x8)
Bit  1..0   Epilog        (2 bits, always 0b00)
```

## Configuration

```yaml
siedle_in_home_bus:
  carrier_pin: GPIO18
  rx_pin: GPIO19
  load_pin: GPIO21
  tx_pin: GPIO22
  dump: true          # Log all received messages (default: true)
  on_message:
    - then:
        - logger.log:
            format: "Received raw: 0x%08X"
            args: ["msg.get_raw()"]
```

### Options

| Option | Required | Default | Description |
|--------|----------|---------|-------------|
| `carrier_pin` | yes | — | GPIO detecting bus carrier level |
| `rx_pin` | yes | — | GPIO reading data bits |
| `load_pin` | yes | — | GPIO pulling bus to carrier level |
| `tx_pin` | yes | — | GPIO encoding data bits |
| `dump` | no | `true` | Log all received messages to console |
| `on_message` | no | — | Automation triggered on every received message |

### `on_message` Trigger

Fires for every received message. The variable `msg` of type `SiedleInHomeBusMessage` is available:

```yaml
on_message:
  - then:
      - lambda: |-
          ESP_LOGI("bus", "cmd=0x%02X dst=0x%X:0x%02X src=0x%X:0x%02X",
            msg.get_command(),
            msg.get_destination_bus(), msg.get_destination(),
            msg.get_source_bus(), msg.get_source());
```

## Binary Sensor

Publishes `true` when a specific message is received, auto-resets after a configurable delay.

```yaml
binary_sensor:
  - platform: siedle_in_home_bus
    name: "Doorbell"
    message:
      command: 0x12
      destination: 0x05
      source: 0x08
      # destination_bus: 0x08  # optional, default 0x08
      # source_bus: 0x08       # optional, default 0x08
    auto_off: 3s              # optional, default 3s
```

### Binary Sensor Options

| Option | Required | Default | Description |
|--------|----------|---------|-------------|
| `message` | yes | — | Message to match (see message fields below) |
| `auto_off` | no | `3s` | Time until sensor resets to `false` after triggering |

## Button

Sends a message on the bus when pressed.

```yaml
button:
  - platform: siedle_in_home_bus
    name: "Open Door"
    message:
      command: 0x20
      destination: 0x01
      source: 0x08
```

## `siedle_in_home_bus.send` Action

Sends a message on the bus from an automation. Supports both raw values and decoded fields.

**With raw value:**
```yaml
on_press:
  - siedle_in_home_bus.send:
      raw: 0x4B980A53
```

**With decoded fields:**
```yaml
on_press:
  - siedle_in_home_bus.send:
      command: 0x12
      destination: 0x05
      source: 0x08
      # destination_bus: 0x08  # optional, default 0x08
      # source_bus: 0x08       # optional, default 0x08
```

**With templates (runtime values):**
```yaml
on_press:
  - siedle_in_home_bus.send:
      command: !lambda "return id(my_number).state;"
      destination: 0x05
      source: 0x08
```

### Send Action Options

Either `raw` or `command` + `destination` + `source` must be specified. `raw` and decoded fields cannot be combined.

| Option | Required | Default | Description |
|--------|----------|---------|-------------|
| `raw` | — | — | Complete 32-bit message value (templatable) |
| `command` | — | — | Command field, 0x00–0x3F (templatable) |
| `destination` | — | — | Destination address, 0x00–0x1F (templatable) |
| `destination_bus` | no | `0x08` | Destination bus, 0x0–0xF (templatable) |
| `source` | — | — | Source address, 0x00–0x1F (templatable) |
| `source_bus` | no | `0x08` | Source bus, 0x0–0xF (templatable) |

## Message Fields

Used in `binary_sensor`, `button`, and the `siedle_in_home_bus.send` action:

| Field | Range | Default | Description |
|-------|-------|---------|-------------|
| `command` | `0x00`–`0x3F` | — | 6-bit command identifier |
| `destination` | `0x00`–`0x1F` | — | 5-bit destination address |
| `destination_bus` | `0x0`–`0xF` | `0x08` | 4-bit destination bus segment |
| `source` | `0x00`–`0x1F` | — | 5-bit source address |
| `source_bus` | `0x0`–`0xF` | `0x08` | 4-bit source bus segment |

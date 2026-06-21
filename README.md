# OpenM5Kit
OpenM5Kit is an open source Arduino firmware starter kit for building, sharing, and deploying projects across M5Stack devices like the M5Stick and CoreS3.

The project is structured as an Arduino library with device-specific example sketches:

```text
src/                  Shared OpenM5Kit runtime code
examples/M5Stick/     Deployable sketch for M5Stick-class devices
examples/CoreS3/      Deployable sketch for M5Stack CoreS3
docs/                 Setup and deployment notes
```

## Supported Targets

- M5Stick-C / M5Stick-C Plus / M5Stick-C Plus2
- M5Stack CoreS3

## Quick Start

1. Install the M5Stack board package in Arduino IDE.
2. Install the `M5Unified` and `M5GFX` libraries.
3. Open one of the sketches in `examples/`.
4. Select the matching board and serial port.
5. Upload to the connected device.

See [docs/setup.md](docs/setup.md) and [docs/deployment.md](docs/deployment.md) for details.

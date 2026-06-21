# OpenM5Kit
OpenM5Kit is an open source Arduino firmware starter kit and dashboard shell for building AI-capable M5Stack devices like the M5Stick and CoreS3.

The project is structured as an Arduino library with device-specific example sketches:

```text
src/                  Shared OpenM5Kit runtime code
examples/M5Stick/     Deployable sketch for M5Stick-class devices
examples/CoreS3/      Deployable sketch for M5Stack CoreS3
docs/                 Setup and deployment notes
dashboard/            Next.js control plane web app
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

## Dashboard

The web dashboard shell lives in `dashboard/`. It is intended to become a control plane for receiving data from M5 devices, sending data back on demand, and routing AI requests through a LiteLLM proxy.

```bash
cd dashboard
npm install
npm run dev
```

For CoreS3 Wi-Fi onboarding into the dashboard, see [docs/dashboard-onboarding.md](docs/dashboard-onboarding.md).

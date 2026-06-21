# Dashboard Onboarding

This is the first local development flow for connecting a CoreS3 to the dashboard over Wi-Fi.

## 1. Start the Dashboard

Run the dashboard dev server from the `dashboard/` directory:

```bash
npm run dev
```

The dev server binds to `0.0.0.0`, so devices on the same Wi-Fi network can reach it through your computer's LAN IP.

Find your computer's Wi-Fi IP address. On macOS:

```bash
ipconfig getifaddr en0
```

Use that IP for the device config. For example:

```text
http://192.168.1.25:3000
```

Do not use `localhost` in the device config. On the device, `localhost` means the device itself.

## 2. Configure CoreS3

Copy the example config:

```text
examples/CoreS3/OpenM5KitConfig.example.h
```

to:

```text
examples/CoreS3/OpenM5KitConfig.h
```

Then edit `OpenM5KitConfig.h`:

```cpp
#define OPENM5KIT_DASHBOARD_ENABLED true
#define OPENM5KIT_WIFI_SSID "YOUR_WIFI_SSID"
#define OPENM5KIT_WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define OPENM5KIT_DASHBOARD_BASE_URL "http://YOUR_COMPUTER_WIFI_IP:3000"
#define OPENM5KIT_DEVICE_ID "cores3-dev-001"
#define OPENM5KIT_PAIRING_CODE "482913"
```

`OpenM5KitConfig.h` is ignored by git so local Wi-Fi credentials do not get committed.

## 3. Upload Firmware

Open:

```text
examples/CoreS3/CoreS3.ino
```

Select the `M5Stack-CoreS3` board, select the serial port, then upload.

## 4. Confirm Connection

The CoreS3 display should show:

- Wi-Fi status
- pairing code
- last API status
- last endpoint path
- last HTTP error text

The dashboard should show the device under **Device Data** after the first hello and telemetry posts arrive.

## Message Board

The dashboard can queue a short message for a connected device. CoreS3 polls:

```text
GET /api/devices/:deviceId/message
```

When a message is available, the dashboard returns it as plain text and marks it delivered. The CoreS3 displays the message on the main device screen.

The polling interval is configured in `OpenM5KitConfig.h`:

```cpp
#define OPENM5KIT_MESSAGE_INTERVAL_MS 3000
```

## Screen Sleep

CoreS3 keeps Wi-Fi and message polling active while the display is asleep.

The display sleeps after the configured idle timeout:

```cpp
#define OPENM5KIT_SCREEN_TIMEOUT_MS 60000
```

Touching the screen wakes it. A new dashboard message also wakes the display and resets the idle timer.

## Troubleshooting `API: -1`

`API: -1` means the device did not receive an HTTP response from the dashboard.

Check these first:

```text
1. The dashboard URL in OpenM5KitConfig.h uses your computer LAN IP.
2. The dashboard URL uses the active dashboard port.
3. The dashboard is reachable from another device on the same Wi-Fi.
4. The CoreS3 is not on a guest or isolated IoT network.
5. macOS firewall allows inbound connections to Node/Next.
```

Open Arduino IDE Serial Monitor at `115200` baud to see the full URL being posted and the HTTP client error string.

## Current API Endpoints

The device posts to:

```text
POST /api/devices/hello
POST /api/devices/events
GET  /api/devices
GET  /api/devices/:deviceId/message
POST /api/devices/:deviceId/message
```

The dashboard currently stores device data in memory. Restarting the dev server clears the device list.

export type DeviceEvent = {
  id: string;
  receivedAt: string;
  eventType: string;
  battery?: number;
  rssi?: number;
  uptimeMs?: number;
  freeHeap?: number;
  buttonA?: boolean;
  buttonB?: boolean;
  raw: Record<string, unknown>;
};

export type DeviceRecord = {
  deviceId: string;
  deviceType: string;
  deviceName: string;
  firmwareVersion: string;
  pairingCode: string;
  ipAddress?: string;
  rssi?: number;
  battery?: number;
  uptimeMs?: number;
  firstSeenAt: string;
  lastSeenAt: string;
  status: "online" | "stale";
  events: DeviceEvent[];
};

type Store = {
  devices: Map<string, DeviceRecord>;
};

const globalStore = globalThis as typeof globalThis & {
  openM5KitStore?: Store;
};

const store =
  globalStore.openM5KitStore ??
  (globalStore.openM5KitStore = {
    devices: new Map<string, DeviceRecord>(),
  });

function stringValue(value: unknown, fallback = "") {
  return typeof value === "string" && value.length > 0 ? value : fallback;
}

function numberValue(value: unknown) {
  return typeof value === "number" && Number.isFinite(value) ? value : undefined;
}

function booleanValue(value: unknown) {
  return typeof value === "boolean" ? value : undefined;
}

function statusFor(lastSeenAt: string): DeviceRecord["status"] {
  const lastSeenMs = new Date(lastSeenAt).getTime();
  return Date.now() - lastSeenMs > 45000 ? "stale" : "online";
}

export function recordHello(payload: Record<string, unknown>) {
  const now = new Date().toISOString();
  const deviceId = stringValue(payload.deviceId, "unknown-device");
  const current = store.devices.get(deviceId);

  const device: DeviceRecord = {
    deviceId,
    deviceType: stringValue(payload.deviceType, current?.deviceType ?? "unknown"),
    deviceName: stringValue(payload.deviceName, current?.deviceName ?? deviceId),
    firmwareVersion: stringValue(
      payload.firmwareVersion,
      current?.firmwareVersion ?? "unknown",
    ),
    pairingCode: stringValue(payload.pairingCode, current?.pairingCode ?? ""),
    ipAddress: stringValue(payload.ipAddress, current?.ipAddress ?? ""),
    rssi: numberValue(payload.rssi) ?? current?.rssi,
    battery: numberValue(payload.battery) ?? current?.battery,
    uptimeMs: numberValue(payload.uptimeMs) ?? current?.uptimeMs,
    firstSeenAt: current?.firstSeenAt ?? now,
    lastSeenAt: now,
    status: "online",
    events: current?.events ?? [],
  };

  store.devices.set(deviceId, device);
  return device;
}

export function recordEvent(payload: Record<string, unknown>) {
  const device = recordHello(payload);
  const event: DeviceEvent = {
    id: `${device.deviceId}-${Date.now()}`,
    receivedAt: device.lastSeenAt,
    eventType: stringValue(payload.eventType, "telemetry"),
    battery: numberValue(payload.battery),
    rssi: numberValue(payload.rssi),
    uptimeMs: numberValue(payload.uptimeMs),
    freeHeap: numberValue(payload.freeHeap),
    buttonA: booleanValue(payload.buttonA),
    buttonB: booleanValue(payload.buttonB),
    raw: payload,
  };

  device.events = [event, ...device.events].slice(0, 20);
  store.devices.set(device.deviceId, device);
  return event;
}

export function listDevices() {
  return Array.from(store.devices.values())
    .map((device) => ({
      ...device,
      status: statusFor(device.lastSeenAt),
    }))
    .sort((a, b) => b.lastSeenAt.localeCompare(a.lastSeenAt));
}

"use client";

import { useEffect, useState } from "react";
import {
  Cable,
  Database,
  LayoutDashboard,
  MessageSquareText,
  Network,
  Radio,
  Settings,
  Terminal,
} from "lucide-react";

import { Badge } from "@/components/ui/badge";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Separator } from "@/components/ui/separator";
import type { DeviceRecord } from "@/lib/device-store";

const navItems = [
  { label: "Overview", icon: LayoutDashboard, active: true },
  { label: "Signals", icon: Radio },
  { label: "Prompts", icon: MessageSquareText },
  { label: "LiteLLM", icon: Network },
  { label: "I/O", icon: Cable },
  { label: "Settings", icon: Settings },
];

function formatRelativeTime(value: string) {
  const elapsedSeconds = Math.max(
    0,
    Math.round((Date.now() - new Date(value).getTime()) / 1000),
  );

  if (elapsedSeconds < 5) {
    return "just now";
  }

  if (elapsedSeconds < 60) {
    return `${elapsedSeconds}s ago`;
  }

  return `${Math.round(elapsedSeconds / 60)}m ago`;
}

function formatUptime(value?: number) {
  if (typeof value !== "number") {
    return "-";
  }

  const totalSeconds = Math.floor(value / 1000);
  const minutes = Math.floor(totalSeconds / 60);
  const seconds = totalSeconds % 60;
  return `${minutes}m ${seconds}s`;
}

function formatHeap(freeHeap?: number, heapSize?: number) {
  if (typeof freeHeap !== "number") {
    return "-";
  }

  const freeKb = freeHeap / 1024;

  if (typeof heapSize !== "number" || heapSize <= 0) {
    return `${Math.round(freeKb)} KB`;
  }

  const percent = Math.round((freeHeap / heapSize) * 100);
  return `${Math.round(freeKb)} KB (${percent}%)`;
}

function statusBadgeClass(status: DeviceRecord["status"]) {
  if (status === "online") {
    return "border-emerald-200 bg-emerald-50 text-emerald-700";
  }

  return "border-border bg-muted text-muted-foreground";
}

export function DashboardClient() {
  const [devices, setDevices] = useState<DeviceRecord[]>([]);
  const [lastRefreshAt, setLastRefreshAt] = useState<string | null>(null);
  const [messageTextByDevice, setMessageTextByDevice] = useState<
    Record<string, string>
  >({});
  const [sendingDeviceId, setSendingDeviceId] = useState<string | null>(null);

  async function loadDevices() {
    const response = await fetch("/api/devices", { cache: "no-store" });
    const payload = (await response.json()) as { devices: DeviceRecord[] };

    setDevices(payload.devices);
    setLastRefreshAt(new Date().toISOString());
  }

  useEffect(() => {
    let active = true;

    async function loadDevicesIfActive() {
      const response = await fetch("/api/devices", { cache: "no-store" });
      const payload = (await response.json()) as { devices: DeviceRecord[] };

      if (active) {
        setDevices(payload.devices);
        setLastRefreshAt(new Date().toISOString());
      }
    }

    loadDevicesIfActive();
    const interval = window.setInterval(loadDevicesIfActive, 3000);

    return () => {
      active = false;
      window.clearInterval(interval);
    };
  }, []);

  async function sendMessage(deviceId: string) {
    const text = messageTextByDevice[deviceId]?.trim();
    if (!text) {
      return;
    }

    setSendingDeviceId(deviceId);
    try {
      const response = await fetch(`/api/devices/${deviceId}/message`, {
        method: "POST",
        headers: {
          "content-type": "application/json",
        },
        body: JSON.stringify({ text }),
      });

      if (!response.ok) {
        throw new Error(`Message request failed with ${response.status}`);
      }

      setMessageTextByDevice((current) => ({
        ...current,
        [deviceId]: "",
      }));
      await loadDevices();
    } finally {
      setSendingDeviceId(null);
    }
  }

  return (
    <main className="min-h-screen bg-muted/30 text-foreground">
      <div className="grid min-h-screen lg:grid-cols-[260px_1fr]">
        <aside className="hidden border-r bg-background lg:block">
          <div className="flex h-16 items-center gap-3 px-5">
            <div className="flex h-9 w-9 items-center justify-center rounded-md border bg-foreground text-background">
              <Terminal className="h-4 w-4" />
            </div>
            <div>
              <div className="text-sm font-semibold">OpenM5Kit</div>
              <div className="text-xs text-muted-foreground">AI device plane</div>
            </div>
          </div>
          <Separator />
          <nav className="space-y-1 p-3">
            {navItems.map((item) => (
              <button
                key={item.label}
                className={`flex h-9 w-full items-center gap-3 rounded-md px-3 text-left text-sm transition-colors ${
                  item.active
                    ? "bg-accent font-medium text-accent-foreground"
                    : "text-muted-foreground hover:bg-accent hover:text-accent-foreground"
                }`}
                type="button"
              >
                <item.icon className="h-4 w-4" />
                {item.label}
              </button>
            ))}
          </nav>
        </aside>

        <section className="flex min-w-0 flex-col">
          <header className="flex h-16 items-center justify-between gap-4 border-b bg-background px-4 sm:px-6">
            <div className="flex min-w-0 items-center gap-3">
              <div className="flex h-9 w-9 items-center justify-center rounded-md border bg-foreground text-background lg:hidden">
                <Terminal className="h-4 w-4" />
              </div>
              <div className="min-w-0">
                <h1 className="truncate text-base font-semibold">Dashboard</h1>
                <p className="truncate text-sm text-muted-foreground">
                  AI device control plane
                </p>
              </div>
            </div>

            <div className="text-xs text-muted-foreground">
              {lastRefreshAt ? `Updated ${formatRelativeTime(lastRefreshAt)}` : ""}
            </div>
          </header>

          <div className="flex-1 space-y-5 p-4 sm:p-6">
            <div>
              <h2 className="text-xl font-semibold tracking-normal">Devices</h2>
              <p className="mt-1 text-sm text-muted-foreground">
                Connected M5 devices and the latest data they have reported.
              </p>
            </div>

            {devices.length === 0 ? (
              <div className="flex min-h-[300px] items-center justify-center rounded-md border border-dashed bg-background">
                <div className="max-w-sm text-center">
                  <div className="mx-auto flex h-11 w-11 items-center justify-center rounded-md border bg-background">
                    <Database className="h-5 w-5 text-muted-foreground" />
                  </div>
                  <h3 className="mt-4 text-sm font-medium">No devices reporting</h3>
                  <p className="mt-2 text-sm text-muted-foreground">
                    Start the dashboard on your LAN, flash CoreS3 with Wi-Fi config,
                    and telemetry will appear here.
                  </p>
                </div>
              </div>
            ) : (
              <div className="grid gap-4">
                {devices.map((device) => (
                  <Card key={device.deviceId}>
                    <CardHeader>
                      <div className="flex flex-col gap-3 sm:flex-row sm:items-start sm:justify-between">
                        <div>
                          <div className="flex items-center gap-2">
                            <CardTitle>{device.deviceName}</CardTitle>
                            <Badge className={statusBadgeClass(device.status)}>
                              {device.status}
                            </Badge>
                          </div>
                          <p className="mt-1 text-xs text-muted-foreground">
                            {device.deviceId} · {device.deviceType} · firmware{" "}
                            {device.firmwareVersion}
                          </p>
                        </div>
                        <div className="text-xs text-muted-foreground">
                          Last seen {formatRelativeTime(device.lastSeenAt)}
                        </div>
                      </div>
                    </CardHeader>

                    <CardContent>
                      <div className="grid gap-3 text-sm sm:grid-cols-2 xl:grid-cols-4">
                        <Metric label="Pairing code" value={device.pairingCode || "-"} />
                        <Metric label="IP address" value={device.ipAddress || "-"} />
                        <Metric
                          label="Battery"
                          value={
                            typeof device.battery === "number"
                              ? `${device.battery}%`
                              : "-"
                          }
                        />
                        <Metric
                          label="RSSI"
                          value={
                            typeof device.rssi === "number" ? `${device.rssi} dBm` : "-"
                          }
                        />
                      </div>

                      <Separator className="my-4" />

                      <div>
                        <div className="text-xs font-medium uppercase tracking-normal text-muted-foreground">
                          Latest telemetry
                        </div>
                        {device.events[0] ? (
                          <div className="mt-3 grid gap-3 text-sm sm:grid-cols-2">
                            <Metric
                              label="Uptime"
                              value={formatUptime(device.events[0].uptimeMs)}
                            />
                            <Metric
                              label="Free heap"
                              value={formatHeap(
                                device.events[0].freeHeap,
                                device.events[0].heapSize,
                              )}
                            />
                          </div>
                        ) : (
                          <div className="mt-3 text-sm text-muted-foreground">
                            No telemetry event received yet.
                          </div>
                        )}
                      </div>

                      <Separator className="my-4" />

                      <div className="flex flex-col gap-3 xl:flex-row xl:items-start">
                        <div className="min-w-0 flex-1">
                          <div className="text-sm font-medium">Message board</div>
                          <textarea
                            className="mt-2 min-h-20 w-full resize-none rounded-md border bg-background px-3 py-2 text-sm outline-none transition-colors placeholder:text-muted-foreground focus:border-ring focus:ring-2 focus:ring-ring/20"
                            maxLength={240}
                            onChange={(event) =>
                              setMessageTextByDevice((current) => ({
                                ...current,
                                [device.deviceId]: event.target.value,
                              }))
                            }
                            placeholder="Send a short message to this device"
                            value={messageTextByDevice[device.deviceId] ?? ""}
                          />
                        </div>
                        <div className="flex flex-col gap-2 xl:w-48">
                          <Button
                            disabled={
                              !messageTextByDevice[device.deviceId]?.trim() ||
                              sendingDeviceId === device.deviceId
                            }
                            onClick={() => sendMessage(device.deviceId)}
                            type="button"
                          >
                            Send message
                          </Button>
                          <div className="text-xs text-muted-foreground">
                            {device.pendingMessage
                              ? "Waiting for device pickup"
                              : device.messages[0]
                                ? `Last sent ${formatRelativeTime(
                                    device.messages[0].createdAt,
                                  )}`
                                : "No messages sent"}
                          </div>
                        </div>
                      </div>
                    </CardContent>
                  </Card>
                ))}
              </div>
            )}
          </div>
        </section>
      </div>
    </main>
  );
}

function Metric({ label, value }: { label: string; value: string }) {
  return (
    <div>
      <div className="text-xs text-muted-foreground">{label}</div>
      <div className="mt-1 truncate font-medium">{value}</div>
    </div>
  );
}

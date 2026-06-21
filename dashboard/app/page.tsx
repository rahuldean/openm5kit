import {
  Cable,
  ChevronDown,
  Circle,
  Cpu,
  Database,
  LayoutDashboard,
  MessageSquareText,
  Network,
  Radio,
  Search,
  Settings,
  Sparkles,
  Terminal,
  UploadCloud,
} from "lucide-react";

import { Badge } from "@/components/ui/badge";
import { Button } from "@/components/ui/button";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { Separator } from "@/components/ui/separator";

const navItems = [
  { label: "Overview", icon: LayoutDashboard, active: true },
  { label: "Signals", icon: Radio },
  { label: "Prompts", icon: MessageSquareText },
  { label: "LiteLLM", icon: Network },
  { label: "I/O", icon: Cable },
  { label: "Settings", icon: Settings },
];

const statCards = [
  { label: "Incoming signals", value: "0", icon: Radio },
  { label: "Queued pushes", value: "0", icon: UploadCloud },
  { label: "AI routes", value: "0", icon: Sparkles },
  { label: "Firmware targets", value: "2", icon: Cpu },
];

const roadmapItems = [
  "Capture device events and sensor readings",
  "Send prompts and commands to devices on demand",
  "Route AI requests through your LiteLLM proxy",
  "Display response state on M5Stick and CoreS3",
];

export default function DashboardPage() {
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

            <div className="flex items-center gap-2">
              <Button className="hidden sm:inline-flex" variant="outline">
                <Search className="h-4 w-4" />
                Search
              </Button>
              <Button>
                Workspace
                <ChevronDown className="h-4 w-4" />
              </Button>
            </div>
          </header>

          <div className="flex-1 space-y-6 p-4 sm:p-6">
            <div className="flex flex-col gap-3 sm:flex-row sm:items-center sm:justify-between">
              <div>
                <div className="flex items-center gap-2">
                  <h2 className="text-xl font-semibold tracking-normal">
                    AI Device Plane
                  </h2>
                  <Badge>Local</Badge>
                </div>
                <p className="mt-1 text-sm text-muted-foreground">
                  Receive data from M5 devices and push AI responses back on demand.
                </p>
              </div>
              <Button>
                <Sparkles className="h-4 w-4" />
                Configure AI route
              </Button>
            </div>

            <div className="grid gap-4 sm:grid-cols-2 xl:grid-cols-4">
              {statCards.map((stat) => (
                <Card key={stat.label}>
                  <CardHeader className="flex flex-row items-center justify-between space-y-0 pb-2">
                    <CardTitle className="text-sm font-medium text-muted-foreground">
                      {stat.label}
                    </CardTitle>
                    <stat.icon className="h-4 w-4 text-muted-foreground" />
                  </CardHeader>
                  <CardContent>
                    <div className="text-2xl font-semibold">{stat.value}</div>
                  </CardContent>
                </Card>
              ))}
            </div>

            <div className="grid gap-4 xl:grid-cols-[1fr_360px]">
              <Card>
                <CardHeader>
                  <CardTitle>Device Data</CardTitle>
                  <CardDescription>No incoming signals yet</CardDescription>
                </CardHeader>
                <CardContent>
                  <div className="flex min-h-[300px] items-center justify-center rounded-md border border-dashed bg-muted/30">
                    <div className="max-w-sm text-center">
                      <div className="mx-auto flex h-11 w-11 items-center justify-center rounded-md border bg-background">
                        <Database className="h-5 w-5 text-muted-foreground" />
                      </div>
                      <h3 className="mt-4 text-sm font-medium">
                        No device data received
                      </h3>
                      <p className="mt-2 text-sm text-muted-foreground">
                        Sensor readings, button events, and AI request context will appear here.
                      </p>
                    </div>
                  </div>
                </CardContent>
              </Card>

              <Card>
                <CardHeader>
                  <CardTitle>Next Work</CardTitle>
                  <CardDescription>Planned AI device workflow</CardDescription>
                </CardHeader>
                <CardContent className="space-y-3">
                  {roadmapItems.map((item) => (
                    <div
                      className="flex items-start gap-3 rounded-md border bg-background p-3"
                      key={item}
                    >
                      <Circle className="mt-1 h-3 w-3 fill-muted text-muted" />
                      <div>
                        <div className="text-sm font-medium">{item}</div>
                        <div className="text-xs text-muted-foreground">
                          Not implemented yet
                        </div>
                      </div>
                    </div>
                  ))}
                </CardContent>
              </Card>
            </div>
          </div>
        </section>
      </div>
    </main>
  );
}

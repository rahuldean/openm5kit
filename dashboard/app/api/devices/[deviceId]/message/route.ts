import { NextResponse } from "next/server";

import { consumePendingMessage, queueMessage } from "@/lib/device-store";

type RouteContext = {
  params: Promise<{
    deviceId: string;
  }>;
};

export async function GET(_request: Request, context: RouteContext) {
  const { deviceId } = await context.params;
  const message = consumePendingMessage(deviceId);

  if (!message) {
    return new Response(null, { status: 204 });
  }

  return new Response(message.text, {
    headers: {
      "content-type": "text/plain; charset=utf-8",
    },
  });
}

export async function POST(request: Request, context: RouteContext) {
  const { deviceId } = await context.params;
  const payload = (await request.json()) as { text?: unknown };
  const text = typeof payload.text === "string" ? payload.text.trim() : "";

  if (!text) {
    return NextResponse.json(
      { ok: false, error: "Message text is required." },
      { status: 400 },
    );
  }

  const message = queueMessage(deviceId, text.slice(0, 240));

  if (!message) {
    return NextResponse.json(
      { ok: false, error: "Device is not registered." },
      { status: 404 },
    );
  }

  return NextResponse.json({
    ok: true,
    message,
  });
}

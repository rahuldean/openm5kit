import { NextResponse } from "next/server";

import { recordEvent } from "@/lib/device-store";

export async function POST(request: Request) {
  const payload = (await request.json()) as Record<string, unknown>;
  const event = recordEvent(payload);

  return NextResponse.json({
    ok: true,
    event,
  });
}

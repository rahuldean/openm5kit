import { NextResponse } from "next/server";

import { recordHello } from "@/lib/device-store";

export async function POST(request: Request) {
  const payload = (await request.json()) as Record<string, unknown>;
  const device = recordHello(payload);

  return NextResponse.json({
    ok: true,
    claimed: false,
    device,
  });
}

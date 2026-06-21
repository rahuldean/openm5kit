import { NextResponse } from "next/server";

import { listDevices } from "@/lib/device-store";

export function GET() {
  return NextResponse.json({
    devices: listDevices(),
  });
}

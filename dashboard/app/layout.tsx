import type { Metadata } from "next";
import "./globals.css";

export const metadata: Metadata = {
  title: "OpenM5Kit Dashboard",
  description: "AI device control plane shell for OpenM5Kit.",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en">
      <body>{children}</body>
    </html>
  );
}

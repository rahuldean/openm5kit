# Setup

OpenM5Kit is designed for Arduino IDE and M5Stack devices supported by the M5Stack ESP32 board package.

## Arduino IDE

Install Arduino IDE 2.x from the official Arduino website if it is not already installed.

## Board Package

Open Arduino IDE settings and add this URL to **Additional Boards Manager URLs**:

```text
https://static-cdn.m5stack.com/resource/arduino/package_m5stack_index.json
```

Then open **Boards Manager**, search for `M5Stack`, and install the M5Stack board package.

## Libraries

Open **Library Manager** and install:

- `M5Unified`
- `M5GFX`

## Local Development

You can open the sketches directly from this repository:

```text
examples/M5Stick/M5Stick.ino
examples/CoreS3/CoreS3.ino
```

Each example includes the shared source through repo-relative paths, so direct upload from Arduino IDE works without installing OpenM5Kit as a library.

For the standard Arduino library workflow, place or symlink this repository into your Arduino libraries directory:

```text
~/Documents/Arduino/libraries/OpenM5Kit
```

After that, restart Arduino IDE. The target sketches should appear under:

```text
File > Examples > OpenM5Kit
```

# Deployment

OpenM5Kit has one deployable Arduino sketch per target.

## M5Stick

Open:

```text
examples/M5Stick/M5Stick.ino
```

Then choose the board that matches your exact hardware:

- `M5Stick-C`
- `M5Stick-C-Plus`
- `M5Stick-C-Plus2`
- `StickS3`, if your device is the S3 Stick

Select the connected serial port, then click **Verify** and **Upload**.

The sketch is designed to work when opened directly from this repository in Arduino IDE.

## CoreS3

Open:

```text
examples/CoreS3/CoreS3.ino
```

Select:

```text
M5Stack-CoreS3
```

Select the connected serial port, then click **Verify** and **Upload**.

The sketch is designed to work when opened directly from this repository in Arduino IDE.

## Deployment Checklist

Use this checklist each time you switch devices:

```text
1. Connect one device over USB.
2. Open the matching sketch from examples/.
3. Select the matching board.
4. Select the serial port.
5. Verify the sketch.
6. Upload the sketch.
7. Confirm the OpenM5Kit screen appears.
```

## Shared Code

Common behavior lives in `src/`. Device-specific sketches should stay small and pass the correct device profile into the shared app runtime.

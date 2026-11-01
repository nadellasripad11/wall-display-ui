# Week 4 wall display — schematic, HALF 1 of 2 (power chain only)

Status: HALF 1 DONE, HALF 2 NOT STARTED. Do not treat this as a complete
schematic. Half 2 (display/touch panel, board I/O, mount/frame) is blocked
until Sripad approves it after seeing Half 1.

## Half 1 components (power chain only)

| Ref | Part | Rating / fact | BOM price (sale) |
|---|---|---|---|
| PS1 | Essager 20W GaN USB-C charger, US plug | Output 5V/3A (also 9V/2.22A, 12V/1.67A — use 5V only) | $4.55 |
| W1 | UGREEN USB-C cable, 2m, braided | 60W, 3A | $5.37 |
| SW1 | Inline USB-C on/off switch cable, 30cm | 3A, inline ON/OFF button | $1.09 |
| U1 | Waveshare ESP32-S3 7-inch Display Board Type B, Touch | USB-C power in, 5V, 350mA draw, 1024x600, GT911 touch (touch wiring is Half 2) | $38.99 |

## Half 1 net list (power only)

- WALL (US mains) -> PS1 input
- PS1 USB-C output, 5V -> W1, pin VBUS
- W1 -> SW1, VBUS in
- SW1, VBUS out (switch ON = connected, OFF = open) -> U1 USB-C, VBUS
- U1 USB-C, GND -> SW1 GND -> W1 GND -> PS1 GND (return path, same chain)
- No other nets in Half 1. No display, touch, I2C, Wi-Fi, or mount nets yet.

## Half 1 checks (arithmetic, not a hardware test)

- Board draw: 0.35A at 5V = 1.75W.
- Charger headroom: 3.00A available / 0.35A draw = 8.6x. Any existing 5V
  charger rated 1A+ is also electrically enough (1.00A / 0.35A = 2.9x).
- Cable headroom: 3A rating / 0.35A draw = 8.6x.
- Switch headroom: 3A rating / 0.35A draw = 8.6x.
- Voltage rule: feed U1 with 5V only. Never feed it from a 12V output.
- Half 1 total (power chain + board): $38.99 + $4.55 + $5.37 + $1.09 = $50.00
  at sale prices, before shipping/tax. Mount/frame/strips are Half 2 /
  BOM-decision territory, not in this half.

## Diagram

See week4-schematic-half1.png in the same folder: wall -> charger -> cable
-> switch -> board, with 5V and GND labelled. Rendered from the net list
above, not from an EasyEDA file. No EasyEDA schematic exists yet for Week 4.

## Half 2, NOT done (needs Sripad's approval)

- Board display panel and GT911 touch wiring / I2C
- Board I/O, buttons (BOOT/RESET), TF slot note (SD dropped from BOM)
- Mount/frame + adhesive strips mounting, cable routing to the wall/outlet
- Any enclosure/frame dimensions once a frame is picked

# Hardware — pinout and power (from the Waveshare wiki pinout, read 2026-10-30)

Board: Waveshare ESP32-S3 7-inch Display Development Board Type B,
SKU 31726, Onboard Antenna + Touch Function. 1024x600 IPS, outline
192.96 x 110.76 mm, weight 0.369 kg, draw 5V / 350 mA. Package has the
board, HY2.0 cables, and a screws pack — NO cable, adapter, case,
switch, SD card, or power bank. NOT ordered.

## Verified pinout (manufacturer wiki)

- Touch: SDA GPIO8 / SCL GPIO9 / IRQ GPIO4, TP_RST via IO-expander EXIO1
- RGB: VSYNC GPIO3 / DE GPIO5 / PCLK GPIO7 / HSYNC GPIO46
- RGB data: GPIO0, 1, 2, 10, 14, 17, 18, 21, 38, 39, 40, 41, 42, 45, 47, 48
- Backlight: DISP via EXIO2, LCD_VDD_EN via EXIO6
- I2C addresses: 0x24 (IO expander), 0x5D (touch)
- SoC: ESP32-S3-WROOM-1-N16R8, 16 MB Flash / 8 MB PSRAM, Wi-Fi + BLE 5
- TF slot, USB-C (power + UART), BOOT/RESET buttons, 3.7V battery
  PH2.0 header (UNUSED — wall power only)
- NO speaker/amp/mic. NO camera connector/interface listed for the 7B
  (31726 = touch, 31727 = non-touch). Other Waveshare boards' 24-pin
  camera port does NOT apply. Do NOT order an OV2640.

## Power chain (schematic half 1)

WALL -> Essager 20W GaN charger (5V/3A) -> UGREEN 2m USB-C cable (3A)
-> inline USB-C switch cable (3A) -> board USB-C (5V, 350 mA).
Board draw 0.35A at 5V = 1.75W. Charger/cable/switch headroom 8.6x.
Any existing 5V 1A+ charger is electrically enough. Feed 5V only,
never 12V. Switch stays outside the holder, reachable by hand.

## Flat wall holder (decided 2026-10-30)

Strips go on a flat protective holder/frame, NEVER directly on the board.
Holder must clear the board outline, keep touch fully open, keep
USB-C/buttons accessible, and let the board lift out. Holder product /
dimensions / price are still undecided and NOT in BOM v2.
No hanging mount. No soldering anywhere — plug-in / pre-soldered
parts only.

## Diagrams (manual upload — PNGs are not pushed via the text route)

- schematic-half1.png: wall -> charger -> cable -> switch -> board
- schematic-half2.png: board blocks + holder + cable route
- Board front, board dimensions, system overview, flat wall:
  real product photos, in the project journal image sets.

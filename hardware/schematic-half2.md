# Week 4 Schematic — HALF 2 (done 2026-10-30, local only, NOT pushed)

Half 1 (done 2026-10-28) = power chain only: WALL → charger → cable → switch → board USB-C, 5V, 350 mA draw.
Half 2 = everything else on the board and how it sits on the wall. No hanging mount.

## Key fact that shapes Half 2
The Waveshare ESP32-S3 7-inch Type B board is one integrated board: the 1024×600
display panel and the GT911 touch panel are ON the board, not separate modules
to wire up. So Half 2 is a board block diagram, not a netlist of separate
display/touch parts. No separate panel wiring is invented here.

## Blocks

- U1 — Waveshare ESP32-S3 7-inch Type B, Touch Function, 1024×600, GT911 touch.
  - Display: integrated panel, 1024×600, 170° viewing, backlight adjustable.
  - Touch: integrated GT911 5-point capacitive touch via I2C, verified
    from the Waveshare wiki pinout (2026-10-30): TP_SDA GPIO8, TP_SCL GPIO9,
    TP_IRQ GPIO4, TP_RST via IO-expander EXIO1. I2C addresses on the board:
    0x24 (IO expander) and 0x5D (touch).
  - Display RGB pins, verified from the same wiki pinout: VSYNC GPIO3,
    DE GPIO5, PCLK GPIO7, HSYNC GPIO46, plus R/G/B data pins GPIO0/1/2/21/
    38/39/40/41/42/45/47/48 and blue pins GPIO10/14/17/18/38. Backlight:
    DISP via EXIO2, LCD_VDD_EN via EXIO6.
  - SoC: ESP32-S3-WROOM-1-N16R8, 16 MB Flash / 8 MB PSRAM, Wi-Fi + BLE 5.
  - USB-C (power): from Half 1, 5V only, never 12V.
  - USB-C (UART): second USB-C port on the board, for programming / serial.
  - BOOT button + RESET button: on the board, used for programming / reset.
  - Indicators: POWER, CHARGE, DONE LEDs on the board.
  - Headers present but UNUSED in v1 firmware: CAN, RS485, Sensor, I2C header
    (touch uses its own touch interface, not this header, unless the board
    definition says otherwise), UART1/UART2 selection switch.
  - TF card slot: present on the board, UNUSED in v1 — photos storage / SD
    was dropped, photos are internal flash only in v1.
  - Battery header: 3.7V lithium PH2.0 header present, UNUSED — power is
    wall cable only, power bank dropped.

## Flat wall holder — strips NOT directly on the board (Sripad, 2026-10-30)
- The board does NOT get strips stuck directly on it. Strips would cover
  ports/buttons, block heat, and risk damage on removal.
- Instead: a flat protective holder / frame holds the board. The holder:
  - must clear the board outline 192.96 × 110.76 mm, with the touch screen
    fully open (no glass or cover over the touch area),
  - must leave the USB-C power port, BOOT/RESET buttons, and TF slot
    accessible, or at least not permanently covered,
  - must have a flat back area for the strips to grip.
- The strips (Hyper Tough, 12 pairs, 5 lbs per set in the BOM) stick the
  HOLDER to the wall, not the board to the wall. The board lifts out of
  the holder for service.
- Holder material / exact product is NOT decided yet — it is not in the
  $56.28 BOM v2 total. Do not claim a holder price or fit until one is
  picked and its internal size + weight are verified. Total board weight
  alone is 0.369 kg; holder + board weight together must stay within
  what the strips can hold, with a flat contact area.

## Cable routing
- Wall outlet → charger → 2m UGREEN cable → inline switch cable → board
  USB-C power port. Switch stays outside the holder, reachable by hand.
- Cable runs down from the holder to the outlet; it is not trapped behind
  the holder where it would pinch or pull the board off the wall.
- Outlet distance is still unconfirmed — the 2m cable length is only
  confirmed once the wall and outlet distance are picked.

## What Half 2 does NOT claim
- No exact GT911 GPIO pin numbers, no exact holder dimensions or price,
  no verified fit for any holder, no data sources (weather location,
  stocks tickers, calendar source are still undecided and are firmware
  configuration, not schematic).
- Firmware v1 (separate file) is a skeleton: home screen, on-display
  split / single full screen choice, and placeholder app screens.
  Live data apps need their sources picked first.

## Files
- This doc: week4-schematic-half2.md
- Diagram: week4-schematic-half2.png (board blocks + holder + cable route)
- Half 1: week4-schematic-half1.md / .png

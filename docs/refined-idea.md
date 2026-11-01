# Week 4 — Double-sided hanging display — refined idea

Status: DRAFT for Sripad's tap, 2026-10-26. Nothing ordered, posted, or submitted.
Locked from Oct 25 (do not re-open): black capsule on a wall bracket, each side
independent, portfolio app cut, cable primary + power bank backup + on/off switch.
2026-10-26 update: Sripad prefers ROUND displays over rectangular. Rectangular
2.8-inch ILI9341 spec below is superseded; the round spec in the next section
is the current draft, still awaiting his approval to start.
Size constraint, 2026-10-26: about 10 cm or smaller, NOT tiny like 2 cm — he
wants BIGGER. Drop 1.28-inch options entirely. Target 2.8-inch (~7 cm) to
4-inch (~10 cm) round; 2.1-inch (~5 cm) is the minimum acceptable, not the
target.

## The refined idea, in one line

A black, double-sided hanging capsule, mounted perpendicular in a hallway or
doorway, so one face greets traffic coming one way and the other face greets
traffic coming the other way. Each face is its own touch screen, running its
own app, changed only by touching that face.

## Why it fits Displays Week

The displays are the project, not a decoration on a clock. Two electronic
touch TFTs, each driving a different live app, is the whole point. It is not
an analog clock with a bought movement, and it is not a mirrored pair of
screens showing the same thing.

## Recommended spec, ROUND version (defaults — veto any line)

- Screens: two 2.1-inch round touch displays, 480x480, ST7701 driver,
  capacitive touch (Waveshare ESP32-S3-Touch-LCD-2.1 / LILYGO T-RGB class).
  Outer diameter about 75 mm per face, inner display about 53 mm, so the
  device becomes a round disc, not a rectangular capsule — closer to the
  original wall-clock look.
- Architecture change vs the rectangular draft: each face is its own
  ESP32-S3 board with its display attached, not one brain driving two
  screens. That actually fits "each side independent" perfectly — no
  shared-SPI juggling, each side gets its own Wi-Fi/NTP — but it means
  two boards, two firmwares (same code, different default app), and
  double the display cost.
- Firmware change: ST7701 uses an RGB interface, not the simple SPI of
  ILI9341, so the display code runs on LVGL-style graphics with PSRAM.
  More firmware lift than the rectangular route, and the UI must be
  designed for a circle: clock is natural, stocks works as a 2x2 grid or
  one big ticker at a time, not a rectangular list.
- Rejected: 1.28-inch round (Waveshare DualEye class, 240x240). Too
  cramped for stocks text, and a retail listing put the dual board at
  $52.86. Open alternative: 2.8-inch round, also 480x480, bigger face and
  easier stocks reading, but a larger disc and a price not verified yet.
- Superseded rectangular route: two 2.8-inch ILI9341 modules + one
  ESP32-S3. Cheaper and simpler firmware, but Sripad prefers round.
- Touch: tap anywhere on a face cycles that face forward through the apps.
  Tap on one side never changes the other side. No app-picker menu in v1 —
  that keeps the firmware and the touch handling much simpler. A tiny row
  of dots shows which app that face is on.
- Boot defaults: clock on one face, stocks on the other, exactly like the
  locked finished-product render.

## Apps, and what each one actually does

1. Clock — big time, date, day. Synced over Wi-Fi (NTP).
2. Stocks — SPY, AAPL, NVDA, TSLA, price plus day change. Live feed in
   build week; mock data first so the display code is testable offline.
3. Weather — current temperature, condition, high/low. Open-Meteo is the
   proposed source because it needs no API key.
4. Calendar — date, weekday, and today's agenda view. The exact calendar
   source is still open; v1 can show date/weekday and a mock agenda.
5. Timer — countdown and stopwatch. Tap to start/stop, long press to reset.
6. Photos — slideshow, tap to advance to the next photo. Storage (SD card
   vs internal flash) to be decided with the final module pick.

## Power, refined

Wall outlet, via USB-C, is primary. A USB power bank is backup, not the
main supply. If the bank supports pass-through charging, the chain is
wall -> bank -> device, and the bank acts like a UPS. If it does not,
the fallback is manual switch-over: unplug from wall, plug into the bank.
That is why the first check is: what bank does he already own, and does
it do pass-through? Cable runs in a slim raceway or along a wall corner
to the nearest outlet. On/off switch stays in the circuit.

## Placement, still open

Pictured plan, not a measured site: hallway or doorway, mounted
perpendicular, doors at both ends, one face each way. Still needed: the
exact spot, and the distance from that spot to the nearest outlet and
to reliable Wi-Fi. No spot is confirmed yet.

## What we need to do this week (design week, in order)

1. Lock this spec — tap the defaults above, or veto a line.
2. Create the Half Life project. No submit tap yet.
3. Build the BOM with real checkout prices only, plus cart screenshots
   with the total visible. No price is claimed in this draft.
4. Draw the schematic in EasyEDA: ESP32-S3, two TFTs on shared SPI,
   two touch controllers, power in, on/off switch. Render it.
5. Write the firmware skeleton: both displays up, tap-to-cycle per face,
   clock over NTP working, other apps on mock data first.
6. README in Sripad's own words, with what/why/how, the BOM table, and
   at least 2 of his own images (schematic renders, not AI graphics —
   AI graphics cannot go in the final repo).
7. Journals and reels, Week 3 pattern: idea reel, progress entries,
   closing reel panning the schematic/layout/render with narration.
8. Run the submit wizard (repo checks, BOM/tier, reel) and stop before
   submit. Sripad taps submit himself.

Time estimate carried from Oct 25: roughly 15-20 hours for design week,
vs 10 hours for Week 3, with the app-switcher firmware as the bulk of it.

## Not verified yet — do not treat as fact

- Week 4 close date: not verified from an official source.
- Week 4 tier and tier hours: not verified. Do not assume Week 3's $30.
- Any checkout price for the screens, ESP32-S3, bank, or enclosure parts.
- Whether Sripad already owns a pass-through power bank.
- The exact hallway/doorway spot and outlet distance.

Next verification step, after the spec tap: check the Half Life dashboard
for the Week 4 close date and tier, then price the BOM at real checkout.

## Tier decision, 2026-10-27 evening — bank dropped, Tier 3

Sripad dropped the power bank from the Week 4 BOM and set Tier 3 ($100)
as the target. Power is wall cable only, switch kept, no backup.
Sale-price BOM before shipping/tax: board $38.99 + charger $4.55 + cable
$5.37 + switch $1.09 + mount $36.31 + strips $6.28 = $92.59, leaving
$7.41 to the $100 cap ($10.41 to the $103 allowance) before shipping/tax.
At list prices the same set is $124.79, so the checkout total at
submission time is what counts, and shipping/tax across vendors is still
unverified. Nothing ordered.

## Board facts verified, 2026-10-27 evening

Waveshare ESP32-S3 7-inch Touch LCD Type B: outline 192.96 x 110.76 mm,
weight 0.369 kg, USB-C power. Box is board + HY cables + screws pack only:
no USB cable, wall adapter, case, switch, or bank. Power/mount/sticky/
switch/backup shortlist (AliExpress only, Amazon blocked to the browser):
~/workspace/shopping-results/week4-power-mount.json — mount fit NOT
exact-verified (7-13 inch rating, no mm clamp range published), UPS
backup is pricey and its main output is 12V, so only its Type-C output
may feed the 5V board. Corrected 2026-10-27: foam tape roll dropped per
Sripad, replaced by pre-cut strips; cheaper UPS found at $52.08 (18000mAh
variant, $40.19 for 10400mAh) but it is a mini-UPS needing a separate
9-12V adapter, and no conventional pocket bank under $71.78 with verified
pass-through wording was found. Nothing ordered.

## Concept framing, 2026-10-27 evening — "iPhone but bigger"

Sripad's framing: the display is basically like an iPhone, but bigger, on
the wall, with all the apps on it. Clarification that matters for the UI:
it is NOT an iPhone and NOT iOS — there is no App Store to download from.
The six apps (clock, stocks, weather, calendar, timer, photos) are built
into the firmware on the ESP32-S3 board. The iPhone-like part is the
experience: a touch home screen with app icons, tap an icon to open that
app, on a 7-inch wall screen, Wi-Fi only, powered by cable/bank, not a
handheld phone battery.

## Pick, 2026-10-27 evening — single-sided, NOT ordered yet

Sripad picked: Waveshare ESP32-S3 7-inch Display Development Board Type B,
1024x600, Touch Function variant (Onboard Antenna + Touch), $38.99, quantity 1.
This supersedes the double-sided plan. Checkout total with shipping/tax is
still unverified and no purchase has been placed; that total must be shown
for his tap before any order.

## Cheapest options found, 2026-10-26 (browser product search)

Still ON HOLD — nothing ordered, no approval tap yet. Shipping and tax not
fully verified, except where noted. Amazon was not searchable (AI-agent wall),
so these are Waveshare + AliExpress only.

- Round, preferred size: Waveshare ESP32-S3 2.8-inch round touch board
  (ESP32-S3-Touch-LCD-2.8C, With Touch), direct from Waveshare, $39.99 each,
  $79.98 for 2, before shipping/tax. 480x480 IPS, ESP32-S3 onboard, GT911
  capacitive touch, 16MB Flash, 8MB PSRAM. Same board via AliExpress
  (JY TEC Store) was $46.79 + $7.71 shipping, worse value.
- Round, minimum size: Waveshare ESP32-S3 2.1-inch round touch board,
  $32.99 each, $65.98 for 2, before shipping/tax. Only the minimum size,
  not the preferred larger size.
- Rectangle fallback: AliExpress ILI9341 2.8-inch touch module
  (Suzpng Limited Store), $8.09 each, $16.18 for 2 modules, before a
  separate ESP32-S3 board per face and before $5.75 standard shipping.
  Module only, not a complete board.
- Larger round found but over budget: Waveshare ESP32-P4 3.4/4-inch round
  touch board at $64.99-$74.99, and the 4-inch variant is marginally over
  the 10 cm cap, so not relied on.

# Week 4 — Full product package (2026-10-30, local only, NOT pushed)

## What the product is
A big wall display you can tap. It shows apps, all on one 7 inch touch
screen. It runs on an esp32-s3 board and plugs into the wall, so it stays
on and you can just glance at it. You can choose on the display itself,
either split screen or a single app full screen. (Outward text only —
no app list, no iPhone comparison.)

## What is DONE
- Idea reel: POSTED 2026-10-27, caption "wall display", dashboard 2/5
- Journal Entry 1: POSTED 2026-10-30, 15h, 15 images, dashboard Completed
- Schematic Half 1: power chain, wall → charger → cable → switch → board, 5V only
- Schematic Half 2: display/touch (verified Waveshare pins), board I/O,
  flat protective holder, cable routing — strips on the HOLDER, not the board
- Firmware v3: Apple-clean UI, home grid, segmented Split/Single control
  saved in Preferences, working Timer, Clock (NTP once Wi-Fi is set via
  Serial, uptime otherwise), Settings (brightness, Wi-Fi status), Split
  screen (Timer + Clock, both live)
- Firmware compile-verified 2026-10-30: ESP32 core 3.3.12, LovyanGFX
  1.2.36, LVGL 9.2.3, 1,268,655 bytes (40% of 3MB app), 0 errors, with
  Flash 16MB + 3MB APP partition + PSRAM OPI
- BOM v2: $56.28 at sale prices before shipping/tax (board $38.99,
  charger $4.55, cable $5.37, switch $1.09, strips $6.28)

## What is NOT done / NOT claimed
- Hardware test: firmware has NOT been flashed or touch-tested on a board
- Live data: weather, stocks, calendar, photos sources not picked, so
  those tiles show "Not connected yet" — no fake data
- Backlight: slider may be a no-op until the IO-expander path is verified
- Holder: protective holder product/dimensions/price undecided, NOT in BOM v2
- Checkout totals: shipping/tax per vendor unverified, nothing ordered
- Wall/outlet: wall and outlet distance unconfirmed (confirms 2m cable)
- README, cover image, repo link on the project, BOM on the project:
  still to do before submit
- Closing reel: still to do
- Project submission: NOT submitted, closes Sun Nov 1, 11:59 PM ET
- Hours: 15h logged, 5h remain to Tier 3's 20h

## Files in this folder
- week4-schematic-half1.md/.png, week4-schematic-half2.md/.png
- firmware/wall-display.ino, firmware/README.md, firmware/lv_conf-notes.txt
- week4-single-sided-bom-corrected-draft-v2.md ($56.28, NOT pushed)
- week4-journal-entry-1-draft.md (posted version, 15h)
- week4-journal-entry-1-images/ (15 posted images)

Nothing here is pushed to GitHub or submitted to Half Life without
Sripad's approval-card tap.

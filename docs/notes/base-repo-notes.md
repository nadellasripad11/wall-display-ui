# Week 4 — base repo decision (Oct 31, 2026)

## Chosen base (legal to copy, with credit)
- **joeycatic/esp32-spotify-touch-controller** — https://github.com/joeycatic/esp32-spotify-touch-controller
- License: **MIT** (checked from cloned LICENSE, copyright 2026 joeycatic).
  MIT allows copy/modify/distribute if the copyright + license notice stays in
  any substantial copy. Keep that notice in the README if code is reused.
- Why it fits: built for **exactly our board** — Waveshare ESP32-S3-Touch-LCD-7B,
  SKU 31726, 1024x600 landscape, LVGL, touch, Wi-Fi. Spotify Connect playback,
  PKCE auth over USB serial (no client secret), token refresh, board detection.
- What to take from it: display/board init pattern (incl. the ESP-IDF note that
  Arduino's precompiled SDK hurts PSRAM/RGB DMA throughput), LVGL structure,
  Spotify PKCE + token refresh pattern, swipe/library UI pattern.
- What NOT to take: its whole app as-is. Our apps (Clock, Weather, Stocks,
  Tasks, Calculator, Calendar, Notes, Search, Timer, Photos) and our wallpaper
  stay ours.

## Runner-ups (checked, not chosen as base)
- **XCRYZER01/MacroDesk** — https://github.com/XCRYZER01/MacroDesk — Waveshare
  7" LVGL 8.4, but **GPL-3.0**. Copying it would make our firmware GPL too.
  Useful as a driver reference only, not as a copy base.
- **jan-tdy/jadivclock** — MIT, ESPHome/LVGL, weather via Open-Meteo (no key),
  tasks, clock. Good pattern reference, but ESPHome (not Arduino) and only
  3.5" panels supported — not a drop-in base for our 7" Arduino build.
- **Light-r4y/uaDASH** — https://github.com/Light-r4y/uaDASH — Waveshare 7"
  supported, LVGL 8.4 + LovyanGFX, but a car dashboard, not a home/apps UI.

## What does NOT exist (do not fake it)
There is no legal open repo that is Apple's Clock/Weather/Calendar or
Robinhood's app code. Those are closed, proprietary products. Any "exact copy"
of them would be their logos/assets/code, not open source. Our v19 screens are
the same *structure and functions*, drawn with generic glyphs, no logos.

## Half Life warning (important for submission)
Design weeks require most parts designed by you and AI coding help ~30% max.
Copying a whole repo verbatim and submitting it as your design risks
rejection. The safe route: credit the MIT base in the README, keep our UI
design and our apps, and use the base only for board/display/Spotify plumbing.

# Wall Display firmware v3 — FULL product firmware

- Sketch: wall-display.ino (LVGL 9 + LovyanGFX, Apple-clean UI)
- Board: Waveshare ESP32-S3 7-inch Type B, Touch Function, 1024x600
- UI: light #F5F5F7 background, white rounded cards, dark text, one
  blue accent, large clean type. Home grid, segmented Split / Single
  control on the display itself, saved in Preferences and restored
  on reboot. Timer (Start/Pause/Reset, live), Clock (NTP time once
  Wi-Fi is set, uptime otherwise — no fake time), Settings (brightness
  slider, Wi-Fi status), Split screen (Timer + Clock side by side).
- Wi-Fi: NO credentials in the code. Over Serial, send:
    WIFI <ssid> <password>
  The board saves them, connects, and syncs time via NTP.
- Live-data tiles: "Not connected yet" — weather, stocks, calendar,
  and photos sources are not picked yet, so no fake data is shown.
- Backlight: brightness slider calls setBrightness, but this board
  drives backlight via the IO expander (EXIO2), so verify on hardware.
- Holder: strips go on a flat protective holder, NOT directly on the
  board. Holder product/dimensions/price are NOT decided yet and are
  NOT in BOM v2 ($56.28 before shipping/tax).
- COMPILE STATUS, 2026-10-30: v4 VERIFIED, 0 errors. ESP32 core 3.3.12,
  LovyanGFX 1.2.36, LVGL 9.2.3, board settings Flash 16MB + Partition
  "16M Flash (3MB APP/9.9MB FATFS)" + PSRAM OPI: 1,391,375 bytes (44%
  of 3MB app). Warning: on the default 1.2MB partition v3 was 96% full,
  so use the 16MB partition above. Do not claim it runs on the board
  until it is flashed and touch-tested on hardware.
- REAL DATA v4: Open-Meteo weather (no key, Alpharetta default
  34.08/-84.29, verified live 2026-10-30: 69.7°F, 43% humidity), Stooq
  daily CSV last close for NU/SOFI/VTI (header must start with "Date",
  Stooq fetch timed out in browser check — CSV format from search
  results, NOT re-verified live on device). UPDATE 2026-10-30 20:01 ET:
  Stooq quote endpoint /q/l/?s=nu.us&f=sd2t2ohlcv&h&e=csv was verified
  to return 'The page you requested does not exist or has been moved'
  — that endpoint is dead/moved, do NOT switch to it. Daily CSV
  endpoint remains unverified (blank in browser, empty via curl)., NTP clock once Wi-Fi is
  set via Serial, ICS calendar URL via Serial, no fake data anywhere.
- NOT pushed to GitHub. Push needs Sripad's approval-card tap.
- lv_conf: see lv_conf-notes.txt (enable Montserrat 24/28/36/48).

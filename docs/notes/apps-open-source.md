# Week 4 — Open-source version for every app (18 apps)

Rule used: copy only MIT-licensed code, LICENSE kept intact. Reference-only = license
not verified / not MIT-compatible, so NOT copied, and NOT claimed as ours.
"Firmware" = our wall-display.ino, already compile-verified, not hardware-tested.
Board truth: Waveshare 7B has NO camera module and NO speaker/amp — see below.

| App | Open-source source | License / status | How it becomes functional on the 7B |
|---|---|---|---|
| Clock | tuct/esphome-lvgl-clock, files/base-lvgl-clock/ | MIT, copied | Analog/digital/flip/7-seg LVGL clock; firmware Clock + NTP already |
| Weather | jan-tdy/jadivclock, files/base-jadivclock/ | MIT, copied | Open-Meteo, no key; firmware Weather already, verified 69.7°F Oct 30 |
| Stocks | KerteszRoland/ESP32-lvgl-widgets, files/base-lvgl-widgets/ | MIT, copied | Stock widget, FMP API; firmware uses Stooq last-close + Finnhub slot (key from Sripad, never in repo) |
| Timer | tuct/esphome-lvgl-clock + LVGL | MIT, copied / built-in | Firmware Timer (Start/Pause/Reset) already; alarm visual only, no speaker |
| Tasks | jan-tdy/jadivclock (Todoist agenda) + firmware | MIT, copied | Firmware Tasks lists real open items; Todoist needs his Todoist account if used |
| Calc | El-EnderJ/NeoCalculator | REFERENCE ONLY, no LICENSE found — not copied | Firmware Calculator already works; LVGL keypad, no external code needed |
| Calendar | LVGL lv_calendar + bigbrainlabs/infohub, files/base-infohub-calendar/ | MIT, copied | lv_calendar month grid; InfoHub calendar-month pattern; firmware Calendar via ICS URL over Serial |
| Safari | — no full browser for ESP32-S3 | No ESP32 MIT browser copied | Search app instead: Wikipedia summary API, no key (firmware SEARCH) — not a real browser, do not claim it is |
| Google | Wikipedia search API, no key | Free API, not a repo | Same as Safari: SEARCH <term> over Serial, summary only |
| Notes | LVGL textarea + Preferences | MIT, built-in | Firmware Notes via Serial NOTE <text>; OpenTodoApp is web/server only, reference only |
| Music | — display only, by user decision 2026-10-31 | No playback, by design | Firmware Music screen now says 'display only'. Spotify base remains in files/ as reference, not required. |
| Photos | Dragos-Hategan/esp32-file-manager, files/base-esp32-file-manager/ | MIT, copied (file_manager component only, not the 172MB third_party bundle) | JPEG viewer from TF/SD card; board has a TF slot |
| Maps | OpenStreetMap tiles, no key | Built in firmware 2026-10-31 | fetch_maps(): lat/lon -> tile x/y at zoom 15, HTTP GET tile.openstreetmap.org, verifies HTTP 200, shows centre/zoom/tile/URL. PNG rendering in LVGL needs lv_png — tile photo rendering is NOT claimed yet. Centre via Serial: MAPS <lat> <lon>. Default Alpharetta 34.08/-84.29. |
| Mail | Gmail IMAP (imap.gmail.com:993) | Built in firmware 2026-10-31, user connects himself | fetch_mail(): LOGIN, SELECT INBOX, FETCH 1:5 headers, shows raw headers truncated. Credentials via Serial only: MAIL <address> <app-password> — never hardcoded, never in repo. Needs a Gmail app password (2-step account). |
| Camera | DROPPED 2026-10-31 | No camera, no extra boards | Removed from Home. No soldering, plug-in parts only. |
| Settings | fbiego/windows-10-mobile-lvgl settingsScreen + firmware | MIT, copied | Firmware Settings: brightness slider (backlight path unverified on hardware), Dark/Light/Tinted, Single/Split mode |
| Files | Dragos-Hategan/esp32-file-manager, files/base-esp32-file-manager/ | MIT, copied | SD browsing, sorting, text edit, image view, SNTP — ESP-IDF, needs porting to our Arduino/LovyanGFX build |
| Messages | fbiego/windows-10-mobile-lvgl Messaging + jadivclock messaging | MIT, copied, placeholders | WinMobile Messaging is a placeholder; Jadiv messaging needs Home Assistant. No SMS on the board without a phone link |

## What is actually functional today (firmware, compile-verified)
Clock, Weather (Open-Meteo), Stocks (last-close), Timer, Tasks, Calc, Calendar
(ICS/deadlines), Notes, Search, Settings, Split screen, Home (long-press Customize).
## What still needs his input / hardware
Finnhub key (Stocks live), Spotify connection (Music real playback), ICS URL,
Wi-Fi over Serial, DAC+speaker (local audio playback, not needed — Music is display-only),
IMAP account (Mail), OSM tile code (Maps).

## Firmware update 2026-10-31 (compile-verified)
wall-display.ino with Maps + Mail screens: 1,419,079 bytes (45%), globals 237,192 bytes (72%), 0 errors. Split mode is no longer a Home icon (Maps took that slot); Split is still set in Settings. Not hardware-tested.

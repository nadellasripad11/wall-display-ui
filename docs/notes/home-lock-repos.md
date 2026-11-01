# Week 4 — home + lock screen layouts to copy (Oct 31, 2026)

## Best copy candidate — home AND lock in one repo
- **fbiego/windows-10-mobile-lvgl** — https://github.com/fbiego/windows-10-mobile-lvgl
  MIT (cloned, LICENSE checked, copyright 2024 Felix Biego).
  LVGL 9.2, ESP32 + SDL2 desktop emulator, LovyanGFX.
  Has: homeScreen, appScreen, settingsScreen, newsScreen, plus a lock screen
  (in src/ui/ui.c, padlock asset in src/ui/images/ui_img_padlock_png.c),
  status bar, navigation bar, notification center, tiles.
  Caveat: it is a Windows 10 Mobile *style* layout (live tiles), not iOS
  style — take the home/lock screen structure, re-skin it to our iOS-style
  icons + torn-paper wallpaper.

## Lock screen specifically
- **Entropypiewholes/GhostESP-Hosyond-E32R35t-Port**
  https://github.com/Entropypiewholes/GhostESP-Hosyond-E32R35t-Port — GPL-3.0.
  Full LVGL UI with carousel/grid/list layouts, PIN lock screen with
  auto-lock, Apps Gallery launcher, 21 themes, SD asset packs.
  GPL means copying it would make our firmware GPL too — reference only
  unless you want that license.

## Home layout only
- **dandoingdev/HouseView** — https://github.com/dandoingdev/HouseView — MIT
  (cloned, LICENSE checked, 2026 dandoingdev).
  Glass-style widgets in swipeable pages that fit the screen, full-screen
  overviews/screensavers when idle, clock/date/calendar/weather/music/timer
  widgets. Catch: it is a Home Assistant web dashboard (JS), not ESP32
  firmware — copy the *layout* (pages, glass widgets, idle full-screen),
  not the code.
- **xavidop/dash** — https://github.com/xavidop/dash — MIT (cloned, 2026).
  Ambient wall display: rotating panels, dim clock at night, weather,
  calendar, markets, Spotify. Also web (Node/TS), not ESP32 — layout only.
- **MaxGramser/homeassistant_espscreen (Tessera)**
  https://github.com/MaxGramser/homeassistant_espscreen — AGPL-3.0.
  Tile home layout for 2.8–10.1" ESP32 panels incl. Waveshare, drag-and-drop
  editor, bedside clock page. AGPL — reference only unless you want AGPL.
- **NeoForge2/Streamdeck-ESP** — https://github.com/NeoForge2/Streamdeck-ESP — MIT.
  1024x600, 36 button/widget slots, drag-and-drop layout editor, resizable
  widgets. Catch: ESP32-P4 + ESPHome, not our ESP32-S3 Arduino build.

## Already in our files (home layout)
- jadivclock (MIT, copied): home screen with clock, date, greeting.
- base-spotify-controller (MIT, copied): 1024x600 cover-first layout.

## Rule
Keep each repo's LICENSE with any copy, credit it in the README, and keep
our wallpaper + iOS-style icons as the submission design.

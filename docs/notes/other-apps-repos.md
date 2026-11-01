# Week 4 — repos to copy for the other apps (Oct 31, 2026)

Already copied: base-spotify-controller/ (MIT, joeycatic, exact 7B board) = Music.

## Copy-ready (license checked by cloning)
- **Stocks + Clock widgets — KerteszRoland/ESP32-lvgl-widgets**
  https://github.com/KerteszRoland/ESP32-lvgl-widgets — MIT (cloned, LICENSE checked).
  LVGL 9.3, stock widget (FMP API, 30-day chart, green/red) + Clockify widget.
  Change: ticker TSLA -> NU/SOFI/VTI, FMP key -> Finnhub/Stooq pattern.
- **Weather + Clock + Tasks — jan-tdy/jadivclock**
  https://github.com/jan-tdy/jadivclock — MIT (cloned, LICENSE checked, 2026 Jan Dubovsky).
  ESPHome/LVGL, weather via Open-Meteo (no key, now + 12h + 6 days),
  Todoist agenda (tasks, reschedule), home clock/date/greeting.
  Caveat: ESPHome + 3.5" only — take the weather/task logic, not the board code.
- **Clock + Weather + Calculator + Calendar — survivorhao/esp32s3watch**
  https://github.com/survivorhao/esp32s3watch — NO LICENSE FILE FOUND.
  Do NOT copy verbatim until the author adds one. Reference only.
  Has: LVGL clock, weather, simple calculator (dual stacks, parentheses),
  calendar view, SD file browser, deep sleep.

## Reference only (not a drop-in copy)
- **brettdottech/info-orbs** — open-source ESP32 desk widget kit: weather,
  stock tickers, Nixie-style clocks. Good widget architecture reference.
- **El-EnderJ/NeoCalculator** — https://github.com/El-EnderJ/NeoCalculator —
  ESP32-S3 graphing/scientific calculator, LVGL 9.x. Overkill for our basic
  calculator, but a strong calculator-engine reference.
- **bigbrainlabs/infohub** — https://github.com/bigbrainlabs/infohub — MIT,
  clock/weather/calendar/reminders via Home Assistant. Needs HA + a Linux
  MicroPython client, not our Arduino board — pattern only.
- **OpenTodoApp/OpenTodo** — web/server todo app, NOT an ESP32 app.
  Do not copy it into firmware; its list layout is a UI reference only.

## Still missing (no good ESP32 repo found — keep ours)
- Notes, Search (Wikipedia, no key), Timer, Photos (TF card). These are
  already in our firmware and are simple enough that our code is the copy base.
- Calendar: LVGL itself has lv_calendar (MIT) — use that widget, not a repo.

## Rule for all of these
Keep each repo's LICENSE with any copy, credit it in the README, and keep
our Home/wallpaper/apps design as the submission design (Half Life: most
parts designed by you).

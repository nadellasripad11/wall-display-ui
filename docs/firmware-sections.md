# Firmware sections — wall-display-square.ino (current square copy)

One file, compiled as a sketch folder (see firmware/). Sections, in
build order. This map exists so each section can be reviewed separately;
the code itself is not split — splitting it would break the sketch.

1. Style tokens — Midnight Glass v2: C_BG 0x0B0E14, C_CARD 0x1A1F2B,
   C_SUB 0x9AA3B2, C_ACCENT 0x0A84FF, card radius 32 + 1px subtle edge,
   back button 120x64. Guestbook keeps its own gb_chrome (liquid chrome,
   simplified) and is untouched by these tokens.
2. Home clock — exact clock hero, face-colour preference (blue, green,
   lavender, white, black, orange, saved in Preferences), live NTP time.
   Home v3 (light Apple-style, design-previews/66-68) is the mockup
   direction; firmware home face is the blue panel today.
3. Home scroll pages — tileview, swipe Clock -> Widgets -> Apps, page
   dots, arrow fallback. Split view deleted, mode forced single.
4. Widgets page — huge date, temperature + high/low (Open-Meteo daily),
   small analog clock, time-based greeting, calendar tile (ICS only),
   manual profile row (tap to switch, no face recognition, no camera).
5. App grid — weather card (real Open-Meteo or empty state), app tiles,
   dock row. No FaceTime, no Safari as a real browser (Search is
   Wikipedia only). No home-controls tile.
6. Clock app — World Clock / Alarms / Stopwatch / Timer rows, Customize
   picker (Dial Style Colour, colour tiles, check mark).
7. Weather app — Open-Meteo current, high/low, humidity, wind.
8. Calendar app — November 2026 grid, today circled, ICS event rows only.
9. Timer app — step timer: black face, orange ring, huge countdown,
   Start/Pause/Reset (Reset = cancel), step label.
10. Tasks app — tap to mark done.
11. Calculator app — digits, + - * /, =, C, left-to-right.
12. Notes app — user text over Serial only (NOTE <text>).
13. Search app — Wikipedia search over Serial (SEARCH <query>).
14. Music app — DISPLAY ONLY, generic drawn art, example tracks, buttons
    change display only. No playback, no speaker, no Spotify.
15. Photos app — 4-photo grid, full view + digital clock, 1x/2x zoom.
    Neutral placeholders; no TF driver, so TF loading is not claimed.
16. Maps app — OpenStreetMap tile fetch, no key, Alpharetta default,
    centre over Serial (MAPS <lat> <lon>). Tile photo not rendered
    (needs lv_png) — fetch/location are real, photo render is not claimed.
17. Mail app — Gmail IMAP, credentials over Serial only
    (MAIL <address> <app-password>), never hardcoded.
18. Settings app — brightness, theme, Wi-Fi via Serial, Single app only
    (no Split row).
19. Recipe app — title, photo placeholder, up to 6 ingredients tapped
    to check off, Start step timer. Content over Serial only:
    RECIPE <title> | <ing1, ing2, ...>. No copied recipe text.
20. Photo clock app — full-panel placeholder, huge live time, tap cycles.
21. Profiles — PROFILE <n> <name> (n 1-6), saved in Preferences.
22. Guestbook, 8 steps — home, signature canvas, captured celebration,
    guest details, saved confirmation, replay, gallery, individual view.
    See docs/guestbook-spec.md.
23. Guestbook, 7 animations — ink-to-chrome, stamp press, liquid ripple,
    light-trace replay, card develops, chrome seal favorite, pad breathes.
    See docs/guestbook-spec.md. No generic confetti, no emoji rain.
24. Data functions — Open-Meteo, Stooq daily CSV last close, ICS,
    OSM tile fetch, Gmail IMAP, NTP. Live data only, empty states honest.
25. Serial commands — WIFI, ICS, SEARCH, ALARM, NOTE, MAPS, MAIL,
    FINNHUB, RECIPE, PROFILE. Nothing hardcoded, ever.

Build: ESP32 core 3.3.12, LovyanGFX 1.2.36, LVGL 9.2.3,
FlashSize=16M, PartitionScheme=app3M_fat9M_16MB, PSRAM=opi.
Latest verified (2026-11-01): 1,521,847 bytes (48%), globals 255,056
(77%, 72,624 left). Compile-only, NOT hardware-tested.

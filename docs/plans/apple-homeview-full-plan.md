# Full HomeView plan — every screen and feature in the reel, coded by us

Source: full reel watched 2026-11-01 (Brad Canning, 13 Oct 2026, "Apple HomeView Visuals", music only, no narration, no timecodes on the web player, frames sampled across multiple loops — very short screens or small labels could have been missed).
Hardware in the reel: square rounded-corner screen, thin black bezel, tabletop version tilted back on a short cylindrical speaker base with a perforated grille and a trailing cable; wall version flat/flush, frame black or light silver, small dot at top centre of the frame on one wall shot. No dimensions, ports, or buttons were legible.
Our hardware (unchanged): Waveshare ESP32-S3 7-inch, 1024x600 landscape, GT911 touch, Wi-Fi/BLE, TF slot, USB-C, no speaker, no mic, no camera, no HomeKit. UI is a 600x600 square panel centred on our board, black side bars — that is the size/look adaptation, not a physical square board.
Coding rule (Sripad, 2026-11-01): we code it ourselves, so it is our original implementation. That means: our own code, our own drawn glyphs, Inter/Montserrat fonts — no Apple logos, no Apple's SF font, no pixel-exact Apple assets, no copied album art, no copied personal event text. Feature ideas and layouts are implemented by us; assets are not lifted.

## Every screen in the reel, and what we build for each

1. Blue analog clock (opening screen)
   - Reel: pale blue face, huge 12/9/6/3 only, tick marks, black hour/minute hands, thin second hand, small date at bottom, "Customize" pill centred at bottom edge, small "+" at right edge, tap on face.
   - We build: FULL. Square blue clock, huge numerals, live NTP time, date bottom left, Customize pill, tap opens the picker. Already mocked as lock/home — this is the base screen.

2. Clock customization picker ("Dial style colour")
   - Reel: pale lavender background, small-caps heading, large analog face centre, row of small clock-face style tiles below, tap a tile, blue check mark at bottom centre on selection.
   - We build: FULL. Picker screen with face-colour tiles (blue, green, lavender, white, black, orange), tap a tile, check mark, selection saved in Preferences, clock face recolours everywhere. No "+" function copied — "+" in the reel is unexplained, so ours opens "add a custom colour" only if Sripad asks for it later.

3. Green analog clock (result of customizing)
   - Reel: same layout, green face, viewed at an angle on a table.
   - We build: FULL, as a consequence of (2) — green is one of the picker colours, not a separate screen.

4. Home app grid / dashboard
   - Reel: pink-to-purple gradient, weather widget top-right with large temperature, rounded app icon grid with labels, a music card with play button, a dock along the bottom, tap near the top.
   - We build: FULL, adapted. Gradient home grid in our square panel, weather widget (Open-Meteo, real data only), app grid with OUR apps (Clock, Weather, Calendar, Timer, Tasks, Calculator, Notes, Search, Music, Photos, Maps, Mail, Settings, Files, Messages). The reel's FaceTime/Safari icons are NOT copied as working apps: FaceTime cannot work on this board (no camera/mic, no Apple calling), Safari cannot be a real browser on ESP32-S3 (Wikipedia search only, as before). Those two are either omitted or shown as clearly display-only placeholders — Sripad to confirm which, default is omit, do not fake a working call/browser.
   - Dock: bottom row of most-used apps, same squircle style.

5. Calendar / weather / clock widget screen
   - Reel: pale blue, left side large weekday + huge date number with smaller calendar text, large temperature + weather text bottom left, right side small analog clock, tap near clock.
   - We build: FULL. Widget screen: huge date, temperature (Open-Meteo), small analog clock, tap clock opens Clock app, tap date opens Calendar, tap weather opens Weather. All values real or empty-state, never invented.

6. Calendar app, month view + day events
   - Reel: pale blue/grey, month header, weekday header, week dates with one date circled, event rows below with coloured bars and times, "+" button at right, tap an event.
   - We build: FULL, with real data only. Month grid (LVGL lv_calendar), today circled, event rows from the user's ICS calendar only (Serial ICS URL). If no ICS is set, the screen says "no calendar connected" instead of inventing events. "+" opens a note/task entry in our Notes/Tasks, not a fake calendar event. Times and titles in mockups are neutral placeholders, not the reel's personal events.

7. Recipe / text screen
   - Reel: pale almost-white background, paragraph of cooking text at top, small icons in top bar, finger on right edge. No app name legible.
   - We build: FULL, as Notes. A text screen that shows user-supplied text (via Serial NOTE command, already in firmware, or a note file). Mockups use a neutral example paragraph, not the reel's recipe text. No cooking backend, no recipe search — it is a text display, honestly labelled.

8. Music player
   - Reel: colourful album art, song title + artist, previous/pause/next, progress bar with elapsed/remaining, room selector pill ("Bedroom" + speaker icon), queue/list icons.
   - We build: DISPLAY ONLY (standing decision, board has no speaker). Player UI with generic drawn art (no copied album art, no real artist/track art), title/artist as layout placeholders, prev/pause/next buttons that change the displayed track only, progress bar as a display, room pill as a label only. No playback, no Spotify, no audio claim. If Sripad later adds a speaker/DAC board, that is a new hardware phase, not this plan.

9. Timer, cooking-step style
   - Reel: black background, orange progress ring/ticks, "Step 2" small at top, huge white countdown centre, "MIN" below, pause button bottom right, cancel X bottom left, tap centre.
   - We build: FULL. Timer screen in the square panel: black face, orange ring, huge countdown, step label (from the current timer's label, or "Timer" if none), pause + cancel controls, both working in firmware. This replaces the old plain timer screen.

10. Home controls
    - Reel: blue-gradient background, white translucent tiles. Large thermostat tile (temperature + room + up/down controls), small analog clock tile, scene tile ("I'm Leaving" + person icon), and smaller tiles for door lock, ceiling fan, and intercom, all tapped.
    - We build: TILES FULL AS UI, CONTROL AS LOCAL STATE ONLY (honest limit). Thermostat, scene, door, fan, and intercom tiles are built and tappable, and tapping toggles the tile's displayed state on-device. They do NOT control a real home on this board: no HomeKit, no smart-home hub, no devices connected. A future phase could add MQTT/Home Assistant control, but that needs a hub, device list, and credentials from Sripad — it is not claimed in this plan. Intercom tile is display-only (no mic/speaker, no live call).

11. Photo + digital clock display
    - Reel: full-screen portrait photo, large translucent digital clock top left, small weather/status icon, tap top right. Photo-display / lock style, not a labelled Photos app.
    - We build: FULL, as Photos/lock variant. Full-screen photo (from TF card if photos are loaded, otherwise a neutral drawn placeholder — never Sripad's real wallpaper, never a chat screenshot), large digital clock overlay, tap opens the photo grid. No photos are loaded yet, so the first version shows the placeholder state honestly.

12. Wall-mounted photo grid
    - Reel: light wall, light frame, screen split into four photos (outdoor/building, plants, car, person). No camera labels or timestamps legible — NOT confirmed as a live camera feed.
    - We build: FULL, as Photos grid. Four-photo grid in the square panel, tap a photo opens it full-screen. Photos come from TF card or placeholders; this is NOT a security camera feed, and we do not claim it is.

13. Wall-mounted selfie / photo view with zoom
    - Reel: dark wall, black frame, full-screen selfie, "2x" zoom pill bottom left, small circular icon bottom right, small dot at top centre of frame.
    - We build: FULL, as Photos zoom. Full-screen photo with a 2x zoom toggle pill, tap toggles 1x/2x, circular icon opens the photo grid. The frame dot is NOT copied as a camera — our board has no camera, so no camera indicator is shown as if a camera existed.

## Not shown in the reel — do not build on assumption
- No live FaceTime/video call in progress, no live intercom call, no voice command/assistant screen, no notification banner, no automation rule editor, no swipe gesture clearly distinguishable from tap. The plan does not add those as "reel features" — FaceTime/intercom stay display-only or omitted (see 4 and 10), voice stays out (no mic), notifications stay out unless Sripad asks for a new feature.

## Build order (each stage: mockup shown first, then firmware port, then compile check)
- Phase 1 — Clock system: blue clock, Customize picker with colour tiles + check mark, green face as a picker result, widget screen (date + temperature + small clock). Done criteria: picker selection recolours clock everywhere and persists in Preferences.
- Phase 2 — Home grid + dock: gradient grid, weather widget, app icons, music card (display only), dock. Done criteria: tap opens the right app, weather is real Open-Meteo or empty-state.
- Phase 3 — Calendar: month grid, today circled, ICS event rows, "+" to Notes/Tasks. Done criteria: no invented events; no ICS = honest empty state.
- Phase 4 — Home controls: thermostat, clock tile, scene tile, door, fan, intercom tiles, local-state toggles. Done criteria: tapping changes the tile state on-device; no real-home control claimed.
- Phase 5 — Music display + Timer step + Recipe/Notes text: player UI (display only), black/orange step timer with pause/cancel, Notes text screen. Done criteria: timer pause/cancel work; music buttons change display only, labelled as such.
- Phase 6 — Photos: full-screen photo + digital clock, 4-photo grid, 2x zoom toggle. Done criteria: TF photos if loaded, placeholder state if not; never real wallpaper, never a chat screenshot.
- Phase 7 — Integration pass: same square panel, same numerals, same status behaviour, same customize behaviour across all screens; privacy scrub (no tickers, counts, totals, real wallpaper, location names, account names in mockups); real-data pass (every number traces to NTP, Open-Meteo, Stooq last-close, ICS, or on-device state).
- Phase 8 — Firmware delivery: square firmware copy only (original untouched), compile-verified with arduino-cli (ESP32 core 3.3.12, LVGL 9.2.3, LovyanGFX 1.2.36, FlashSize=16M, PartitionScheme=app3M_fat9M_16MB, PSRAM=opi), bytes + globals recorded per phase, hero fonts added only with a memory check (globals were 72% before hero fonts). Never claimed as hardware-tested until it runs on the real board.
- Phase 9 — Optional, only on Sripad's tap: a reel in the same style as the 5h/15h reels showing the new UI, shown in chat first, never posted without a tap. No post, push, submit, or order on this plan alone.

## What "exactly like the reel" means here, honestly
- Layout, colours, numerals, tiles, and interactions: as close as our code and our board allow, in our own implementation.
- Not exact: physical square board + speaker base (we have a landscape board, square UI panel instead), Apple logos/fonts/assets, copied album art, copied personal events, live FaceTime/intercom calls, live music playback, real smart-home control, live camera feed. Each of those is either display-only, omitted, or a separate future hardware/backend phase — and the plan says which, per feature, above.

# Apple HomeView-style redesign plan — wall display UI

Reference: Instagram reel https://www.instagram.com/reel/Deda5xNxzzA/ — Brad Canning, 13 Oct 2026, caption "Apple HomeView is perfection", "tabletop and wall mounted options", "Hosted by Apple".
What the reel actually shows (thumbnail frame, verified): a square display with rounded corners, thin black bezel and silver frame on a speaker base, a huge clock face with oversized 12 / 3 / 6 / 9 numerals, thin hands, a flat light-blue face, and a small "TUE 13" date at the bottom left. Clean, minimal, one thing at a time, lots of empty space.
Caveat: this is Apple's HomeView concept/display as shown at that event, not our hardware. Our board is different, so this is an inspired redesign, not a copy.

## Scope — decided in the plan, confirm before build starts
- SIZE DECISION (2026-11-01, Sripad: "change our display to match their size and the looks of the reel"): YES, in UI — the UI is redrawn as a 600x600 square panel centred on the 1024x600 board, with black side bars, a silver frame + thin black bezel, and the HomeView blue clock face. That matches the reel's square look on the hardware we already have, at zero cost.
- HARDWARE SWAP OPTION (not chosen yet): a physically square display board would need a new board, new driver/schematic, new BOM and cost, and a re-journal — major work, not tonight, and it would break the already-posted Week 4 hardware story. Keep it as a later personal-build option only if Sripad explicitly picks it.
- IN SCOPE: the on-screen UI only — home, lock, and all app screens in firmware + mockups.
- OUT OF SCOPE (unless Sripad says otherwise): hardware changes. Our board stays the Waveshare 7-inch ESP32-S3 (1024x600, landscape, flat-to-wall, no speaker, no camera). The reel's square portrait screen and speaker base are NOT copied — that would need a different board, a speaker, and a new BOM.
- IP RULE (standing): no Apple logos, no Apple's SF font, no pixel-exact Apple assets. Use Inter/Montserrat (already in the project), generic glyphs, and an inspired layout. Mockups are labelled as design mockups, not Apple screenshots.
- PRIVACY RULE (standing): no stock tickers, share counts, or portfolio values in any outward mockup or post. Stocks app shows a generic chart shape with no tickers in mockups; real firmware keeps last-close data only on-device.
- DATA RULE (standing): no fake time, weather, or calendar data. Clock from NTP/uptime, weather from Open-Meteo, calendar from ICS only.

## What Sripad needs to do first (before Muse starts building)
1. Pick the lane: (a) redesign for the personal build AFTER Week 4 submits tonight, or (b) try to fold it into Week 4 before the 11:59 PM ET close. Recommendation: (a) — a full redesign cannot be honestly journalled, compiled, and submitted in the hours left tonight without rushing. The Week 4 submission (Entry 2 posted, 24h) stays as-is either way.
2. Pick the base theme: light (HomeView blue face / white system) or dark (black faces, white numerals). Recommendation: light home + dark app option, matching the reel's blue clock face.
3. Pick the wallpaper rule for the redesign: keep the campfire image for posted pics, use a plain solid/gradient for the UI itself, or no wallpaper at all (HomeView style = solid face, no photo behind the clock). Recommendation: solid faces, no photo wallpaper in the UI.
4. Confirm the screen shape rule: our screen is 1024x600 landscape, the reel is square portrait. The plan adapts HomeView to landscape (wide clock, numerals left, info right) instead of forcing a square layout that would crop on our board.
5. One tap to start. After that, Muse works stage by stage, shows each stage visually, and does not post, push, submit, or order anything without a fresh tap.

## Stage 0 — Baseline audit (done criteria: a one-page audit note)
- Inventory current UI: home v30 (17 apps, no widgets, no camera), lock v20, app screens v19 (clock, weather, stocks, tasks, calc, calendar, music), firmware screens in wall-display.ino (compile-verified 1,419,011 bytes, globals 72%).
- List what breaks HomeView style: small clock text, dense icon grid, black-only app faces, inconsistent corner radii, status bar clutter.
- Output: audit note in files/, no code changes yet.

## Stage 1 — Design system (done criteria: a single reference sheet, shown in chat)
- Colours: light face #7FB8FF-ish flat blue for clock hero (tuned, not sampled from Apple), system white #FFFFFF, ink #111111, secondary grey #6B7280, accent blue #0A84FF-style, dark face #000000 with white numerals. Light and dark tokens, one set each.
- Type: Montserrat on-device, Inter in mockups. Scale: hero numerals ~200-280px on 1024x600, titles 48-64, body 28-36, captions 24. One weight for numerals (light/bold contrast), never more than two weights per screen.
- Shape: squircle icons with large corner radius, screens with a uniform outer margin so the UI reads as a thin-bezel panel, 24-32px spacing, no borders unless a card needs one.
- Motion: fade/slide only, 200-300ms, no bounce, no 5-second silent stretches in any reel VO (standing rule).
- Output: design-system sheet image + tokens table, shown for approval before any screen is redrawn.

## Stage 2 — Home screen, HomeView-style (done criteria: mockup approved, then firmware port)
- Layout (landscape adaptation): left ~55% = huge clock (oversized numerals, thin hands, small weekday + date underneath, HomeView style). Right ~45% = a clean 3x3 or 2x4 app grid, squircle icons, one-line labels, no widgets, no tabs (standing home rule: apps only).
- Status bar: time hidden on home (it IS the clock), only Wi-Fi/battery small at top right, hidden on long-press customize (standing rule: customize only on long-press, top-right, never visible normally).
- Wallpaper: solid face, not a photo. The campfire image stays only as a posted-pic asset, not behind the UI.
- Output: home mockup 1024x600, shown in chat, then firmware home screen rewritten to match.

## Stage 3 — Lock screen (done criteria: mockup approved, then firmware port)
- Full-face huge clock, oversized numerals, thin hands, weekday + date small at bottom left (mirroring "TUE 13" in the reel), tap to unlock, no notifications list, no widgets.
- Output: lock mockup 1024x600, shown in chat, then firmware lock screen rewritten to match.

## Stage 4 — App screens, one at a time (SPLIT DELETED 2026-11-01 per Sripad: split screen removed from mockups, settings, and square firmware; single-app full screen only. Square firmware compiles at 1,418,735 bytes, globals 237,184.)

## Stage 4 — App screens, one at a time (done criteria per app: mockup approved, then firmware port)
Order: Clock, Weather, Calendar, Timer, Tasks, Calculator, Notes, Search, Music (display only), Maps, Mail, Settings, Split screen.
- Clock: same hero numerals as lock/home, world clock / alarms / stopwatch / timer as clean list rows, no fake time.
- Weather: big temperature numeral, small condition + humidity + wind rows, Open-Meteo data only, Alpharetta default.
- Calendar: month grid with the same large-numeral header, "up next" as one clean row, ICS data only.
- Timer / Tasks / Calc / Notes / Search: one primary number or one list at a time, large tap targets, round calculator buttons (already fixed), no clutter.
- Music: display only (standing decision), no playback claim, no Spotify logo.
- Maps: OpenStreetMap centre/zoom/tile label, no key; tile photo render only if lv_png work lands (currently fetch/location real, photo render not claimed).
- Mail: IMAP headers only, credentials over Serial only, never in mockups.
- Settings: brightness + single/split mode as clean toggles/sliders, no hidden menus.
- Split screen: two live panels, each with its own hero number, mode saved in Preferences.
- Output per app: mockup image, shown in chat, then firmware screen rewritten to match. No app moves to firmware until its mockup is approved.

## Stage 5 — Firmware port + compile verification (done criteria: clean compile, size tracked)
- Port stages 2-4 into files/firmware/wall-display.ino using LVGL 9.2.3 + LovyanGFX 1.2.36, ESP32 core 3.3.12, board settings FlashSize=16M, PartitionScheme=app3M_fat9M_16MB, PSRAM=opi.
- Watch memory: globals are already at 72% — if hero fonts push globals over budget, shrink font sets (drop unused Montserrat sizes) before cutting features, and report the trade-off.
- Verify with arduino-cli (~/bin/arduino-cli), sketch folder name matching sketch name, lv_conf.h with Montserrat 24/28/36/48 (plus hero sizes as needed). Record bytes + globals after every stage.
- NO hardware test claim: compile-verified only, not hardware-tested, until it runs on the real board.
- Output: updated firmware + README notes, compile log per stage.

## Stage 6 — End-to-end pass (done criteria: checklist signed off in files/)
- Consistency pass: same numerals, same spacing, same status bar, same customize behaviour across home/lock/all apps.
- Privacy pass: no tickers, counts, or portfolio values in any mockup; no real wallpaper in any posted pic; no chat screenshots as assets.
- Real-data pass: every number on every screen traces to NTP, Open-Meteo, Stooq last-close, ICS, or on-device state — no invented values.
- Reel pass (only if Sripad asks): a short reel in the same style as the 5h/15h reels (full images, no cropping, Helio VO, no BOM, no what's-next) showing home → lock → clock → weather → calendar. Shown in chat first, never posted without a tap.
- Output: checklist file, plus a final set of mockups shown together in chat.

## Stage 7 — Delivery + Week 4 boundary (done criteria: user decision recorded)
- Deliver: final mockup set + firmware file + compile log, in chat, with a clear label: "personal build v2, not Week 4 submission" unless Sripad explicitly moves it into Week 4.
- If he wants it in Week 4: that needs a new journal entry, new images, and a submission decision — all separate taps, and only if the Week 4 window is still open. Do not silently rewrite already-posted Entry 1 / Entry 2.
- Nothing posts, pushes, submits, or orders on this plan alone.

## Risks / honest limits
- Our board is landscape 1024x600, not square portrait — the HomeView look adapts, it does not copy the reel frame-for-frame.
- LVGL hero numerals at 200px+ need large font assets; memory is tight (globals 72%) and may force a font trade-off.
- No speaker on the board, so the reel's speaker-base form factor is not reproducible in hardware without a new board/BOM.
- Apple HomeView itself is Apple's concept/display — this plan is an inspired UI redesign for Sripad's own project, not an Apple product claim.

## Batch 2 progress — 2026-11-01 (subagent, autonomous)
- Mockups 10-16 built (Notes, Search, Music display-only, Maps, Mail, Settings, Split), all 1024x600, same square panel, Inter only, each footered "LAYOUT PREVIEW - values are placeholders, not live data".
- Privacy scrub: opened all 7 new mockups; Maps third row overflow fixed (now "Tile / x / y" + "Tile photo not rendered - fetch only"); no tickers, counts, totals, real wallpaper, location names, or account names present in any new mockup.
- Firmware copy: files/firmware/wall-display-square.ino (original wall-display.ino untouched) + sketch folder wall-display-square/. Changes: sq_frame() 600x600 silver frame on every screen, lock rebuilt as blue square panel, card radius 24->32. All data functions, Serial/Wi-Fi credential handling, no-camera, music-display-only, maps tile-photo-not-rendered rules unchanged.
- Compile: arduino-cli, ESP32 core 3.3.12, LovyanGFX 1.2.36, LVGL 9.2.3, FlashSize=16M, PartitionScheme=app3M_fat9M_16MB, PSRAM=opi — 1,419,295 bytes (45%), globals 237,200 bytes (72%), 0 errors. Index-download TLS warnings are the known environment quirk; local libraries were used.
- NOT hardware-tested. Remaining: hero fonts (Montserrat 48 max enabled, mockup 200px+ numerals not in firmware), white app panels in firmware, and any hardware test on the real board.

# Home screen v3 — Apple-style, light (mockups 66-68, PNGs manual upload)

User feedback 2026-11-01: the Midnight Glass home did not look like
Apple's HomeView. Home v3 fixes home only; other apps stay Midnight
Glass v2 unless Sripad says otherwise.

## What Apple home looks like (from the two HomeView reels)

- Light, not dark: pale blue clock face, off-white background, white tiles.
- Huge numerals, only 12 / 3 / 6 / 9, thin hands, almost empty screen.
- One hero clock, then a few big widgets — not a dense grid of dark cards.
- Dashboard page: GOOD MORNING greeting, profile avatar row, weather
  widget with high/low, small blue analog clock, calendar tile, app row.

## Mockups (design-previews/, PNGs only)

- 66-home-apple-clock-v3.png: page 1, lock/home hero — pale blue face,
  huge 12/3/6/9, black hour hand, white minute hand, thin second hand,
  "SUN 1" bottom left, "Tap to unlock" bottom right, silver frame +
  thin black bezel, square panel centred on the 1024x600 board.
- 67-home-apple-dashboard-v3.png: page 2, swipe — profile row (numbered
  avatars + "+"), "GOOD MORNING", weather tile (72°, Sunny, H: 75°,
  L: 53° — placeholders), blue analog clock tile, calendar tile (ICS
  event only, honest "No calendar connected yet" empty state), app row.
- 68-home-apple-icons-v3.png: same dashboard, WITH app icons at the
  bottom — Clock (analog glyph), Weather (sun over cloud), Calendar
  (outline), Timer (orange ring, "5"). This answers Sripad's question:
  yes, add icons to the apps. Icons are generic drawn glyphs, NOT
  Apple/Google/Spotify logos, NOT copied assets.

## Firmware status for home v3

- Firmware home face today is the blue panel (face-colour preference);
  the light dashboard page and the icon row are mockup direction, NOT
  yet ported to firmware. Porting them is the next firmware step, with
  the same 48pt hero-numeral limit and 77% globals memory check.
- No home controls in home v3. Thermostat/door/fan/intercom stay deleted.

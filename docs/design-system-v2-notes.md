# Design System v2 — "Midnight Glass" (wall display, everything except Guestbook)

Guestbook keeps its liquid chrome full-bleed look as defined. Everything else follows
Midnight Glass, so the two sit together without copying each other.

## Direction (inspo, layout ideas only, no copied assets)
- Wall dashboards: one screen, no scroll; uniform grid; touch targets 64px+;
  critical info readable from across the room; sections swipe, never nested menus.
- Liquid glass: frosted cards, 1px bright top/left edge, 2-3 depth planes max,
  solid fallback behind every glass surface so text contrast never depends on blur.
- Widgets: huge type first, details second; tap zones open the full app; event dots
  on calendar, high/low on weather, no invented values.

## Tokens (mockups Inter, firmware Montserrat — no Apple SF, no logos)
- Background: near-black #0B0E14, square panel 600x600 centred on 1024x600 board,
  silver frame + thin black bezel (existing hardware framing, unchanged).
- Cards: dark frosted #1A1F2B, radius 32, 1px top edge #FFFFFF33, 16px gaps.
- Ink: white #FFFFFF, secondary #9AA3B2, accent blue #0A84FF.
- Accents by function only: blue = navigation/time, amber #FF9500 = timer/music,
  green #34C759 = done/positive, red #FF3B30 = delete/cancel.
- Type: hero 48pt max on-device (LVGL font limit), titles 36, body 24, captions 14.
  Mockups may show larger hero numerals as a target; firmware stays at 48pt until
  hero font assets are added with a memory check (globals were 77% pre-Guestbook).
- Spacing: 24px outer margin, 24px header, back button top-left on every app screen.
- Motion: 200ms fade/slide only; no bounce, no rainbow effects. Guestbook's 7
  animations are the exception and stay inside Guestbook.
- Touch: every button 64px+ tall, labels under icons, selected state = accent border.
- Data honesty: live NTP/Open-Meteo/ICS/Stooq-last-close only; empty states say so;
  placeholders in mockups are footered as placeholders, never presented as live data.

## Screens in v2
Home clock (hero face, greeting, date) -> Widgets (huge date + temp + small clock)
-> App grid (uniform squircle grid, weather card, dock row) -> each app full screen,
single-app only. Split view stays deleted. Home controls stay deleted.

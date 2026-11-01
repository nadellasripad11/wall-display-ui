# Sprint batch 1 — inspo + design system + mockups (firmware NOT touched yet)

- Inspo (layout ideas only, no copied assets): wall-dashboard guides (one screen,
  no scroll, 64px+ targets, readable from across the room, uniform grid, swipe
  sections), liquid-glass guides (frosted cards, 1px bright edge, 2-3 planes max,
  solid fallback for contrast, 150-200ms motion), widget guides (huge type,
  high/low, event dots, tap zones open full apps).
- Design system v2: Midnight Glass — #0B0E14 background, #1A1F2B cards r32,
  accent #0A84FF, amber timer/music, green done. Guestbook keeps liquid chrome.
- Mockups built + privacy-scrubbed (opened each; fixed 48 overflow, 49 greeting
  overlap, 53 calendar column-major bug, 59 button/footer overlap): 48-65.
  Scrub by kind: no tickers/counts/totals, no real wallpaper, no real location
  names, no account names/credentials, no chat screenshots; neutral placeholders
  only, footered as placeholders.
- Firmware: NOT edited — Guestbook agent was still editing at 22:20 UTC
  (mtime moved during this sprint), so the conflict rule holds. Firmware port
  waits until that agent finishes, then compile-verify.
- Push: NOT done — waiting for Sripad to create nadellasripad11/wall-display-ui
  himself + approve. Plan in files/push-plan-wall-display-ui.md: text-only via
  connector in many small commits; PNGs manual upload only; never old repo.

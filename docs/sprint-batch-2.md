# Sprint batch 2 — firmware port (after Guestbook finished)

- Guestbook finished + verified by parent: 1,521,783 bytes, globals 255,056 (77%).
  Firmware re-read before editing; both square copies were identical.
- Port (token-only, NO new globals, Guestbook preserved): Midnight Glass tokens —
  C_BG 0x0B0E14, C_CARD 0x1A1F2B, C_SUB 0x9AA3B2, C_ACCENT 0x0A84FF, C_LINE
  0x2A2F3D; style_card radius 32 + 1px subtle edge; back_button 120x64
  (64px+ touch target). gb_chrome / Guestbook screens untouched (verified by grep).
- No home controls re-added (grep: only DELETED/omitted comments), split stays
  deleted, original firmware untouched, both square copies kept in sync.
- Compile (independently run): 1,521,847 bytes (48%), globals 255,056 (77%,
  72,624 left) — +64 bytes vs Guestbook build, globals unchanged.
- NOT a full coordinate relayout of every screen: with globals at 77% and
  Guestbook preservation required, the safe port is tokens + card edge +
  touch targets. Mockups 48-65 remain the layout target for any future relayout.
- Push: still NOT done — gated on Sripad's push approval (repo confirmed:
  https://github.com/nadellasripad11/wall-display-ui). Plan in
  files/push-plan-wall-display-ui.md.

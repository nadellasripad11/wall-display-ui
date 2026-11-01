# Guestbook — Core Memories — full spec (8 steps, 7 animations)

Liquid chrome look, Guestbook ONLY. On-device it is simplified chrome
(silver gradient + dark vein lines + white glass cards) — photo-real
liquid swirls are not renderable in LVGL on this board. Everything else
keeps Midnight Glass v2. Design mockups are in design-previews/40-47
(PNGs, manual upload only).

## 8 steps, in order

1. Guestbook home — "CORE MEMORIES", live total count, "Leave your
   mark.", "SIGN THE GUESTBOOK ->", "RECENT ENTRIES" (up to 3 newest),
   "VIEW ALL ->".
2. Handwritten signature canvas — finger-drawn pad (GT911 touch, LVGL
   lines, one line per stroke, points only inside the pad), UNDO
   (removes last stroke), CLEAR, DONE. Done with fewer than 2 points
   shows "Draw your signature first" and does not advance.
3. Signature captured celebration — animations 1-3, auto-advances to
   guest details after 2,600 ms, tap advances immediately.
4. Guest details — Name field + Who came with you field (blank =
   "alone"), tap a field to switch typing (active field gets the accent
   border), A-Z keyboard + Space + Backspace, "SAVE MEMORY". Empty name
   shows "Enter a name first" and cannot save. Back returns to the pad
   with the signature preserved.
5. Saved confirmation — signature + name + who + auto date/time,
   "SAVED ✓" header, OK returns to Guestbook home. Date/time is the
   live clock (NTP once Wi-Fi is set, otherwise uptime) — never invented.
6. Signature replay — light-trace animation (see below), Replay button.
   Back returns to the individual view.
7. Memories gallery with filters — All / Favorites / With friends /
   This week pill buttons (selected filter shown dark), scrollable
   cards, tap a card to open its individual view. "This week" uses the
   saved NTP timestamp (within 7 days); uptime-stamped entries count
   as this week.
8. Individual memory view — large signature, name, who, date/time,
   REPLAY button, favorite toggle ("Favorite" / "Favorited *"), chrome
   seal badge on the card corner while favorited. Favorite state is
   saved on-device in Preferences and reflected in the Favorites filter.
   Back returns to the gallery.

## 7 animations (no generic confetti, no emoji rain)

1. Ink-to-chrome: after Done, a chrome shine bar sweeps the celebration
   panel (~900 ms), then signature lines recolour black ink -> metallic
   silver (0xC0C0C8), nothing else moves.
2. Stamp press: the "SAVED / Signature locked in" stamp drops from y=60
   to y=220 over 300 ms on the celebration screen.
3. Liquid chrome ripple: a white ring expands from the signature area
   (radius 30 -> 280, fading) on the celebration screen, background only.
4. Light trace replay: a blue dot traces the exact saved signature path
   point by point (60 ms steps), each stroke revealing progressively;
   the full signature is visible at the end.
5. Card develops: on the Saved screen, signature lines fade in from
   transparent -> half -> full, then the name appears, then the date/who
   line (150 ms phases).
6. Chrome seal favorite: tapping Favorite pops a chrome seal (silver
   circle, "*" centre) onto the detail card corner with a zoom-in
   animation; unfavoriting hides it.
7. Pad breathes while signing: while a finger is down on the pad, the
   pad border thickens to 4 px and turns silver; on lift it settles
   back to 2 px grey.

## Saving

Signature stroke points + name + who + auto date/time + favorite flag,
on-device in Preferences only, survives reboot, never sent anywhere.
Firmware list starts empty; only visitor-entered names ever appear.
Caps: 20 entries (oldest dropped), 8 strokes per signature, 120 points
per stroke, 150 points total per saved signature.

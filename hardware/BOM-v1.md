# Week 4 — Single-sided wall display — BOM and start list
Started 2026-10-27 evening. Concept: a bigger iPhone on the wall.
Single-sided, flat on a wall, touch home screen with app icons.
Outward text: it shows apps (do NOT list the apps in project/reel text,
per Sripad 2026-10-27 evening).
Board: Waveshare ESP32-S3 7-inch Type B, Touch Function, 1024x600.
Board outline: 192.96 x 110.76 mm. Weight: 0.369 kg. Power: USB-C, 5V, 350mA draw.
Tier target: Tier 3 ($100). Tier 3 design minimum is 20 hours (per Week 3 tier notes).
Power bank: DROPPED from the Week 4 BOM. Power is wall cable only, switch kept, no backup.
Status: NOT ordered, NO Half Life project created yet, NO schematic, NO firmware, NO submit.
Checkout totals with shipping/tax are still unverified for every vendor.

## In the BOM right now (quantity 1 each)

| Item | Vendor | Sale price | List price |
|---|---|---|---|
| ESP32-S3 7-inch Display Development Board Type B, Touch Function, 1024x600 | Waveshare direct | $38.99 | $38.99 |
| Essager 20W GaN USB-C charger, 5V/3A, US plug | AliExpress | $4.55 | $9.48 |
| UGREEN USB-C cable, 2m, 3A braided | AliExpress | $5.37 | $9.37 |
| Inline USB-C on/off switch cable, 30cm, 3A | AliExpress | $1.09 | $6.47 |
| Wall mount tablet holder, telescopic, 7-13 inch, metal, anti-theft lock | AliExpress | $36.31 | $54.20 |
| Hyper Tough Large Removable Adhesive Strips, 12 pairs, 5 lbs capacity, pre-cut strips (NOT foam tape) | Walmart | $6.28 | $6.28 |
| **Total before shipping/tax** | | **$92.59** | **$124.79** |

Headroom at sale prices: $7.41 to the $100 cap, $10.41 to the $103 wizard allowance,
before shipping/tax. At list prices this set is already over Tier 3, so the
checkout total at submission time is what counts.

Caveats already known:
- Mount fit is by 7-13 inch rating only. The page publishes no mm clamp range,
  so it is NOT a verified exact fit for 192.96 x 110.76 mm, and screw-hole fit is unverified.
- Strips are rated 5 lbs per set, well above the bare 0.369 kg board, but frame
  weight is unknown. Strips alone are only safe for a light frame; a heavier frame needs screws.
- Charger: any existing 5V charger rated 1A+ is electrically enough for the 350mA board.
  Whether Sripad already owns one (and can drop the charger row) is still open.
- Sale prices can end. Shipping and tax per vendor (Waveshare, AliExpress, Walmart)
  are unverified, and cart screenshots are needed per vendor at submission.

## Decisions locked 2026-10-27 evening

- Frame: DROPPED. Sripad: the holder already hangs it, a frame is not really
  needed. No frame cost in the BOM, headroom stays $7.41 at sale prices.
- UI: USER-CHOOSABLE ON THE DISPLAY, corrected 2026-10-27 evening.
  Sripad can choose on the display itself: either split screen or a single
  app full screen. Split view shows clock + stocks/weather at a glance;
  single view opens one app full screen. This supersedes the earlier
  split-screen-only choice, which was Muse's, not his.

## What we need to add / decide

1. Wall and outlet: which wall, distance to the nearest outlet, Wi-Fi strength there.
   This confirms whether the 2m cable is long enough.
2. Mount fit: telescopic holder is the mount (no frame). Its 7-13 inch rating
   is still not a verified exact mm / screw-hole fit for 192.96 x 110.76 mm.
3. Charger owned or bought: confirm whether he already owns a 5V 1A+ USB-C charger.
4. Mount decision: keep the telescopic holder, or replace it with a built frame
   that mounts directly. The holder and a built frame may overlap — do not count both
   without deciding which one actually holds the board.
5. UI design: home screen with app icons (iPhone-like), tap an icon to open an app,
   tap home to return. Or split dashboard? Home-icon layout is the current framing.
6. Data sources: weather location, stocks tickers, calendar source, photos source
   (internal flash only, since photos storage / SD was dropped).
7. Checkout verification: real checkout totals per vendor, with legible cart
   screenshots, before any BOM total is locked for submission.
8. Design-week work, in order: Half Life project creation (needs his browser sign-in),
   repo, system schematic (board + USB-C power + switch + mount), LVGL firmware for
   the six apps, enclosure design, README in his own words with BOM and at least
   2 of his own images, journals, closing reel. Submit wizard stopped before submit
   for his tap. Nothing orders without his tap on the exact checkout total.

## Dropped, not in this BOM

- Power bank backup (HONCLL $52.08 UPS and UGREEN $71.78 UPS): dropped 2026-10-27.
  The Week 5 BOM already plans a power bank for genuine portable use there.
- Foam tape roll: replaced by pre-cut strips.
- SD / TF card for photos: dropped by Sripad when he said forget photos storage.

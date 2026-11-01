# Week 4 — Single-sided wall display — BOM — CORRECTED DRAFT v2, NOT PUSHED
# Prepared 2026-10-29 evening (21:xx ET) after Sripad's flat-wall correction.
# Supersedes the 2026-10-29 corrected draft ($92.59 with mount) — that draft is now stale.
# Do NOT push to GitHub without Sripad's tap. Nothing ordered, nothing submitted.
#
# Change from v1 corrected draft: the $36.31 telescopic wall mount is REMOVED.
# Sripad 2026-10-29 ~20:57 ET: "Wait I thought we weren't having a hanging thing
# from wall" — journal text revised the same evening to "no hanging mount, it sits
# flat on the wall" and "bom without the mount is $56.28 at sale prices before
# shipping and tax." Posting of the revised 15-hour Journal Entry 1 was requested
# at 21:01 ET the same evening; confirmation that it logged is NOT in this draft —
# verify the Half Life project hours before claiming 15h is logged.
#
# Project: Half Life Week 4, draft at https://halflife.hackclub.com/projects/pv1dawbazq5nm13wc0s2byy6
# (title 'wall display', Tier 3 $100 / 20h, weeks 4 & 9). Idea reel posted 2026-10-27.
# Board: Waveshare ESP32-S3 7-inch Type B, Touch Function, 1024x600, 192.96 x 110.76 mm,
# 0.369 kg, USB-C 5V, 350mA draw. Power: wall cable only, switch kept, power bank DROPPED.
# Outward text: "shows apps" only — no app list, no Buddy mention, no iPhone comparison
# (removed from the journal text at Sripad's request 2026-10-29 evening).

## In the BOM right now (quantity 1 each)

| Item | Vendor | Sale price | List price |
|---|---|---|---|
| ESP32-S3 7-inch Display Development Board Type B, Touch Function, 1024x600 | Waveshare direct | $38.99 | $38.99 |
| Essager 20W GaN USB-C charger, 5V/3A, US plug | AliExpress | $4.55 | $9.48 |
| UGREEN USB-C cable, 2m, 3A braided | AliExpress | $5.37 | $9.37 |
| Inline USB-C on/off switch cable, 30cm, 3A | AliExpress | $1.09 | $6.47 |
| Hyper Tough Large Removable Adhesive Strips, 12 pairs, 5 lbs capacity, pre-cut strips (NOT foam tape) | Walmart | $6.28 | $6.28 |
| **Total before shipping/tax** | | **$56.28** | **$70.59** |

Headroom to the $100 Tier 3 cap before shipping/tax: $43.72 at sale prices,
$29.41 at list prices. That is the budget change that matters: shipping/tax are
far less likely to blow Tier 3 than they were at $92.59 — but they are still
unverified, and any flat holder added below will eat into this headroom.

## What is still UNDECIDED (do not lock the BOM until these are answered)

1. Flat holder — the open question from 2026-10-29 evening, still unanswered:
   strips straight to the wall, or a flat frame/case holding the board?
   The journal text says "sticky strips" and "sits flat on the wall" but names
   no holder. Do NOT count both a holder and the mount. If a frame/case is
   added, add its price to $56.28 and re-check the $100 cap with shipping/tax.
   Earlier caution (2026-10-28): sticking a bare board directly with adhesive
   risks ports/buttons and removal — a light frame holding the board is safer
   than adhesive on the board itself. That caution is still open, not decided.
2. Wall and outlet: which wall, distance to the nearest outlet (confirms the
   2m cable is long enough), Wi-Fi strength there.
3. Charger owned or bought: any existing 5V 1A+ USB-C charger works for the
   350mA board; dropping the $4.55 charger row would take the total to $51.73.
4. Schematic half 2: display/touch wiring, board I/O, and the flat-wall part —
   NOT started, still needs Sripad's approval. The half-2 wording must say
   "flat wall part", not "mount part".
5. Checkout verification: real checkout totals per vendor, with legible cart
   screenshots, before any BOM total is locked for submission.
6. Project submit: Week 4 closes Sunday, Nov 1, 2026 at 11:59 PM ET.
   If the 15-hour Journal Entry 1 logs, 5 hours remain to Tier 3's 20h.

## Dropped, not in this BOM

- Telescopic wall mount tablet holder, $36.31 sale / $54.20 list — REMOVED
  2026-10-29 evening (no hanging thing from the wall).
- Power bank backup (HONCLL $52.08, UGREEN $71.78) — dropped 2026-10-27.
- Frame/case options set aside, not in the total: Mainstays 8x10 frame $2.08
  (unverified fit, "looks really bad" per Sripad), Waveshare PI5-CASE-TD2 $8.99
  (Pi port layout, no switch slot — inline cable switch sits outside, so that
  part is fine), BARNWOODUSA open frame $29.99, 3D-printed enclosure (no printer
  access confirmed). None of these is counted in $56.28.
- Foam tape roll: replaced by pre-cut strips. SD/TF card for photos: dropped.

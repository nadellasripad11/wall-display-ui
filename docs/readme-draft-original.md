# wall display — README DRAFT (rewrite in your own words before pushing)

IMPORTANT: Half Life design weeks require the README to be in your own
words, not AI-written. This draft is a starting point only — rewrite it
in your voice, then push. Do not push this draft as-is.

## what
a big wall display you can tap. it shows apps, all on one 7 inch touch
screen. it runs on an esp32-s3 board and plugs into the wall, so it stays
on and you can just glance at it. you can choose on the display itself,
either split screen or a single app full screen.

## why
i wanted a display for my wall that stays on, instead of picking up my
phone every time. single sided, so the screen could be bigger, and flat
to the wall, no hanging mount.

## how
- board: waveshare esp32-s3 7 inch display board, type b, touch function
- power: wall charger, cable, inline switch cable, board usb-c, 5v only
- holder: flat protective holder, strips on the holder, not on the board
- firmware: lvgl ui, home screen, split or single app full screen, timer,
  clock with ntp once wifi is set, live weather and stocks last close
  from free no-key sources, calendar from an ics url you set

## bom
| part | price | link |
|---|---|---|
| waveshare esp32-s3 7 inch display board, type b, touch | $38.99 | https://www.waveshare.com/esp32-s3-lcd-7b.htm?sku=31726 |
| essager 20w gan charger, us white | $4.55 | aliexpress 3256806874329623 |
| ugreen 60w cable, 2m, silver | $5.37 | aliexpress 3256808074455409 |
| inline usb-c switch cable, 30cm, black | $1.09 | aliexpress 3256812085710088 |
| hyper tough strips | $6.28 | walmart |
| total at sale prices, before shipping/tax | $56.28 | |
| holder | not decided yet, not in total | |

## images
- schematic half 1: power chain
- schematic half 2: display, touch, holder
- board front product photo

## status
- journal entry 1 posted, 15h
- schematic done, firmware v4 compile-verified, not hardware-tested
- not ordered, not submitted yet

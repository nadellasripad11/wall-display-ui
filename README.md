# wall display

a touchscreen display that lives on your wall and puts a few useful things in one place.

i'm building this around a 7-inch esp32-s3 display. the idea is pretty simple: instead of checking a bunch of different devices for the time, weather, calendar, notes, or a timer, you can just walk up to the screen and tap what you need.

the project is still a work in progress, but the firmware and main interface are taking shape.

![front of the display board](images/02-board-front.jpg)

## the idea

i wanted to make something that feels like part of a home, rather than another tablet sitting on a desk. it should be easy to glance at from across the room, but still useful when you walk up and interact with it.

the interface uses a square-style layout centered on the display. the home screen is built around a large clock and a small set of widgets, with the other screens kept one tap away.

## core memories: the guestbook

the guestbook is probably my favorite part of the project.

when someone visits, they can walk up to the display and sign their name with their finger. after signing, they add their name and who they came with. the display saves the signature along with the date and time.

later, you can open the memories gallery, look through past visits, favorite an entry, or replay the signature and watch it draw itself again.

guestbook entries are stored on the device, not uploaded to a server. the goal is to make it feel more like a real guestbook that happens to live on a screen.

## what's included

the firmware has screens and ui for:

- **home and clock** — a clock-first home screen with a customizable clock face.
- **widgets** — a quick view for things like the date, weather, and time.
- **everyday tools** — timer, calendar, tasks, calculator, notes, and search.
- **information screens** — weather, stocks, maps, and mail.
- **personal screens** — photos, a photo clock, and a recipe view.
- **music** — a display-only screen. it doesn't play music.
- **core memories** — the handwritten guestbook, saved entries, favorites, and signature replay.

not every screen is fully connected yet. some features need wi-fi or an external data source, and a few are still being refined. when information isn't available, the display should show an empty or not-connected state instead of making up data.

## hardware

the current target is the **waveshare esp32-s3 7-inch display development board, type b, with touch**.

- 7-inch, 1024 × 600 display
- esp32-s3 controller
- gt911 capacitive touch
- usb-c power
- designed to stay plugged in on the wall

the current square interface uses a 600 × 600 area centered on the board's 1024 × 600 screen.

![display board dimensions](images/04-board-dimensions.jpg)

### hardware diagrams

the repo also includes the power and display/touch diagrams:

- [power setup](hardware/schematics/week4-schematic-half1.png)
- [display and touch setup](hardware/schematics/week4-schematic-half2.png)

## software

the firmware is written in c++ for the esp32 and uses:

- **lvgl 9** for the interface
- **lovyangfx** for display and touch handling
- **esp32 arduino core** for the board and networking
- **preferences** for saving settings and guestbook entries on the device

wi-fi details and other private configuration are entered on the device over serial. credentials and api keys should not be added to the source code or committed to this repo.

## building the firmware

the main sketch is in `firmware/wall-display-square.ino`. there is also an earlier, wider-layout sketch at `firmware/wall-display.ino`.

to build the current version, you'll need the esp32 arduino board package, lvgl 9.x, and lovyangfx. use the following board settings:

- flash size: 16 mb
- app partition: 3 mb
- psram: opi

the lvgl configuration also needs montserrat fonts 24, 28, 36, and 48 enabled. the notes in [firmware/lv_conf-notes.txt](firmware/lv_conf-notes.txt) cover that part.

## current status

the firmware has been compile-verified with the required board settings. it has **not been flashed and tested on the physical display yet**, so the actual screen output, touch behavior, backlight control, and animation timing still need hardware testing.

this is an active project, not a finished plug-and-play product. i'm still working through the hardware testing and polishing the screens.

## project files

- [firmware](firmware/) — esp32 sketches and configuration notes
- [guestbook design](docs/guestbook-spec.md) — the core memories flow and behavior
- [home screen notes](docs/home-apple-v3-spec.md) — home screen layout and design direction
- [hardware notes](hardware/) — board, power, and wiring information
- [schematics](hardware/schematics/) — power and display/touch diagrams

---

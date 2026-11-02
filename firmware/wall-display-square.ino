// Wall Display — SQUARE variant (COPY of wall-display.ino, original untouched)
// Square HomeView-style panel: 600x600 centred on the 1024x600 board,
// black side bars, silver frame + thin black bezel, blue lock panel.
// Mockups in files/week4-apple-redesign/ (01-16) are the layout reference.
// INTEGRATED 2026-11-01 (HomeView full plan, square copy only):
// clock face-colour preference + picker, exact-clock home + app grid,
// widget screen, November calendar + today + ICS-only events, home
// controls (local state only), music display-only player, step timer
// with orange ring + pause/cancel, Notes user text, photos grid +
// full view + 2x zoom. Split view stays deleted. No FaceTime, no
// Safari-as-browser, no HomeKit, no live intercom, no copied assets.
// HONEST LIMITS: LVGL here enables Montserrat 24/28/36/48 only, so the
// firmware does NOT yet render the mockups' 200px+ hero numerals, and
// pre-existing app screens keep their full-width layout inside the
// square frame; new HomeView screens use panel coordinates. Hero fonts
// + white app panels are remaining work (see plan).
// All data functions, Wi-Fi/Serial credential handling, no-camera and
// music-display-only rules are unchanged from the original.
// NOT hardware-tested. Compile-verified only.
// Wall Display — Firmware v4 (FULL product firmware, Apple-clean UI, REAL DATA)
// Real data, no keys, no fake numbers:
//   Weather: Open-Meteo, no API key, verified 2026-10-30:
//     https://api.open-meteo.com/v1/forecast?latitude=34.08&longitude=-84.29
//     &current=temperature_2m,relative_humidity_2m,weather_code,wind_speed_10m
//     &temperature_unit=fahrenheit&timezone=America/New_York
//   Default location is Alpharetta, GA (school area, 34.08, -84.29) —
//   change WEATHER_LAT / WEATHER_LON below to your exact location.
//   Stocks: Stooq daily CSV, no key, https://stooq.com/q/d/l/?s=<ticker>.us&i=d
//     Columns Date,Open,High,Low,Close,Volume. First data row is used as
//     the latest close, and the header MUST start with "Date" or the fetch
//     is rejected (Stooq returns HTML after its unpublished daily limit).
//     Stocks are LAST CLOSE, not intraday live — the label says so.
//   Default tickers are the user's real holdings NU, SOFI, VTI — change
//   STOCK_TICKERS below to change them.
//   Clock: NTP once Wi-Fi is set via Serial (WIFI <ssid> <password>),
//     uptime otherwise. Calendar: an ICS URL can be set via Serial
//     (ICS <url>); the first event summary is shown, or "No calendar
//     URL set" — no fake events are ever shown.
//   Photos: no photos are bundled. Add your own images to flash in
//     build week; until then the Photos tile says so plainly.
// Wall Display — Firmware v3 (FULL product firmware, Apple-clean UI)
// Board: Waveshare ESP32-S3 7-inch Type B, Touch Function, 1024x600, GT911 touch.
// Pins verified from the Waveshare wiki pinout for ESP32-S3-Touch-LCD-7B:
//   Display RGB: VSYNC GPIO3, DE GPIO5, PCLK GPIO7, HSYNC GPIO46,
//     R/G/B data GPIO0,1,2,10,14,17,18,21,38,39,40,41,42,45,47,48
//   Touch: TP_SDA GPIO8, TP_SCL GPIO9, TP_IRQ GPIO4, TP_RST via IO-expander EXIO1
//   Backlight: DISP via EXIO2, LCD_VDD_EN via EXIO6
//   I2C addresses on board: 0x24 (IO expander), 0x5D (touch)
// Power: wall USB-C only, 5V, board draws 350 mA. No battery, no power bank.
//
// DESIGN (Apple-clean, as requested):
//   Light background #F5F5F7, white rounded cards, dark text #1D1D1F,
//   secondary text #86868B, one accent #0071E3, no clutter, no gradients,
//   large clean type, generous spacing. No app list is claimed in outward
//   project text — outward text says only "shows apps".
//
// WHAT IS FULL IN V3:
//   - Home: exact clock (tap opens the app grid); single-app only
//   - Split view DELETED 2026-11-01: no split screen, no Split button
//   - Timer: Start / Pause / Reset, live counting
//   - Clock: live uptime, and real NTP time once Wi-Fi is set (see below)
//   - Brightness slider, Settings screen with mode + Wi-Fi status
//   - (Split screen deleted — see above)
//   - Wi-Fi + NTP: NO credentials are hardcoded. Set them once over Serial:
//       WIFI <ssid> <password>
//     then the board saves them, connects, and syncs time. Without Wi-Fi,
//     Clock shows uptime only and says so plainly — no fake time.
// WHAT IS STILL NOT FULL, honestly:
//   - Live weather / stocks / calendar / photos: data sources (location,
//     tickers, calendar source, API keys) are not picked yet, so those tiles
//     show "Not connected yet" instead of fake data.
//   - Backlight brightness is via the IO expander (EXIO2) on this board,
//     not a plain PWM pin, so the slider may be a no-op until that path
//     is verified on hardware.
//   - Hardware test: no board is available here, so display + touch have
//     NOT been tested on the real board. Compile verification is met;
//     flashing + touch test is still required before calling it done.
//
// Dependencies: LovyanGFX, LVGL 9.x, board ESP32-S3, PSRAM enabled.
// lv_conf.h must enable LV_FONT_MONTSERRAT_24, _28, _36, _48 (see
// lv_conf-notes.txt in this folder). Board settings that matter:
//   Flash Size 16MB, Partition "16M Flash (3MB APP/9.9MB FATFS)",
//   PSRAM OPI. Compile:
//   arduino-cli compile --fqbn esp32:esp32:esp32s3:PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,FlashSize=16M wall-display
// Compile-verified 2026-10-30: ESP32 core 3.3.12, LovyanGFX 1.2.36,
// LVGL 9.2.3, 1,268,655 bytes (40% of 3MB app), 0 errors. On the default
// 1.2MB partition the same sketch is 96% full, so do NOT use default.
// Sketch folder must be named wall-display and contain wall-display.ino.

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lvgl.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>

// ---- Real-data defaults (change these, no keys needed) ----
static const float WEATHER_LAT = 34.08f, WEATHER_LON = -84.29f;  // Alpharetta, GA
static const char *STOCK_TICKERS[3] = {"nu.us", "sofi.us", "vti.us"};
static const char *STOCK_NAMES[3] = {"NU", "SOFI", "VTI"};

// ---- Display / touch config (pins from Waveshare wiki, see header) ----
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_RGB _panel_instance;
  lgfx::Bus_RGB _bus_instance;
  lgfx::Touch_GT911 _touch_instance;
 public:
  LGFX() {
    {
      auto cfg = _panel_instance.config();
      cfg.memory_width = 1024; cfg.memory_height = 600;
      cfg.panel_width = 1024; cfg.panel_height = 600;
      cfg.offset_x = 0; cfg.offset_y = 0;
      _panel_instance.config(cfg);
    }
    {
      auto cfg = _panel_instance.config_detail();
      cfg.use_psram = 1;
      _panel_instance.config_detail(cfg);
    }
    {
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;
      cfg.pin_d0 = 40; cfg.pin_d1 = 41; cfg.pin_d2 = 42; cfg.pin_d3 = 0;
      cfg.pin_d4 = 1;  cfg.pin_d5 = 2;  cfg.pin_d6 = 21; cfg.pin_d7 = 39;
      cfg.pin_d8 = 38; cfg.pin_d9 = 14; cfg.pin_d10 = 17; cfg.pin_d11 = 18;
      cfg.pin_d12 = 10; cfg.pin_d13 = 47; cfg.pin_d14 = 48; cfg.pin_d15 = 45;
      cfg.pin_henable = 5; cfg.pin_vsync = 3; cfg.pin_hsync = 46; cfg.pin_pclk = 7;
      cfg.freq_write = 15000000;
      cfg.hsync_polarity = 0; cfg.vsync_polarity = 0;
      cfg.hsync_pulse_width = 48; cfg.hsync_back_porch = 40; cfg.hsync_front_porch = 40;
      cfg.vsync_pulse_width = 31; cfg.vsync_back_porch = 13; cfg.vsync_front_porch = 1;
      cfg.pclk_active_neg = 1; cfg.de_idle_high = 0; cfg.pclk_idle_high = 0;
      _bus_instance.config(cfg);
    }
    _panel_instance.setBus(&_bus_instance);
    {
      auto cfg = _touch_instance.config();
      cfg.x_min = 0; cfg.x_max = 1024; cfg.y_min = 0; cfg.y_max = 600;
      cfg.i2c_port = 0; cfg.i2c_addr = 0x5D;
      cfg.pin_sda = 8; cfg.pin_scl = 9; cfg.pin_int = 4;
      cfg.freq = 400000;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
    setPanel(&_panel_instance);
  }
};
static LGFX tft;
static Preferences prefs;

// ---- Apple-clean palette ----
// DESIGN SYSTEM v2 — Midnight Glass (2026-11-01 sprint, Guestbook keeps its
// separate liquid-chrome look via gb_chrome, untouched by these tokens).
// BG #0B0E14, cards #1A1F2B r32, sub #9AA3B2, accent #0A84FF. No new globals:
// token-only port, so Guestbook 8 steps + 7 animations are preserved exactly.
static const uint32_t C_BG = 0x0B0E14, C_CARD = 0x1A1F2B, C_TEXT = 0xFFFFFF;
static const uint32_t C_SUB = 0x9AA3B2, C_ACCENT = 0x0A84FF, C_LINE = 0x2A2F3D;

// ---- State ----
static lv_obj_t *scr_home = nullptr, *scr_timer = nullptr, *scr_clock = nullptr;
static lv_obj_t *scr_settings = nullptr, *scr_split = nullptr, *scr_live = nullptr;
static lv_obj_t *scr_stocks = nullptr, *scr_calendar = nullptr;
static lv_obj_t *scr_camera = nullptr, *scr_web = nullptr, *scr_photos = nullptr, *scr_lock = nullptr;
static lv_obj_t *scr_tasks = nullptr, *scr_calc = nullptr, *scr_music = nullptr, *scr_notes = nullptr;
static lv_obj_t *scr_search = nullptr, *search_label = nullptr, *note_label = nullptr;
static lv_obj_t *scr_customize = nullptr;
static lv_obj_t *scr_maps = nullptr, *scr_mail = nullptr, *maps_label = nullptr, *mail_label = nullptr;
static lv_obj_t *scr_picker = nullptr, *scr_grid = nullptr, *scr_widget = nullptr, *scr_photo_view = nullptr;
static lv_obj_t *lock_panel = nullptr, *lock_time_label = nullptr, *home_face_panel = nullptr, *home_clock_label = nullptr, *home_date_label = nullptr;
static lv_obj_t *picker_preview = nullptr, *picker_tiles[6] = {nullptr}, *picker_check_label = nullptr;
static lv_obj_t *widget_date_label = nullptr, *widget_weekday_label = nullptr, *widget_clock_label = nullptr, *widget_temp_label = nullptr, *grid_weather_label = nullptr;
static lv_obj_t *cal_day_labels[31] = {nullptr};
// HOME PAGES — swipeable home feed (tileview). Page order: 0 Clock, 1 Widgets, 2 Apps. Home controls DELETED 2026-11-01.
static lv_obj_t *home_tv = nullptr, *home_tiles[4] = {nullptr}, *home_dots[4] = {nullptr};
static int home_page = 0;
static lv_obj_t *tile_widget_weekday_label = nullptr, *tile_widget_date_label = nullptr, *tile_widget_clock_label = nullptr, *tile_widget_temp_label = nullptr, *tile_grid_weather_label = nullptr;
static lv_obj_t *music_track_label = nullptr, *music_artist_label = nullptr, *music_state_label = nullptr;
static const char *MUSIC_TITLES[3] = {"Example Track 1", "Example Track 2", "Example Track 3"};
static const char *MUSIC_ARTISTS[3] = {"Example Artist", "Example Artist", "Example Artist"};
static int music_idx = 0; static bool music_playing = false;
static lv_obj_t *timer_arc = nullptr;
static lv_obj_t *photo_panel = nullptr, *photo_zoom_label = nullptr, *photo_clock_label = nullptr; static bool photo_zoom_on = false;
static const uint32_t PHOTO_COLORS[4] = {0x6FB3FF, 0x7ED491, 0xE8A87C, 0xC9B8FF};
// SECOND-REEL ADDITIONS 2026-11-01 (Sripad approved, square copy only).
// Home layout (greeting + H/L + calendar tile), photo clock, recipe,
// manual profiles. NO home controls (deleted 2026-11-01, not re-added),
// NO face recognition, NO camera, NO voice/Siri — all omitted by design.
static lv_obj_t *scr_recipe = nullptr, *scr_photoclock = nullptr;
static lv_obj_t *recipe_title_label = nullptr, *recipe_ing_labels[6] = {nullptr};
static String recipe_title = "No recipe yet";
static String recipe_ings[6] = {"", "", "", "", "", ""};
static int recipe_count = 0; static bool recipe_checked[6] = {false};
static lv_obj_t *photoclock_panel = nullptr, *photoclock_time_label = nullptr; static int photoclock_idx = 0;
static lv_obj_t *home_greeting_label = nullptr, *home_hl_label = nullptr, *home_cal_label = nullptr;
static lv_obj_t *profile_btns[6] = {nullptr};
static String profile_names[6] = {"Profile 1", "Profile 2", "Profile 3", "Profile 4", "Profile 5", "Profile 6"};
static int profile_idx = 0;
static bool ntp_ok = false;
static String hl_text = "High/low: waiting for Wi-Fi…";
static const char *greeting_for_hour(int h) { if (h >= 5 && h < 12) return "GOOD MORNING"; if (h >= 12 && h < 18) return "GOOD AFTERNOON"; if (h >= 18 && h < 22) return "GOOD EVENING"; return "GOOD NIGHT"; }
static void greeting_update() {
  if (!home_greeting_label) return;
  if (ntp_ok) { time_t now = time(nullptr); struct tm *tmv = localtime(&now); lv_label_set_text_fmt(home_greeting_label, "%s, %s", greeting_for_hour(tmv->tm_hour), profile_names[profile_idx].c_str()); }
  else lv_label_set_text_fmt(home_greeting_label, "GOOD DAY, %s", profile_names[profile_idx].c_str());
}
static void profile_pick_cb(lv_event_t *e) { int i = (int)(intptr_t)lv_event_get_user_data(e); if (i < 0 || i > 5) return; profile_idx = i; prefs.putInt("profile", profile_idx); greeting_update(); for (int j = 0; j < 6; j++) if (profile_btns[j]) lv_obj_set_style_border_width(profile_btns[j], j == profile_idx ? 4 : 0, 0); }
static void recipe_toggle_cb(lv_event_t *e) { int i = (int)(intptr_t)lv_event_get_user_data(e); if (i < 0 || i >= recipe_count) return; recipe_checked[i] = !recipe_checked[i]; if (recipe_ing_labels[i]) { lv_obj_set_style_text_color(recipe_ing_labels[i], lv_color_hex(recipe_checked[i] ? 0x98989D : C_TEXT), 0); lv_obj_set_style_text_decor(recipe_ing_labels[i], recipe_checked[i] ? LV_TEXT_DECOR_STRIKETHROUGH : LV_TEXT_DECOR_NONE, 0); } }
static void recipe_refresh() { if (recipe_title_label) lv_label_set_text(recipe_title_label, recipe_title.c_str()); for (int i = 0; i < 6; i++) if (recipe_ing_labels[i]) lv_label_set_text(recipe_ing_labels[i], i < recipe_count ? recipe_ings[i].c_str() : ""); }
static void open_recipe_cb(lv_event_t *e) { (void)e; if (scr_recipe) lv_screen_load(scr_recipe); }
static void open_photoclock_cb(lv_event_t *e) { (void)e; if (scr_photoclock) lv_screen_load(scr_photoclock); }
static void photoclock_cycle_cb(lv_event_t *e) { (void)e; photoclock_idx = (photoclock_idx + 1) % 4; if (photoclock_panel) lv_obj_set_style_bg_color(photoclock_panel, lv_color_hex(PHOTO_COLORS[photoclock_idx]), 0); }
static void recipe_timer_cb(lv_event_t *e) { (void)e; if (scr_timer) lv_screen_load(scr_timer); }


// ---- GUESTBOOK (sign-first, Sripad correction 2026-11-01) ----
// Flow: (1) draw signature on a touch pad, (2) ink-to-chrome + stamp + ripple
// + SAVED stamp animation, (3) name + who-came-with-you on keyboard,
// then Save. Saved entry: signature strokes, name, who, auto date+time.
// CHROME LOOK IS ONLY FOR THIS APP: silver gradient panels, chrome
// border, shine sweep. No other screen changes its look.
// CAPS (kept small so Preferences stays within NVS limits): 20 entries,
// oldest dropped; 8 strokes per signature; 120 points per stroke;
// 150 points total encoded per saved signature. Date+time is the live
// clock (NTP once Wi-Fi is set, otherwise uptime) — never invented.
// NO home controls (deleted 2026-11-01, not re-added), split deleted.
static lv_obj_t *label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color);
// GUESTBOOK CAPS (stated): 20 entries (oldest dropped), 8 strokes per
// signature, 120 points per stroke, 150 points total per saved signature.
static const int GB_MAX = 20, GB_STROKES = 8, GB_PTS = 120, GB_PTS_TOTAL = 150;
static const int GB_PAD_X = 252, GB_PAD_Y = 160, GB_PAD_W = 520, GB_PAD_H = 240;
static lv_obj_t *scr_guestbook = nullptr, *scr_gb_sign = nullptr, *scr_gb_anim = nullptr;
static lv_obj_t *scr_gb_name = nullptr, *scr_gb_replay = nullptr;
static lv_obj_t *scr_gb_home = nullptr, *scr_gb_gallery = nullptr, *scr_gb_detail = nullptr, *scr_gb_saved = nullptr;
static lv_obj_t *gb_list_cont = nullptr, *gb_pad_panel = nullptr, *gb_shine_bar = nullptr;
static lv_obj_t *gb_home_cont = nullptr, *gb_home_count = nullptr, *gb_gallery_cont = nullptr;
static lv_obj_t *gb_filter_btns[4] = {nullptr};
static lv_obj_t *gb_detail_panel = nullptr, *gb_detail_name = nullptr, *gb_detail_sub = nullptr, *gb_detail_star = nullptr, *gb_detail_seal = nullptr;
static lv_obj_t *gb_saved_panel = nullptr, *gb_saved_name = nullptr, *gb_saved_sub = nullptr, *gb_saved_stamp = nullptr;
static lv_obj_t *gb_anim_sig_panel = nullptr, *gb_anim_stamp = nullptr, *gb_anim_ripple = nullptr;
static lv_obj_t *gb_anim_lines[8] = {nullptr}, *gb_saved_lines[8] = {nullptr}, *gb_detail_lines[8] = {nullptr};
static lv_obj_t *gb_trace_dot = nullptr;
static lv_obj_t *gb_pad_note = nullptr, *gb_name_note = nullptr, *gb_replay_panel = nullptr, *gb_replay_info = nullptr;
static lv_obj_t *gb_name_field = nullptr, *gb_who_field = nullptr;
static lv_obj_t *gb_name_field_label = nullptr, *gb_who_field_label = nullptr;
static lv_obj_t *gb_pad_lines[8] = {nullptr}, *gb_replay_lines[8] = {nullptr};
static lv_point_precise_t gb_pad_pts[8][120];
static int gb_pad_n[8] = {0}, gb_pad_strokes = 0; static bool gb_pad_drawing = false;
static lv_point_precise_t gb_replay_pts[8][120];
static int gb_replay_n[8] = {0}, gb_replay_segs = 0, gb_replay_shown = 0;
static lv_timer_t *gb_anim_timer = nullptr, *gb_replay_timer = nullptr, *gb_anim_phase_timer = nullptr, *gb_saved_timer = nullptr;
static String gb_names[20], gb_whos[20], gb_stamps[20], gb_sigs[20];
static bool gb_favs[20] = {false};
static long gb_when[20] = {0};
static int gb_filter = 0, gb_detail_idx = -1, gb_anim_phase = 0, gb_saved_phase = 0, gb_trace_seg = 0, gb_trace_pt = 0;
static int gb_count = 0;
static String gb_name = "", gb_who = "", gb_pending_sig = "";
static int gb_active_field = 0;
static void gb_clean_str(String &s) { s.replace("|", " "); s.replace("\n", " "); s.replace("\r", " "); }
static String gb_stamp() {
  if (ntp_ok) { time_t now = time(nullptr); struct tm *tmv = localtime(&now); char buf[40]; strftime(buf, sizeof(buf), "%a %b %d %I:%M %p", tmv); return String(buf); }
  unsigned long m = millis() / 60000; return String("Uptime ") + String((unsigned long)(m / 60)) + "h " + String((unsigned long)(m % 60)) + "m";
}
static String gb_sig_encode() {
  String out; int total = 0;
  for (int s = 0; s < gb_pad_strokes && total < GB_PTS_TOTAL; s++) {
    for (int j = 0; j < gb_pad_n[s] && total < GB_PTS_TOTAL; j++) { out += String((int)gb_pad_pts[s][j].x) + "," + String((int)gb_pad_pts[s][j].y) + ";"; total++; }
    out += "-1,-1;";
  }
  return out;
}
static void gb_persist() {
  prefs.putInt("gb_count", gb_count);
  for (int i = 0; i < gb_count; i++) prefs.putString(("gb" + String(i)).c_str(), gb_names[i] + "|" + gb_whos[i] + "|" + gb_stamps[i] + "|" + gb_sigs[i] + "|" + (gb_favs[i] ? String("1") : String("0")) + "|" + String(gb_when[i]));
}
static void gb_load() {
  gb_count = 0; int c = prefs.getInt("gb_count", 0); if (c < 0) c = 0; if (c > GB_MAX) c = GB_MAX;
  for (int i = 0; i < c; i++) {
    String e = prefs.getString(("gb" + String(i)).c_str(), "");
    int p1 = e.indexOf('|'); int p2 = p1 < 0 ? -1 : e.indexOf('|', p1 + 1); int p3 = p2 < 0 ? -1 : e.indexOf('|', p2 + 1);
    if (p1 < 0 || p2 < 0 || p3 < 0) continue;
    int p4 = e.indexOf('|', p3 + 1);
    gb_names[gb_count] = e.substring(0, p1); gb_whos[gb_count] = e.substring(p1 + 1, p2);
    gb_stamps[gb_count] = e.substring(p2 + 1, p3);
    gb_sigs[gb_count] = p4 < 0 ? e.substring(p3 + 1) : e.substring(p3 + 1, p4);
    gb_favs[gb_count] = false; gb_when[gb_count] = 0;
    if (p4 >= 0) { int p5 = e.indexOf('|', p4 + 1); gb_favs[gb_count] = e.substring(p4 + 1, p5 < 0 ? e.length() : p5) == "1"; if (p5 >= 0) gb_when[gb_count] = e.substring(p5 + 1).toInt(); }
    gb_count++;
  }
}
static void gb_pad_clear() {
  for (int s = 0; s < GB_STROKES; s++) { if (gb_pad_lines[s]) { lv_obj_delete(gb_pad_lines[s]); gb_pad_lines[s] = nullptr; } gb_pad_n[s] = 0; }
  gb_pad_strokes = 0; gb_pad_drawing = false;
}
static void gb_fields_refresh() {
  if (gb_name_field_label) lv_label_set_text(gb_name_field_label, ("Name: " + gb_name).c_str());
  if (gb_who_field_label) lv_label_set_text(gb_who_field_label, ("Who came with you: " + gb_who).c_str());
  if (gb_name_field) lv_obj_set_style_border_width(gb_name_field, gb_active_field == 0 ? 3 : 1, 0);
  if (gb_who_field) lv_obj_set_style_border_width(gb_who_field, gb_active_field == 1 ? 3 : 1, 0);
}

static void gb_entry_cb(lv_event_t *e);
static void gb_replay_cb(lv_event_t *e);
static void gb_open_guestbook_cb(lv_event_t *e);
static void gb_open_gallery_cb(lv_event_t *e);
static void gb_back_to_detail_cb(lv_event_t *e);
static void gb_open_sign_cb(lv_event_t *e);
static void gb_back_to_pad_cb(lv_event_t *e);
static void gb_pad_cb(lv_event_t *e);
static void gb_pad_done_cb(lv_event_t *e);
static void gb_pad_clear_cb(lv_event_t *e);
static void gb_pad_undo_cb(lv_event_t *e);
static void gb_key_cb(lv_event_t *e);
static void gb_field_cb(lv_event_t *e);
static void gb_replay_build(int idx);
static void gb_replay_start();
static void gb_anim_phase_cb(lv_timer_t *t);
static void gb_save_cb(lv_event_t *e) {
  (void)e; String nm = gb_name; nm.trim(); if (nm.length() == 0) { if (gb_name_note) lv_label_set_text(gb_name_note, "Enter a name first"); return; }
  gb_clean_str(nm); String wh = gb_who; gb_clean_str(wh);
  int lim = gb_count < GB_MAX ? gb_count : GB_MAX - 1;
  for (int i = lim - 1; i >= 0; i--) { gb_names[i + 1] = gb_names[i]; gb_whos[i + 1] = gb_whos[i]; gb_stamps[i + 1] = gb_stamps[i]; gb_sigs[i + 1] = gb_sigs[i]; gb_favs[i + 1] = gb_favs[i]; gb_when[i + 1] = gb_when[i]; }
  gb_names[0] = nm; gb_whos[0] = wh; gb_stamps[0] = gb_stamp(); gb_sigs[0] = gb_pending_sig; gb_favs[0] = false; gb_when[0] = ntp_ok ? (long)time(nullptr) : 0;
  if (gb_count < GB_MAX) gb_count++;
  gb_persist(); gb_detail_idx = 0; gb_saved_build(0); gb_home_rebuild(); gb_gallery_rebuild();
  gb_pad_clear(); gb_name = ""; gb_who = ""; gb_pending_sig = ""; gb_fields_refresh();
  if (scr_gb_saved) lv_screen_load(scr_gb_saved);
  gb_saved_phase = 0; if (gb_saved_timer) { lv_timer_reset(gb_saved_timer); lv_timer_resume(gb_saved_timer); }
}
static void gb_saved_timer_cb(lv_timer_t *tm) {
  (void)tm; gb_saved_phase++;
  // ANIM 5 card develops: faint, signature darkens in, then name, then date.
  if (gb_saved_phase == 1) { for (int s = 0; s < GB_STROKES; s++) if (gb_saved_lines[s]) lv_obj_set_style_line_opa(gb_saved_lines[s], 128, 0); }
  if (gb_saved_phase == 2) { for (int s = 0; s < GB_STROKES; s++) if (gb_saved_lines[s]) lv_obj_set_style_line_opa(gb_saved_lines[s], LV_OPA_COVER, 0); }
  if (gb_saved_phase == 3 && gb_saved_name) lv_obj_clear_flag(gb_saved_name, LV_OBJ_FLAG_HIDDEN);
  if (gb_saved_phase >= 4) { if (gb_saved_sub) lv_obj_clear_flag(gb_saved_sub, LV_OBJ_FLAG_HIDDEN); lv_timer_pause(tm); }
}
static void gb_fav_cb(lv_event_t *e) {
  (void)e; int i = gb_detail_idx; if (i < 0 || i >= gb_count) return;
  gb_favs[i] = !gb_favs[i]; gb_persist(); gb_gallery_rebuild();
  if (gb_detail_star) lv_label_set_text(gb_detail_star, gb_favs[i] ? "Favorited *" : "Favorite");
  if (gb_detail_seal) {
    if (gb_favs[i]) {
      // ANIM 6 chrome seal favorite: seal pops onto the card corner.
      lv_obj_clear_flag(gb_detail_seal, LV_OBJ_FLAG_HIDDEN);
      lv_anim_t a; lv_anim_init(&a); lv_anim_set_var(&a, gb_detail_seal); lv_anim_set_exec_cb(&a, [](void *o, int32_t v) { lv_obj_set_style_transform_zoom((lv_obj_t *)o, v, 0); }); lv_anim_set_values(&a, 0, 256); lv_anim_set_duration(&a, 300); lv_anim_start(&a);
    } else lv_obj_add_flag(gb_detail_seal, LV_OBJ_FLAG_HIDDEN);
  }
}
static void gb_filter_cb(lv_event_t *e) { gb_filter = (int)(intptr_t)lv_event_get_user_data(e); gb_gallery_rebuild(); }
static bool gb_in_week(int i) {
  if (gb_when[i] == 0) return true;
  if (!ntp_ok) return true;
  long now = (long)time(nullptr); return now >= gb_when[i] && (now - gb_when[i]) < 7L * 86400L;
}
static void gb_decode_sig(String sig, lv_obj_t *panel, lv_obj_t **lines_out) {
  for (int s = 0; s < GB_STROKES; s++) { if (lines_out[s]) { lv_obj_delete(lines_out[s]); lines_out[s] = nullptr; } gb_replay_n[s] = 0; }
  int seg = 0, pos = 0;
  while (pos < (int)sig.length() && seg < GB_STROKES) {
    int semi = sig.indexOf(';', pos); if (semi < 0) break;
    String tok = sig.substring(pos, semi); pos = semi + 1;
    if (tok == "-1,-1") { if (gb_replay_n[seg] > 0) seg++; continue; }
    int comma = tok.indexOf(','); if (comma < 0) continue;
    if (gb_replay_n[seg] < GB_PTS) { gb_replay_pts[seg][gb_replay_n[seg]].x = tok.substring(0, comma).toInt(); gb_replay_pts[seg][gb_replay_n[seg]].y = tok.substring(comma + 1).toInt(); gb_replay_n[seg]++; }
  }
  int segs = gb_replay_n[seg] > 0 ? seg + 1 : seg;
  if (!panel) return;
  for (int s = 0; s < segs; s++) {
    lv_obj_t *ln = lv_line_create(panel);
    lv_obj_set_style_line_width(ln, 4, 0); lv_obj_set_style_line_color(ln, lv_color_hex(0x111111), 0); lv_obj_set_style_line_rounded(ln, true, 0);
    lv_line_set_points_mutable(ln, gb_replay_pts[s], gb_replay_n[s]); lines_out[s] = ln;
  }
}
static void gb_home_rebuild() {
  if (gb_home_count) lv_label_set_text_fmt(gb_home_count, "%03d MEMORIES", gb_count);
  if (!gb_home_cont) return;
  lv_obj_clean(gb_home_cont);
  int n = gb_count < 3 ? gb_count : 3;
  for (int i = 0; i < n; i++) {
    lv_obj_t *b = lv_button_create(gb_home_cont); lv_obj_set_size(b, 520, 64); lv_obj_set_pos(b, 0, i * 72);
    lv_obj_set_style_bg_color(b, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_bg_opa(b, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(b, 20, 0); lv_obj_set_style_border_width(b, 1, 0); lv_obj_set_style_border_color(b, lv_color_hex(0xC0C0C8), 0);
    lv_obj_t *nm = lv_label_create(b); lv_label_set_text(nm, gb_names[i].c_str()); lv_obj_set_style_text_font(nm, &lv_font_montserrat_24, 0); lv_obj_set_style_text_color(nm, lv_color_hex(0x111111), 0); lv_obj_set_pos(nm, 16, 6);
    String sub = gb_stamps[i] + "  •  " + (gb_whos[i].length() == 0 ? String("alone") : String("with ") + gb_whos[i]);
    lv_obj_t *sb = lv_label_create(b); lv_label_set_text(sb, sub.c_str()); lv_obj_set_style_text_font(sb, &lv_font_montserrat_14, 0); lv_obj_set_style_text_color(sb, lv_color_hex(0x555555), 0); lv_obj_set_pos(sb, 16, 36);
    lv_obj_add_event_cb(b, gb_entry_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
  }
}
static void gb_gallery_rebuild() {
  const char *fn[4] = {"All", "Favorites", "With friends", "This week"};
  for (int f = 0; f < 4; f++) if (gb_filter_btns[f]) { lv_obj_set_style_bg_color(gb_filter_btns[f], lv_color_hex(gb_filter == f ? 0x111111 : 0xFFFFFF), 0); lv_obj_t *l = lv_obj_get_child(gb_filter_btns[f], 0); if (l) lv_obj_set_style_text_color(l, lv_color_hex(gb_filter == f ? 0xFFFFFF : 0x111111), 0); }
  if (!gb_gallery_cont) return;
  lv_obj_clean(gb_gallery_cont);
  int y = 0;
  for (int i = 0; i < gb_count; i++) {
    if (gb_filter == 1 && !gb_favs[i]) continue;
    if (gb_filter == 2 && gb_whos[i].length() == 0) continue;
    if (gb_filter == 3 && !gb_in_week(i)) continue;
    lv_obj_t *b = lv_button_create(gb_gallery_cont); lv_obj_set_size(b, 520, 72); lv_obj_set_pos(b, 0, y); y += 80;
    lv_obj_set_style_bg_color(b, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_bg_opa(b, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(b, 20, 0); lv_obj_set_style_border_width(b, 1, 0); lv_obj_set_style_border_color(b, lv_color_hex(0xC0C0C8), 0);
    String nm = gb_names[i] + (gb_favs[i] ? String(" *") : String(""));
    lv_obj_t *nl = lv_label_create(b); lv_label_set_text(nl, nm.c_str()); lv_obj_set_style_text_font(nl, &lv_font_montserrat_24, 0); lv_obj_set_style_text_color(nl, lv_color_hex(0x111111), 0); lv_obj_set_pos(nl, 16, 6);
    String sub = gb_stamps[i] + "  •  " + (gb_whos[i].length() == 0 ? String("alone") : String("with ") + gb_whos[i]);
    lv_obj_t *sl = lv_label_create(b); lv_label_set_text(sl, sub.c_str()); lv_obj_set_style_text_font(sl, &lv_font_montserrat_14, 0); lv_obj_set_style_text_color(sl, lv_color_hex(0x555555), 0); lv_obj_set_pos(sl, 16, 36);
    lv_obj_add_event_cb(b, gb_entry_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
  }
  (void)fn;
}
static void gb_detail_build(int idx) {
  if (idx < 0 || idx >= gb_count) return;
  gb_detail_idx = idx;
  if (gb_detail_name) lv_label_set_text(gb_detail_name, gb_names[idx].c_str());
  if (gb_detail_sub) lv_label_set_text(gb_detail_sub, (gb_stamps[idx] + "  •  " + (gb_whos[idx].length() == 0 ? String("alone") : String("with ") + gb_whos[idx])).c_str());
  if (gb_detail_star) lv_label_set_text(gb_detail_star, gb_favs[idx] ? "Favorited *" : "Favorite");
  if (gb_detail_seal) { if (gb_favs[idx]) lv_obj_clear_flag(gb_detail_seal, LV_OBJ_FLAG_HIDDEN); else lv_obj_add_flag(gb_detail_seal, LV_OBJ_FLAG_HIDDEN); }
  gb_decode_sig(gb_sigs[idx], gb_detail_panel, gb_detail_lines);
}
static void gb_saved_build(int idx) {
  if (idx < 0 || idx >= gb_count) return;
  gb_decode_sig(gb_sigs[idx], gb_saved_panel, gb_saved_lines);
  for (int s = 0; s < GB_STROKES; s++) if (gb_saved_lines[s]) lv_obj_set_style_line_opa(gb_saved_lines[s], 0, 0);
  if (gb_saved_name) { lv_label_set_text(gb_saved_name, gb_names[idx].c_str()); lv_obj_add_flag(gb_saved_name, LV_OBJ_FLAG_HIDDEN); }
  if (gb_saved_sub) { lv_label_set_text(gb_saved_sub, (gb_stamps[idx] + "  •  " + (gb_whos[idx].length() == 0 ? String("alone") : String("with ") + gb_whos[idx])).c_str()); lv_obj_add_flag(gb_saved_sub, LV_OBJ_FLAG_HIDDEN); }
}

static void gb_replay_build(int idx) {
  for (int s = 0; s < GB_STROKES; s++) { if (gb_replay_lines[s]) { lv_obj_delete(gb_replay_lines[s]); gb_replay_lines[s] = nullptr; } gb_replay_n[s] = 0; }
  gb_replay_segs = 0; gb_replay_shown = 0;
  if (idx < 0 || idx >= gb_count) return;
  if (gb_replay_info) lv_label_set_text(gb_replay_info, (gb_names[idx] + "  •  " + gb_stamps[idx]).c_str());
  String sig = gb_sigs[idx]; int seg = 0, pos = 0;
  while (pos < (int)sig.length() && seg < GB_STROKES) {
    int semi = sig.indexOf(';', pos); if (semi < 0) break;
    String tok = sig.substring(pos, semi); pos = semi + 1;
    if (tok == "-1,-1") { if (gb_replay_n[seg] > 0) seg++; continue; }
    int comma = tok.indexOf(','); if (comma < 0) continue;
    if (gb_replay_n[seg] < GB_PTS) { gb_replay_pts[seg][gb_replay_n[seg]].x = tok.substring(0, comma).toInt(); gb_replay_pts[seg][gb_replay_n[seg]].y = tok.substring(comma + 1).toInt(); gb_replay_n[seg]++; }
  }
  gb_replay_segs = gb_replay_n[seg] > 0 ? seg + 1 : seg;
  if (!gb_replay_panel) return;
  for (int s = 0; s < gb_replay_segs; s++) {
    lv_obj_t *ln = lv_line_create(gb_replay_panel);
    lv_obj_set_style_line_width(ln, 4, 0); lv_obj_set_style_line_color(ln, lv_color_hex(0x111111), 0); lv_obj_set_style_line_rounded(ln, true, 0);
    lv_line_set_points_mutable(ln, gb_replay_pts[s], gb_replay_n[s]); lv_obj_add_flag(ln, LV_OBJ_FLAG_HIDDEN); gb_replay_lines[s] = ln;
  }
}
static void gb_replay_timer_cb(lv_timer_t *t) {
  // ANIM 4 light trace: a bright dot traces the exact signature path, and
  // each stroke reveals point by point; the full signature is visible at the end.
  if (gb_trace_seg >= gb_replay_segs) { if (gb_trace_dot) lv_obj_add_flag(gb_trace_dot, LV_OBJ_FLAG_HIDDEN); lv_timer_pause(t); return; }
  lv_obj_t *ln = gb_replay_lines[gb_trace_seg];
  if (!ln) { gb_trace_seg++; gb_trace_pt = 0; return; }
  if (gb_trace_pt < gb_replay_n[gb_trace_seg]) {
    gb_trace_pt++;
    lv_line_set_points_mutable(ln, gb_replay_pts[gb_trace_seg], gb_trace_pt);
    lv_obj_clear_flag(ln, LV_OBJ_FLAG_HIDDEN);
    if (gb_trace_dot) { lv_obj_clear_flag(gb_trace_dot, LV_OBJ_FLAG_HIDDEN); lv_obj_set_pos(gb_trace_dot, GB_PAD_X + (int)gb_replay_pts[gb_trace_seg][gb_trace_pt - 1].x - 6, GB_PAD_Y + (int)gb_replay_pts[gb_trace_seg][gb_trace_pt - 1].y - 6); }
  } else { gb_trace_seg++; gb_trace_pt = 0; }
}
static void gb_replay_start() {
  gb_trace_seg = 0; gb_trace_pt = 0; gb_replay_shown = 0;
  for (int s = 0; s < gb_replay_segs; s++) if (gb_replay_lines[s]) { lv_line_set_points_mutable(gb_replay_lines[s], gb_replay_pts[s], gb_replay_n[s] > 0 ? 1 : 0); lv_obj_add_flag(gb_replay_lines[s], LV_OBJ_FLAG_HIDDEN); }
  if (gb_trace_dot) lv_obj_add_flag(gb_trace_dot, LV_OBJ_FLAG_HIDDEN);
  if (gb_replay_timer) { lv_timer_reset(gb_replay_timer); lv_timer_resume(gb_replay_timer); }
}
static void gb_entry_cb(lv_event_t *e) { int i = (int)(intptr_t)lv_event_get_user_data(e); gb_detail_build(i); if (scr_gb_detail) lv_screen_load(scr_gb_detail); }
static void gb_replay_cb(lv_event_t *e) { (void)e; int i = gb_detail_idx; if (i < 0) i = 0; gb_replay_build(i); if (scr_gb_replay) lv_screen_load(scr_gb_replay); gb_replay_start(); }
static void gb_open_guestbook_cb(lv_event_t *e) { (void)e; gb_home_rebuild(); if (scr_gb_home) lv_screen_load(scr_gb_home); }
static void gb_open_gallery_cb(lv_event_t *e) { (void)e; gb_gallery_rebuild(); if (scr_gb_gallery) lv_screen_load(scr_gb_gallery); }
static void gb_back_to_detail_cb(lv_event_t *e) { (void)e; if (gb_detail_idx >= 0 && scr_gb_detail) lv_screen_load(scr_gb_detail); else { gb_home_rebuild(); if (scr_gb_home) lv_screen_load(scr_gb_home); } }
static void gb_open_sign_cb(lv_event_t *e) { (void)e; gb_pad_clear(); gb_name = ""; gb_who = ""; gb_active_field = 0; gb_fields_refresh(); if (gb_name_note) lv_label_set_text(gb_name_note, ""); if (gb_pad_note) lv_label_set_text(gb_pad_note, ""); if (scr_gb_sign) lv_screen_load(scr_gb_sign); }
static void gb_back_to_pad_cb(lv_event_t *e) { (void)e; if (scr_gb_sign) lv_screen_load(scr_gb_sign); }
static void gb_pad_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_PRESSED) {
    if (gb_pad_strokes < GB_STROKES) gb_pad_drawing = true;
    // ANIM 7 pad breathes while signing: silver glow edge while finger is down.
    if (gb_pad_panel) { lv_obj_set_style_border_width(gb_pad_panel, 4, 0); lv_obj_set_style_border_color(gb_pad_panel, lv_color_hex(0xC0C0C8), 0); }
    return;
  }
  if (code == LV_EVENT_RELEASED) { gb_pad_drawing = false; if (gb_pad_strokes < GB_STROKES && gb_pad_n[gb_pad_strokes] > 0) gb_pad_strokes++; if (gb_pad_panel) { lv_obj_set_style_border_width(gb_pad_panel, 2, 0); lv_obj_set_style_border_color(gb_pad_panel, lv_color_hex(0x9A9AA2), 0); } return; }
  if (code != LV_EVENT_PRESSING || !gb_pad_drawing) return;
  lv_indev_t *indev = lv_indev_get_act(); if (!indev) return;
  lv_point_t pt; lv_indev_get_point(indev, &pt);
  int lx = (int)pt.x - GB_PAD_X, ly = (int)pt.y - GB_PAD_Y;
  if (lx < 0 || ly < 0 || lx > GB_PAD_W || ly > GB_PAD_H) return;
  if (gb_pad_strokes >= GB_STROKES) return;
  int s = gb_pad_strokes;
  if (!gb_pad_lines[s]) {
    lv_obj_t *ln = lv_line_create(gb_pad_panel);
    lv_obj_set_style_line_width(ln, 4, 0); lv_obj_set_style_line_color(ln, lv_color_hex(0x111111), 0); lv_obj_set_style_line_rounded(ln, true, 0);
    gb_pad_lines[s] = ln; gb_pad_n[s] = 0;
  }
  if (gb_pad_n[s] > 0) { int px = (int)gb_pad_pts[s][gb_pad_n[s] - 1].x, py = (int)gb_pad_pts[s][gb_pad_n[s] - 1].y; int dx = lx - px, dy = ly - py; if (dx * dx + dy * dy < 16) return; }
  if (gb_pad_n[s] >= GB_PTS) return;
  gb_pad_pts[s][gb_pad_n[s]].x = lx; gb_pad_pts[s][gb_pad_n[s]].y = ly; gb_pad_n[s]++;
  lv_line_set_points_mutable(gb_pad_lines[s], gb_pad_pts[s], gb_pad_n[s]);
}
static void gb_shine_exec_cb(void *obj, int32_t v) { lv_obj_set_x((lv_obj_t *)obj, v); }
static void gb_anim_timer_cb(lv_timer_t *t) { lv_timer_pause(t); if (gb_anim_phase_timer) lv_timer_pause(gb_anim_phase_timer); if (scr_gb_name) lv_screen_load(scr_gb_name); }
static void gb_anim_tap_cb(lv_event_t *e) { (void)e; if (gb_anim_timer) lv_timer_pause(gb_anim_timer); if (gb_anim_phase_timer) lv_timer_pause(gb_anim_phase_timer); if (scr_gb_name) lv_screen_load(scr_gb_name); }
static void gb_anim_phase_cb(lv_timer_t *t) {
  (void)t; gb_anim_phase++;
  if (gb_anim_phase == 9) {
    // ANIM 1 ink-to-chrome: sheen has swept; the ink turns metallic silver.
    for (int s = 0; s < GB_STROKES; s++) if (gb_anim_lines[s]) lv_obj_set_style_line_color(gb_anim_lines[s], lv_color_hex(0xC0C0C8), 0);
    // ANIM 2 stamp press: stamp drops onto the card.
    if (gb_anim_stamp) { lv_obj_clear_flag(gb_anim_stamp, LV_OBJ_FLAG_HIDDEN); lv_anim_t a; lv_anim_init(&a); lv_anim_set_var(&a, gb_anim_stamp); lv_anim_set_exec_cb(&a, [](void *o, int32_t v) { lv_obj_set_y((lv_obj_t *)o, v); }); lv_anim_set_values(&a, 60, 220); lv_anim_set_duration(&a, 300); lv_anim_start(&a); }
  }
  if (gb_anim_phase >= 15 && gb_anim_ripple) {
    // ANIM 3 liquid chrome ripple: a ring spreads from the signature, background only.
    lv_obj_clear_flag(gb_anim_ripple, LV_OBJ_FLAG_HIDDEN);
    int r = 30 + (gb_anim_phase - 15) * 28; if (r > 280) r = 280;
    lv_obj_set_size(gb_anim_ripple, r * 2, r * 2); lv_obj_set_pos(gb_anim_ripple, 512 - r, 300 - r);
    lv_obj_set_style_bg_opa(gb_anim_ripple, (lv_opa_t)(r >= 280 ? 0 : 120), 0);
  }
}
static void gb_pad_done_cb(lv_event_t *e) {
  (void)e; int total = 0; for (int s = 0; s < gb_pad_strokes; s++) total += gb_pad_n[s];
  if (total < 2) { if (gb_pad_note) lv_label_set_text(gb_pad_note, "Draw your signature first"); return; }
  gb_pending_sig = gb_sig_encode();
  if (gb_anim_sig_panel) {
    for (int s = 0; s < GB_STROKES; s++) { if (gb_anim_lines[s]) { lv_obj_delete(gb_anim_lines[s]); gb_anim_lines[s] = nullptr; } }
    for (int s = 0; s < gb_pad_strokes; s++) {
      lv_obj_t *ln = lv_line_create(gb_anim_sig_panel);
      lv_obj_set_style_line_width(ln, 4, 0); lv_obj_set_style_line_color(ln, lv_color_hex(0x111111), 0); lv_obj_set_style_line_rounded(ln, true, 0);
      lv_line_set_points_mutable(ln, gb_pad_pts[s], gb_pad_n[s]); gb_anim_lines[s] = ln;
    }
  }
  if (gb_anim_stamp) { lv_obj_add_flag(gb_anim_stamp, LV_OBJ_FLAG_HIDDEN); lv_obj_set_y(gb_anim_stamp, 60); }
  if (gb_anim_ripple) lv_obj_add_flag(gb_anim_ripple, LV_OBJ_FLAG_HIDDEN);
  if (gb_shine_bar) { lv_obj_set_x(gb_shine_bar, -160); lv_anim_t a; lv_anim_init(&a); lv_anim_set_var(&a, gb_shine_bar); lv_anim_set_exec_cb(&a, gb_shine_exec_cb); lv_anim_set_values(&a, -160, 900); lv_anim_set_duration(&a, 900); lv_anim_set_delay(&a, 300); lv_anim_start(&a); }
  gb_anim_phase = 0; if (gb_anim_phase_timer) { lv_timer_reset(gb_anim_phase_timer); lv_timer_resume(gb_anim_phase_timer); }
  if (gb_anim_timer) { lv_timer_reset(gb_anim_timer); lv_timer_resume(gb_anim_timer); }
  if (scr_gb_anim) lv_screen_load(scr_gb_anim);
}
static void gb_pad_clear_cb(lv_event_t *e) { (void)e; gb_pad_clear(); if (gb_pad_note) lv_label_set_text(gb_pad_note, ""); }
static void gb_pad_undo_cb(lv_event_t *e) { (void)e; if (gb_pad_strokes > 0) { gb_pad_strokes--; if (gb_pad_lines[gb_pad_strokes]) { lv_obj_delete(gb_pad_lines[gb_pad_strokes]); gb_pad_lines[gb_pad_strokes] = nullptr; } gb_pad_n[gb_pad_strokes] = 0; } else if (gb_pad_n[0] > 0) { if (gb_pad_lines[0]) { lv_obj_delete(gb_pad_lines[0]); gb_pad_lines[0] = nullptr; } gb_pad_n[0] = 0; } if (gb_pad_note) lv_label_set_text(gb_pad_note, ""); }
static void gb_key_cb(lv_event_t *e) {
  int c = (int)(intptr_t)lv_event_get_user_data(e); String &f = gb_active_field == 0 ? gb_name : gb_who;
  if (c == 8) { if (f.length() > 0) f.remove(f.length() - 1); }
  else if (f.length() < 28) { f += (char)c; }
  if (gb_name_note) lv_label_set_text(gb_name_note, ""); gb_fields_refresh();
}
static void gb_field_cb(lv_event_t *e) { gb_active_field = (int)(intptr_t)lv_event_get_user_data(e); gb_fields_refresh(); }
static const uint32_t FACE_COLORS[6] = {0x6FB3FF, 0x7ED491, 0xC9B8FF, 0xFFFFFF, 0x111111, 0xFF9500};
static const char *FACE_NAMES[6] = {"Blue", "Green", "Lavender", "White", "Black", "Orange"};
static int face_idx = 0;
static void apply_face() {
  uint32_t c = FACE_COLORS[face_idx];
  bool dark_face = (face_idx == 4);
  uint32_t tc = dark_face ? 0xFFFFFF : 0x111111;
  if (lock_panel) lv_obj_set_style_bg_color(lock_panel, lv_color_hex(c), 0);
  if (home_face_panel) lv_obj_set_style_bg_color(home_face_panel, lv_color_hex(c), 0);
  if (picker_preview) lv_obj_set_style_bg_color(picker_preview, lv_color_hex(c), 0);
  if (lock_time_label) lv_obj_set_style_text_color(lock_time_label, lv_color_hex(tc), 0);
  if (home_clock_label) lv_obj_set_style_text_color(home_clock_label, lv_color_hex(tc), 0);
  if (home_date_label) lv_obj_set_style_text_color(home_date_label, lv_color_hex(tc), 0);
  if (picker_check_label) lv_label_set_text_fmt(picker_check_label, "✓ %s selected", FACE_NAMES[face_idx]);
  for (int i = 0; i < 6; i++) if (picker_tiles[i]) lv_obj_set_style_border_width(picker_tiles[i], i == face_idx ? 4 : 0, 0);
}
static void face_pick_cb(lv_event_t *e) {
  int i = (int)(intptr_t)lv_event_get_user_data(e);
  face_idx = i; prefs.putInt("face", face_idx); apply_face();
}
static void open_picker_cb(lv_event_t *e) { (void)e; if (scr_picker) lv_screen_load(scr_picker); }
static void open_grid_cb(lv_event_t *e) { (void)e; if (scr_grid) lv_screen_load(scr_grid); }
static void open_widget_cb(lv_event_t *e) { (void)e; if (scr_widget) lv_screen_load(scr_widget); }
static void open_notes_cb(lv_event_t *e) { (void)e; if (scr_notes) lv_screen_load(scr_notes); }
static void open_clock_cb(lv_event_t *e) { (void)e; if (scr_clock) lv_screen_load(scr_clock); }
static void home_dots_update() {
  for (int i = 0; i < 4; i++) if (home_dots[i]) lv_obj_set_style_bg_color(home_dots[i], lv_color_hex(i == home_page ? 0x0A84FF : 0x8E8E93), 0);
}
static void home_tv_scroll_cb(lv_event_t *e) {
  (void)e; if (!home_tv) return;
  lv_obj_t *act = lv_tileview_get_tile_active(home_tv);
  for (int i = 0; i < 4; i++) if (home_tiles[i] == act) { home_page = i; break; }
  home_dots_update();
}
static void home_page_prev_cb(lv_event_t *e) { (void)e; if (!home_tv || home_page <= 0) return; home_page--; lv_tileview_set_tile_by_index(home_tv, home_page, 0, LV_ANIM_ON); home_dots_update(); }
static void home_page_next_cb(lv_event_t *e) { (void)e; if (!home_tv || home_page >= 2) return; home_page++; lv_tileview_set_tile_by_index(home_tv, home_page, 0, LV_ANIM_ON); home_dots_update(); }
static void music_prev_cb(lv_event_t *e) { (void)e; music_idx = (music_idx + 2) % 3; if (music_track_label) lv_label_set_text(music_track_label, MUSIC_TITLES[music_idx]); if (music_artist_label) lv_label_set_text(music_artist_label, MUSIC_ARTISTS[music_idx]); }
static void music_next_cb(lv_event_t *e) { (void)e; music_idx = (music_idx + 1) % 3; if (music_track_label) lv_label_set_text(music_track_label, MUSIC_TITLES[music_idx]); if (music_artist_label) lv_label_set_text(music_artist_label, MUSIC_ARTISTS[music_idx]); }
static void music_toggle_cb(lv_event_t *e) { (void)e; music_playing = !music_playing; if (music_state_label) lv_label_set_text(music_state_label, music_playing ? "Playing (display only)" : "Paused (display only)"); }
static void photo_open_cb(lv_event_t *e) { int i = (int)(intptr_t)lv_event_get_user_data(e); if (photo_panel) lv_obj_set_style_bg_color(photo_panel, lv_color_hex(PHOTO_COLORS[i % 4]), 0); if (scr_photo_view) lv_screen_load(scr_photo_view); }
static void photo_zoom_cb(lv_event_t *e) { (void)e; photo_zoom_on = !photo_zoom_on; if (photo_panel) lv_obj_set_size(photo_panel, photo_zoom_on ? 560 : 500, photo_zoom_on ? 460 : 400); if (photo_zoom_label) lv_label_set_text(photo_zoom_label, photo_zoom_on ? "Zoom: 2x" : "Zoom: 1x"); }
static void photo_back_cb(lv_event_t *e) { (void)e; if (scr_photos) lv_screen_load(scr_photos); }
static float map_lat = 34.08f, map_lon = -84.29f; static int map_zoom = 15;
static void fetch_maps();
static void fetch_mail();
static void theme_dark_cb(lv_event_t *e) { (void)e; prefs.putString("theme", "dark"); lv_obj_set_style_bg_color(scr_home, lv_color_hex(0x000000), 0); }
static void theme_light_cb(lv_event_t *e) { (void)e; prefs.putString("theme", "light"); lv_obj_set_style_bg_color(scr_home, lv_color_hex(0xF5F5F7), 0); }
static void theme_tint_cb(lv_event_t *e) { (void)e; prefs.putString("theme", "tinted"); lv_obj_set_style_bg_color(scr_home, lv_color_hex(0x1A1A2E), 0); }
static void customize_cb(lv_event_t *e) { (void)e; lv_screen_load(scr_customize); }
static lv_obj_t *cust_btn = nullptr;
static void home_longpress_cb(lv_event_t *e) { (void)e; if (cust_btn) lv_obj_clear_flag(cust_btn, LV_OBJ_FLAG_HIDDEN); }
static void fetch_search(const String &term) {
  if (WiFi.status() != WL_CONNECTED) { if (search_label) lv_label_set_text(search_label, "Search: Wi-Fi not connected."); return; }
  String t = term; t.replace(" ", "_");
  String j = http_get(String("https://en.wikipedia.org/api/rest_v1/page/summary/") + t);
  int ei = j.indexOf("\"extract\":\"");
  if (ei < 0) { if (search_label) lv_label_set_text(search_label, "Search: no summary found."); return; }
  int start = ei + 11, end = j.indexOf("\",\"", start);
  String ex = j.substring(start, end < 0 ? j.length() : end);
  ex.replace("\\\"", "\"");
  if (search_label) lv_label_set_text(search_label, ex.c_str());
}
static lv_obj_t *calc_display = nullptr;
static double calc_acc = 0, calc_cur = 0; static char calc_op = 0; static bool calc_fresh = true;
static void calc_press_cb(lv_event_t *e) {
  const char *k = (const char *)lv_event_get_user_data(e);
  if (!calc_display) return;
  if (k[0] >= '0' && k[0] <= '9') {
    if (calc_fresh) { calc_cur = 0; calc_fresh = false; }
    calc_cur = calc_cur * 10 + (k[0] - '0');
    lv_label_set_text_fmt(calc_display, "%g", calc_cur);
  } else if (k[0] == 'C') { calc_acc = 0; calc_cur = 0; calc_op = 0; calc_fresh = true; lv_label_set_text(calc_display, "0"); }
  else if (k[0] == '=') {
    double r = calc_cur;
    if (calc_op == '+') r = calc_acc + calc_cur; else if (calc_op == '-') r = calc_acc - calc_cur;
    else if (calc_op == '*') r = calc_acc * calc_cur; else if (calc_op == '/' && calc_cur != 0) r = calc_acc / calc_cur;
    lv_label_set_text_fmt(calc_display, "%g", r); calc_cur = r; calc_op = 0; calc_fresh = true;
  } else {
    calc_acc = calc_cur; calc_op = k[0]; calc_fresh = true;
  }
}
static void task_toggle_cb(lv_event_t *e) {
  lv_obj_t *l = (lv_obj_t *)lv_event_get_user_data(e);
  const char *t = lv_label_get_text(l);
  if (t[0] != 'D') {  // not yet marked done (labels start with a task name)
    lv_obj_set_style_text_color(l, lv_color_hex(0x98989D), 0);
    lv_obj_set_style_text_decor(l, LV_TEXT_DECOR_STRIKETHROUGH, 0);
  }
}
static lv_obj_t *seg_single = nullptr, *seg_split = nullptr;
static lv_obj_t *timer_label = nullptr, *clock_label = nullptr, *clock_sub = nullptr;
static lv_obj_t *sw_label = nullptr, *alarm_label = nullptr, *stocks_chart = nullptr;
static lv_chart_series_t *stocks_series = nullptr;
static bool sw_running = false; static uint32_t sw_start_ms = 0, sw_accum_ms = 0;
static int alarm_h = -1, alarm_m = -1;
static void sw_start_cb(lv_event_t *e) { (void)e; if (!sw_running) { sw_running = true; sw_start_ms = millis(); } }
static void sw_reset_cb(lv_event_t *e) { (void)e; sw_running = false; sw_accum_ms = 0; if (sw_label) lv_label_set_text(sw_label, "00:00.00"); }
static void sw_lap_cb(lv_event_t *e) { (void)e; if (sw_running) { sw_accum_ms += millis() - sw_start_ms; sw_start_ms = millis(); } }
static lv_obj_t *split_timer_label = nullptr, *split_clock_label = nullptr;
static lv_obj_t *wifi_label = nullptr, *live_label = nullptr;
static lv_obj_t *weather_label = nullptr, *stocks_label = nullptr, *cal_label = nullptr;
static String weather_text = "Weather: waiting for Wi-Fi…";
static String stocks_text = "Stocks: waiting for Wi-Fi…";
static String cal_text = "Calendar: no ICS URL set.\nSet over Serial: ICS <url>";
static bool split_mode = false;
static bool timer_running = false;
static uint32_t timer_start_ms = 0, timer_accum_ms = 0;
// ntp_ok declared above (second-reel block).

static void style_card(lv_obj_t *o) {
  lv_obj_set_style_bg_color(o, lv_color_hex(C_CARD), 0);
  lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(o, 32, 0);
  lv_obj_set_style_border_width(o, 1, 0);
  lv_obj_set_style_border_color(o, lv_color_hex(0x3A4152), 0);
  lv_obj_set_style_shadow_width(o, 0, 0);
  lv_obj_set_style_pad_all(o, 20, 0);
}
static void style_screen(lv_obj_t *o) {
  lv_obj_set_style_bg_color(o, lv_color_hex(C_BG), 0);
  lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(o, 0, 0);
  lv_obj_set_style_pad_all(o, 0, 0);
}

// SQUARE PANEL: 600x600 frame centred on the 1024x600 board (x=212..812).
// Transparent centre so existing content stays visible; silver border
// reads as the frame, black screen behind reads as the bezel/side bars.
// Moved to background and made non-clickable so it never blocks touch.
static void sq_frame(lv_obj_t *scr) {
  if (!scr) return;
  lv_obj_t *f = lv_obj_create(scr);
  lv_obj_set_size(f, 600, 600); lv_obj_set_pos(f, 212, 0);
  lv_obj_set_style_bg_opa(f, LV_OPA_TRANSP, 0);
  lv_obj_set_style_radius(f, 50, 0);
  lv_obj_set_style_border_width(f, 6, 0);
  lv_obj_set_style_border_color(f, lv_color_hex(0xB4B4B9), 0);
  lv_obj_set_style_pad_all(f, 0, 0);
  lv_obj_clear_flag(f, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(f, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_move_background(f);
}
static lv_obj_t *label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color) {
  lv_obj_t *l = lv_label_create(parent);
  lv_label_set_text(l, text);
  lv_obj_set_style_text_font(l, font, 0);
  lv_obj_set_style_text_color(l, lv_color_hex(color), 0);
  return l;
}
static lv_obj_t *back_button(lv_obj_t *parent, lv_event_cb_t cb) {
  lv_obj_t *btn = lv_button_create(parent);
  lv_obj_set_size(btn, 120, 64); // v2: 64px+ touch targets
  lv_obj_set_style_bg_color(btn, lv_color_hex(C_CARD), 0);
  lv_obj_set_style_radius(btn, 24, 0);
  lv_obj_set_style_border_width(btn, 0, 0);
  lv_obj_t *l = lv_label_create(btn);
  lv_label_set_text(l, "<  Home");
  lv_obj_set_style_text_color(l, lv_color_hex(C_ACCENT), 0);
  lv_obj_center(l);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);
  return btn;
}
static void show_home_cb(lv_event_t *e) { (void)e; lv_screen_load(scr_home); }
static void refresh_segments() {
  // seg_single/seg_split no longer exist on the clock home (split deleted);
  // guard so a null segment can never crash the device.
  if (!seg_single || !seg_split) return;
  lv_obj_set_style_bg_color(seg_single, lv_color_hex(split_mode ? C_CARD : C_ACCENT), 0);
  lv_obj_set_style_bg_color(seg_split, lv_color_hex(split_mode ? C_ACCENT : C_CARD), 0);
  lv_obj_t *a = lv_obj_get_child(seg_single, 0);
  lv_obj_t *b = lv_obj_get_child(seg_split, 0);
  lv_obj_set_style_text_color(a, lv_color_hex(split_mode ? C_TEXT : 0xFFFFFF), 0);
  lv_obj_set_style_text_color(b, lv_color_hex(split_mode ? 0xFFFFFF : C_TEXT), 0);
}
static void set_mode(bool split) {
  (void)split; split_mode = false; // SPLIT DELETED: always single-app full screen
  prefs.putBool("split", false);
  refresh_segments();
}
static void mode_single_cb(lv_event_t *e) { (void)e; set_mode(false); }
static void mode_split_cb(lv_event_t *e) { (void)e; set_mode(true); }
static void open_screen_cb(lv_event_t *e) {
  lv_obj_t *target = (lv_obj_t *)lv_event_get_user_data(e);
  if (split_mode && target != scr_settings) { lv_screen_load(scr_split); return; }
  lv_screen_load(target);
}
static void timer_start_cb(lv_event_t *e) { (void)e; if (!timer_running) { timer_start_ms = millis(); timer_running = true; } }
static void timer_pause_cb(lv_event_t *e) { (void)e; if (timer_running) { timer_accum_ms += millis() - timer_start_ms; timer_running = false; } }
static void timer_reset_cb(lv_event_t *e) { (void)e; timer_running = false; timer_accum_ms = 0; }
static void brightness_cb(lv_event_t *e) {
  lv_obj_t *s = (lv_obj_t *)lv_event_get_target(e);
  tft.setBrightness((uint8_t)lv_slider_get_value(s));
}
static void fmt_time(char *buf, size_t n, uint32_t ms) {
  uint32_t s = ms / 1000;
  snprintf(buf, n, "%02lu:%02lu", (unsigned long)(s / 60), (unsigned long)(s % 60));
}
static void tick_cb(lv_timer_t *t) {
  (void)t;
  char buf[64];
  uint32_t elapsed = timer_accum_ms + (timer_running ? (millis() - timer_start_ms) : 0);
  fmt_time(buf, sizeof(buf), elapsed);
  if (timer_label) lv_label_set_text_fmt(timer_label, "%s", buf);
  if (split_timer_label) lv_label_set_text_fmt(split_timer_label, "%s", buf);
  if (ntp_ok) {
    time_t now = time(nullptr);
    struct tm *tmv = localtime(&now);
    snprintf(buf, sizeof(buf), "%02d:%02d", tmv->tm_hour, tmv->tm_min);
    if (clock_label) lv_label_set_text(clock_label, buf);
    if (clock_sub) lv_label_set_text(clock_sub, "Time, synced");
    if (split_clock_label) lv_label_set_text(split_clock_label, buf);
  } else {
    fmt_time(buf, sizeof(buf), millis());
    if (clock_label) lv_label_set_text_fmt(clock_label, "%s", buf);
    if (clock_sub) lv_label_set_text(clock_sub, "Uptime — Wi-Fi not set, no fake time");
    if (split_clock_label) lv_label_set_text_fmt(split_clock_label, "%s", buf);
  }
  if (wifi_label) {
    lv_label_set_text_fmt(wifi_label, "Wi-Fi: %s\nSet over Serial: WIFI <ssid> <password>",
      WiFi.status() == WL_CONNECTED ? "connected" : "not connected");
  }
  // Stopwatch (real, millis-based) + Alarm (visual only — no speaker on board)
  uint32_t sw_elapsed = sw_accum_ms + (sw_running ? (millis() - sw_start_ms) : 0);
  if (sw_label) {
    uint32_t cs = (sw_elapsed / 10) % 100;
    lv_label_set_text_fmt(sw_label, "%02lu:%02lu.%02lu", (unsigned long)(sw_elapsed / 60000), (unsigned long)((sw_elapsed / 1000) % 60), (unsigned long)cs);
  }
  if (ntp_ok && alarm_h >= 0 && alarm_label) {
    time_t now = time(nullptr); struct tm *tmv = localtime(&now);
    if (tmv->tm_hour == alarm_h && tmv->tm_min == alarm_m) lv_label_set_text(alarm_label, "ALARM!  —  tap Clock to dismiss");
    else lv_label_set_text_fmt(alarm_label, "Alarm set: %02d:%02d", alarm_h, alarm_m);
  }
  // HomeView surfaces: home clock/date, widget clock/date, photo clock,
  // calendar today, step-timer ring. All live, never invented.
  if (ntp_ok) {
    time_t now = time(nullptr); struct tm *tmv = localtime(&now);
    char tbuf[32], dbuf[32];
    snprintf(tbuf, sizeof(tbuf), "%02d:%02d", tmv->tm_hour, tmv->tm_min);
    snprintf(dbuf, sizeof(dbuf), "%s, %s %d", tmv->tm_wday == 0 ? "Sun" : tmv->tm_wday == 1 ? "Mon" : tmv->tm_wday == 2 ? "Tue" : tmv->tm_wday == 3 ? "Wed" : tmv->tm_wday == 4 ? "Thu" : tmv->tm_wday == 5 ? "Fri" : "Sat",
      tmv->tm_mon == 0 ? "Jan" : tmv->tm_mon == 1 ? "Feb" : tmv->tm_mon == 2 ? "Mar" : tmv->tm_mon == 3 ? "Apr" : tmv->tm_mon == 4 ? "May" : tmv->tm_mon == 5 ? "Jun" : tmv->tm_mon == 6 ? "Jul" : tmv->tm_mon == 7 ? "Aug" : tmv->tm_mon == 8 ? "Sep" : tmv->tm_mon == 9 ? "Oct" : tmv->tm_mon == 10 ? "Nov" : "Dec", tmv->tm_mday);
    if (home_clock_label) lv_label_set_text(home_clock_label, tbuf);
    if (home_date_label) lv_label_set_text(home_date_label, dbuf);
    if (tile_widget_clock_label) lv_label_set_text(tile_widget_clock_label, tbuf);
    if (tile_widget_weekday_label) lv_label_set_text(tile_widget_weekday_label, dbuf);
    if (widget_clock_label) lv_label_set_text(widget_clock_label, tbuf);
    if (photo_clock_label) lv_label_set_text(photo_clock_label, tbuf);
    if (photoclock_time_label) lv_label_set_text(photoclock_time_label, tbuf); greeting_update();
    if (widget_weekday_label) lv_label_set_text(widget_weekday_label, dbuf);
    if (widget_date_label) lv_label_set_text_fmt(widget_date_label, "%d", tmv->tm_mday);
    if (tile_widget_date_label) lv_label_set_text_fmt(tile_widget_date_label, "%d", tmv->tm_mday);
    if (tmv->tm_mon == 10 && tmv->tm_year == 126) {
      for (int i = 0; i < 31; i++) if (cal_day_labels[i]) lv_obj_set_style_text_color(cal_day_labels[i], lv_color_hex(i == tmv->tm_mday - 1 ? 0x0A84FF : C_TEXT), 0);
    }
  } else {
    fmt_time(buf, sizeof(buf), millis());
    if (home_clock_label) lv_label_set_text(home_clock_label, buf);
    if (home_date_label) lv_label_set_text(home_date_label, "Wi-Fi not set — tap for Clock app");
    if (tile_widget_clock_label) lv_label_set_text(tile_widget_clock_label, buf);
    if (tile_widget_date_label) lv_label_set_text(tile_widget_date_label, "--");
    if (widget_clock_label) lv_label_set_text(widget_clock_label, buf);
    if (photo_clock_label) lv_label_set_text(photo_clock_label, buf);
    if (photoclock_time_label) lv_label_set_text(photoclock_time_label, buf); greeting_update();
    if (widget_date_label) lv_label_set_text(widget_date_label, "--");
  }
  if (timer_arc) { int pct = elapsed >= 300000UL ? 100 : (int)(elapsed * 100 / 300000UL); lv_arc_set_value(timer_arc, pct); }
}
static lv_obj_t *tile(lv_obj_t *parent, int x, int y, const char *title, const char *sub, lv_obj_t *target) {
  lv_obj_t *card = lv_obj_create(parent);
  lv_obj_set_size(card, 310, 130);
  lv_obj_set_pos(card, x, y);
  style_card(card);
  lv_obj_t *t = label(card, title, &lv_font_montserrat_24, C_TEXT);
  lv_obj_align(t, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_t *s = label(card, sub, &lv_font_montserrat_14, C_SUB);
  lv_obj_align(s, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(card, open_screen_cb, LV_EVENT_CLICKED, target);
  return card;
}
static void icon_btn(lv_obj_t *parent, int x, int y, const char *glyph, const char *name, uint32_t color, lv_obj_t *target) {
  lv_obj_t *btn = lv_button_create(parent);
  lv_obj_set_size(btn, 90, 90); lv_obj_set_pos(btn, x, y);
  lv_obj_set_style_bg_color(btn, lv_color_hex(color), 0);
  lv_obj_set_style_radius(btn, 24, 0); lv_obj_set_style_border_width(btn, 0, 0);
  lv_obj_t *g = lv_label_create(btn); lv_label_set_text(g, glyph);
  lv_obj_set_style_text_font(g, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(g, lv_color_hex(0xFFFFFF), 0); lv_obj_center(g);
  lv_obj_add_event_cb(btn, open_screen_cb, LV_EVENT_CLICKED, target);
  lv_obj_t *n = label(parent, name, &lv_font_montserrat_14, C_TEXT);
  lv_obj_set_pos(n, x, y + 96);
}
static lv_obj_t *pill_button(lv_obj_t *parent, const char *text, lv_event_cb_t cb) {
  lv_obj_t *btn = lv_button_create(parent);
  lv_obj_set_size(btn, 150, 48);
  lv_obj_set_style_radius(btn, 24, 0);
  lv_obj_set_style_border_width(btn, 0, 0);
  lv_obj_t *l = lv_label_create(btn);
  lv_label_set_text(l, text);
  lv_obj_center(l);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);
  return btn;
}
static String http_get(const String &url) {
  WiFiClientSecure client; client.setInsecure();
  HTTPClient http; http.begin(client, url); http.setTimeout(10000);
  int code = http.GET();
  String body = (code == 200) ? http.getString() : String("");
  http.end();
  return body;
}
static String json_num(const String &body, const String &key) {
  int i = body.indexOf("\"" + key + "\":");
  if (i < 0) return "";
  int s = i + key.length() + 3;
  int e = body.indexOf(',', s); if (e < 0) e = body.indexOf('}', s);
  return body.substring(s, e);
}
static void fetch_real_data() {
  if (WiFi.status() != WL_CONNECTED) return;
  String w = http_get(String("https://api.open-meteo.com/v1/forecast?latitude=") + String(WEATHER_LAT, 2) +
    "&longitude=" + String(WEATHER_LON, 2) +
    "&current=temperature_2m,relative_humidity_2m,weather_code,wind_speed_10m&temperature_unit=fahrenheit&timezone=America%2FNew_York");
  if (w.length() > 0) {
    weather_text = "Weather (Alpharetta): " + json_num(w, "temperature_2m") + "°F, " +
      json_num(w, "relative_humidity_2m") + "% humidity, wind " + json_num(w, "wind_speed_10m") + " km/h";
    if (weather_label) lv_label_set_text(weather_label, weather_text.c_str());
  if (grid_weather_label) lv_label_set_text(grid_weather_label, weather_text.c_str());
  if (widget_temp_label) lv_label_set_text(widget_temp_label, weather_text.c_str());
  if (tile_widget_temp_label) lv_label_set_text(tile_widget_temp_label, weather_text.c_str());
  if (tile_grid_weather_label) lv_label_set_text(tile_grid_weather_label, weather_text.c_str());
  }
  // 10-day forecast (Open-Meteo daily, no key): first day's max/min only,
  // full 10-day list is a follow-up once the Weather screen has room.
  {
    String f = http_get("https://api.open-meteo.com/v1/forecast?latitude=34.08&longitude=-84.29&daily=temperature_2m_max,temperature_2m_min&temperature_unit=fahrenheit&timezone=America%2FNew_York&forecast_days=10");
    int mi = f.indexOf("\"temperature_2m_max\":[");
    if (mi >= 0) {
      int b = f.indexOf('[', mi), c = f.indexOf(',', b);
      String mx = f.substring(b + 1, c);
      int mi2 = f.indexOf("\"temperature_2m_min\":[");
      int b2 = f.indexOf('[', mi2), c2 = f.indexOf(',', b2);
      String mn = f.substring(b2 + 1, c2);
      weather_text += "  •  Today H:" + mx + "°F L:" + mn + "°F (forecast)";
      hl_text = "Today H:" + mx + "°F L:" + mn + "°F (forecast)";
      if (home_hl_label) lv_label_set_text(home_hl_label, hl_text.c_str());
      if (weather_label) lv_label_set_text(weather_label, weather_text.c_str());
    }
  }
  // Stocks history chart: last 10 NU closes from the Stooq daily CSV,
  // plotted as close*100 (e.g. 1163 for $11.63). Only if the CSV is real.
  if (stocks_chart && stocks_series) {
    String csv = http_get("https://stooq.com/q/d/l/?s=nu.us&i=d");
    if (csv.startsWith("Date")) {
      int lines[10]; int n = 0; int pos = 0;
      // Collect last 10 data-row close values by scanning lines.
      String rows[64]; int rc = 0; int p = csv.indexOf('\n');
      while (p >= 0 && rc < 64) { int q = csv.indexOf('\n', p + 1); if (q < 0) break; rows[rc++] = csv.substring(p + 1, q); p = q; }
      int start = rc > 10 ? rc - 10 : 0;
      for (int i = start; i < rc && n < 10; i++) {
        String row = rows[i]; int f = 0, pp = 0; String cv;
        while (f <= 4) { int comma = row.indexOf(',', pp); String field = (comma < 0) ? row.substring(pp) : row.substring(pp, comma); if (f == 4) cv = field; pp = comma + 1; f++; }
        lines[n++] = (int)(cv.toFloat() * 100);
      }
      for (int i = 0; i < n; i++) lv_chart_set_value_by_id(stocks_chart, stocks_series, i, lines[i]);
      lv_chart_refresh(stocks_chart);
    }
  }
  String s = "Stocks — last close:\n";
  for (int i = 0; i < 3; i++) {
    String csv = http_get(String("https://stooq.com/q/d/l/?s=") + STOCK_TICKERS[i] + "&i=d");
    if (!csv.startsWith("Date")) { s += String(STOCK_NAMES[i]) + ": unavailable\n"; continue; }
    int nl = csv.indexOf('\n');
    String row = csv.substring(nl + 1, csv.indexOf('\n', nl + 1));
    // Row: Date,Open,High,Low,Close,Volume — Close is field 5
    int f = 0, pos = 0; String date, closev;
    while (f <= 4) {
      int comma = row.indexOf(',', pos);
      String field = (comma < 0) ? row.substring(pos) : row.substring(pos, comma);
      if (f == 0) date = field; if (f == 4) closev = field;
      pos = comma + 1; f++;
    }
    s += String(STOCK_NAMES[i]) + ": $" + closev + " (" + date + ")\n";
  }
  stocks_text = s;
  if (stocks_label) lv_label_set_text(stocks_label, stocks_text.c_str());
  String ics_url = prefs.getString("ics", "");
  if (ics_url.length() > 0) {
    String ics = http_get(ics_url);
    int si = ics.indexOf("SUMMARY:");
    if (si >= 0) {
      int e = ics.indexOf('\n', si);
      cal_text = "Calendar: " + ics.substring(si + 8, e);
    } else cal_text = "Calendar: no events found in ICS feed.";
  } else cal_text = "Calendar: no ICS URL set.\nSet over Serial: ICS <url>";
  if (cal_label) lv_label_set_text(cal_label, cal_text.c_str());
  if (home_cal_label) lv_label_set_text(home_cal_label, cal_text.c_str());
}

// MAPS — OpenStreetMap tiles, no key. Tile x/y from lat/lon at map_zoom.
// PNG rendering in LVGL needs lv_png enabled, so this build fetches the
// tile and verifies HTTP 200, shows centre/zoom/tile, and draws a schematic
// pin map — it does NOT claim to render the photo tile yet.
static void fetch_maps() {
  if (WiFi.status() != WL_CONNECTED) { if (maps_label) lv_label_set_text(maps_label, "Maps: Wi-Fi not connected."); return; }
  double latr = map_lat * 3.14159265 / 180.0;
  int n = 1 << map_zoom;
  int tx = (int)((map_lon + 180.0) / 360.0 * n);
  int ty = (int)((1.0 - log(tan(latr) + 1.0 / cos(latr)) / 3.14159265) / 2.0 * n);
  String url = "https://tile.openstreetmap.org/" + String(map_zoom) + "/" + String(tx) + "/" + String(ty) + ".png";
  WiFiClientSecure client; client.setInsecure();
  HTTPClient http; http.begin(client, url); http.addHeader("User-Agent", "wall-display/1.0");
  int code = http.GET(); http.end();
  String msg = "Maps — Alpharetta, GA\nLat " + String(map_lat, 4) + "  Lon " + String(map_lon, 4) +
    "\nZoom " + String(map_zoom) + "  Tile " + String(tx) + "/" + String(ty) +
    "\nTile fetch: HTTP " + String(code) + (code == 200 ? " (tile exists)" : " (tile not fetched)") +
    "\nTile URL:\n" + url +
    "\nSet centre over Serial: MAPS <lat> <lon>";
  if (maps_label) lv_label_set_text(maps_label, msg.c_str());
}
// MAIL — Gmail IMAP, user connects himself. Credentials over Serial only:
//   MAIL <gmail-address> <app-password>
// Never hardcoded, never in the repo. App passwords are for accounts with
// 2-step verification; the regular password will not work for IMAP.
static void fetch_mail() {
  if (WiFi.status() != WL_CONNECTED) { if (mail_label) lv_label_set_text(mail_label, "Mail: Wi-Fi not connected."); return; }
  String user = prefs.getString("mail_user", "");
  String pass = prefs.getString("mail_pass", "");
  if (user.length() == 0) { if (mail_label) lv_label_set_text(mail_label, "Mail: not connected.\nSet over Serial: MAIL <address> <app-password>"); return; }
  WiFiClientSecure client; client.setInsecure();
  if (!client.connect("imap.gmail.com", 993)) { if (mail_label) lv_label_set_text(mail_label, "Mail: could not reach imap.gmail.com:993."); return; }
  String resp;
  auto read_resp = [&]() { resp = ""; unsigned long t0 = millis(); while (millis() - t0 < 5000 && client.available() == 0) delay(50); while (client.available()) { char c = client.read(); resp += c; if (resp.length() > 4000) break; } };
  read_resp();
  client.println("a001 LOGIN " + user + " " + pass); read_resp();
  if (resp.indexOf("OK") < 0) { if (mail_label) lv_label_set_text(mail_label, "Mail: login failed — check address / app password."); client.stop(); return; }
  client.println("a002 SELECT INBOX"); read_resp();
  client.println("a003 FETCH 1:5 (BODY[HEADER.FIELDS (SUBJECT FROM DATE)])"); read_resp();
  client.println("a004 LOGOUT"); client.stop();
  String out = "Mail — " + user + "\nRecent headers (raw, truncated):\n" + resp.substring(0, 900);
  if (mail_label) lv_label_set_text(mail_label, out.c_str());
}

// Small square-grid button for scr_grid: 64px squircle + label below.
static void grid_btn(lv_obj_t *parent, int x, int y, const char *glyph, const char *name, uint32_t color, lv_obj_t *target) {
  lv_obj_t *btn = lv_button_create(parent);
  lv_obj_set_size(btn, 64, 64); lv_obj_set_pos(btn, x, y);
  lv_obj_set_style_bg_color(btn, lv_color_hex(color), 0);
  lv_obj_set_style_radius(btn, 18, 0); lv_obj_set_style_border_width(btn, 0, 0);
  lv_obj_t *g = lv_label_create(btn); lv_label_set_text(g, glyph);
  lv_obj_set_style_text_font(g, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(g, lv_color_hex(0xFFFFFF), 0); lv_obj_center(g);
  lv_obj_add_event_cb(btn, open_screen_cb, LV_EVENT_CLICKED, target);
  lv_obj_t *n = label(parent, name, &lv_font_montserrat_14, C_TEXT);
  lv_obj_set_pos(n, x, y + 68);
}
static void build_ui() {
  // HOME — swipeable home feed (tileview, GT911 touch). The tileview
  // itself is created here; its 4 tiles are built at the end of
  // build_ui(), after every target screen exists. Page order: 0 Clock
  // (exact blue clock face, face-colour preference), 1 Widgets,
  // 2 Apps grid, 3 Home controls. A short tap still fires CLICKED on
  // children; only a drag scrolls to the next page. Single-app full
  // screen only (split deleted).
  scr_home = lv_obj_create(NULL); style_screen(scr_home);
  face_idx = prefs.getInt("face", 0);
  profile_idx = prefs.getInt("profile", 0); if (profile_idx < 0 || profile_idx > 5) profile_idx = 0;
  home_tv = lv_tileview_create(scr_home);
  lv_obj_set_size(home_tv, 600, 600); lv_obj_set_pos(home_tv, 212, 0);
  lv_obj_set_style_bg_opa(home_tv, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(home_tv, 0, 0);
  lv_obj_set_scrollbar_mode(home_tv, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_event_cb(home_tv, home_tv_scroll_cb, LV_EVENT_SCROLL_END, NULL);
  // TIMER
  scr_timer = lv_obj_create(NULL); style_screen(scr_timer);
  back_button(scr_timer, show_home_cb); lv_obj_align(lv_obj_get_child(scr_timer, 0), LV_ALIGN_TOP_LEFT, 32, 24);
  label(scr_timer, "Timer", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_timer, 1), LV_ALIGN_TOP_LEFT, 32, 80);
  lv_obj_t *tcard = lv_obj_create(scr_timer); lv_obj_set_size(tcard, 540, 340); lv_obj_set_pos(tcard, 242, 110);
  lv_obj_set_style_bg_color(tcard, lv_color_hex(0x000000), 0); lv_obj_set_style_bg_opa(tcard, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(tcard, 40, 0); lv_obj_set_style_border_width(tcard, 0, 0); lv_obj_set_style_pad_all(tcard, 20, 0);
  label(tcard, "Step timer", &lv_font_montserrat_24, 0xFF9500); lv_obj_align(lv_obj_get_child(tcard, 0), LV_ALIGN_TOP_MID, 0, 0);
  timer_arc = lv_arc_create(tcard); lv_obj_set_size(timer_arc, 200, 200); lv_obj_center(timer_arc);
  lv_arc_set_range(timer_arc, 0, 100); lv_arc_set_value(timer_arc, 0);
  lv_obj_set_style_arc_color(timer_arc, lv_color_hex(0xFF9500), LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(timer_arc, lv_color_hex(0x38383A), LV_PART_MAIN);
  timer_label = label(tcard, "00:00", &lv_font_montserrat_48, 0xFFFFFF); lv_obj_center(timer_label);
  lv_obj_t *st = pill_button(scr_timer, "Start", timer_start_cb); lv_obj_set_size(st, 160, 56); lv_obj_align(st, LV_ALIGN_BOTTOM_MID, -180, -40);
  lv_obj_t *pa = pill_button(scr_timer, "Pause", timer_pause_cb); lv_obj_set_size(pa, 160, 56); lv_obj_align(pa, LV_ALIGN_BOTTOM_MID, 0, -40);
  lv_obj_t *re = pill_button(scr_timer, "Reset", timer_reset_cb); lv_obj_set_size(re, 160, 56); lv_obj_align(re, LV_ALIGN_BOTTOM_MID, 180, -40);
  // CLOCK
  scr_clock = lv_obj_create(NULL); style_screen(scr_clock);
  back_button(scr_clock, show_home_cb); lv_obj_align(lv_obj_get_child(scr_clock, 0), LV_ALIGN_TOP_LEFT, 32, 24);
  label(scr_clock, "Clock", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_clock, 1), LV_ALIGN_TOP_LEFT, 32, 80);
  lv_obj_t *ccard = lv_obj_create(scr_clock); lv_obj_set_size(ccard, 960, 260); lv_obj_align(ccard, LV_ALIGN_TOP_MID, 0, 160); style_card(ccard);
  clock_label = label(ccard, "00:00", &lv_font_montserrat_48, C_TEXT); lv_obj_align(clock_label, LV_ALIGN_TOP_MID, 0, 40);
  clock_sub = label(ccard, "", &lv_font_montserrat_14, C_SUB); lv_obj_align(clock_sub, LV_ALIGN_BOTTOM_MID, 0, -40);
  // Stopwatch + Alarm (real Clock-app features, not just a time label)
  sw_label = label(scr_clock, "Stopwatch: 00:00.00", &lv_font_montserrat_24, C_TEXT); lv_obj_align(sw_label, LV_ALIGN_TOP_MID, 0, 440);
  lv_obj_t *sws = pill_button(scr_clock, "Start", sw_start_cb); lv_obj_set_size(sws, 160, 56); lv_obj_align(sws, LV_ALIGN_BOTTOM_MID, -180, -40);
  lv_obj_t *swl = pill_button(scr_clock, "Lap", sw_lap_cb); lv_obj_set_size(swl, 160, 56); lv_obj_align(swl, LV_ALIGN_BOTTOM_MID, 0, -40);
  lv_obj_t *swr = pill_button(scr_clock, "Reset", sw_reset_cb); lv_obj_set_size(swr, 160, 56); lv_obj_align(swr, LV_ALIGN_BOTTOM_MID, 180, -40);
  alarm_label = label(scr_clock, "Alarm: none — set over Serial: ALARM HH:MM", &lv_font_montserrat_14, C_SUB); lv_obj_align(alarm_label, LV_ALIGN_BOTTOM_MID, 0, -110);
  { lv_obj_t *cb = pill_button(scr_clock, "Customize", open_picker_cb); lv_obj_set_pos(cb, 630, 28); }
  // SETTINGS
  scr_settings = lv_obj_create(NULL); style_screen(scr_settings);
  back_button(scr_settings, show_home_cb); lv_obj_align(lv_obj_get_child(scr_settings, 0), LV_ALIGN_TOP_LEFT, 32, 24);
  label(scr_settings, "Settings", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_settings, 1), LV_ALIGN_TOP_LEFT, 32, 80);
  lv_obj_t *scard = lv_obj_create(scr_settings); lv_obj_set_size(scard, 960, 260); lv_obj_align(scard, LV_ALIGN_TOP_MID, 0, 160); style_card(scard);
  label(scard, "Brightness", &lv_font_montserrat_24, C_TEXT); lv_obj_align(lv_obj_get_child(scard, 0), LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_t *slider = lv_slider_create(scard);
  lv_slider_set_range(slider, 0, 255); lv_slider_set_value(slider, 200, LV_ANIM_OFF);
  lv_obj_set_size(slider, 600, 20); lv_obj_align(slider, LV_ALIGN_CENTER, 0, 20);
  lv_obj_add_event_cb(slider, brightness_cb, LV_EVENT_VALUE_CHANGED, NULL);
  wifi_label = label(scard, "Wi-Fi: not connected", &lv_font_montserrat_14, C_SUB); lv_obj_align(wifi_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  { lv_obj_t *tb = pill_button(scr_settings, "Theme", customize_cb); lv_obj_set_pos(tb, 242, 450); }
  // WEATHER (real data via Open-Meteo, no key)
  scr_live = lv_obj_create(NULL); style_screen(scr_live);
  back_button(scr_live, show_home_cb); lv_obj_align(lv_obj_get_child(scr_live, 0), LV_ALIGN_TOP_LEFT, 32, 24);
  label(scr_live, "Weather", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_live, 1), LV_ALIGN_TOP_LEFT, 32, 80);
  lv_obj_t *wcard = lv_obj_create(scr_live); lv_obj_set_size(wcard, 960, 260); lv_obj_align(wcard, LV_ALIGN_TOP_MID, 0, 160); style_card(wcard);
  weather_label = label(wcard, "Weather: waiting for Wi-Fi…", &lv_font_montserrat_24, C_TEXT); lv_obj_align(weather_label, LV_ALIGN_CENTER, 0, 0);
  // STOCKS (brokerage-style: big price, chart, ticker list — chart is
  // illustrative until Stooq daily CSV is verified live on the board;
  // ticker text is only set from a verified fetch, never invented)
  {
    lv_obj_t *st = lv_obj_create(NULL); style_screen(st); scr_stocks = st;
    back_button(st, show_home_cb); lv_obj_align(lv_obj_get_child(st, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(st, "Stocks", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(st, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    lv_obj_t *chart = lv_chart_create(st);
    lv_obj_set_size(chart, 960, 240); lv_obj_align(chart, LV_ALIGN_TOP_MID, 0, 160);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 10);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 1000, 1300);
    stocks_chart = chart;
    stocks_series = lv_chart_add_series(chart, lv_color_hex(0xFF453A), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_next_value(chart, stocks_series, 1185); lv_chart_set_next_value(chart, stocks_series, 1180);
    lv_chart_set_next_value(chart, stocks_series, 1178); lv_chart_set_next_value(chart, stocks_series, 1175);
    lv_chart_set_next_value(chart, stocks_series, 1171); lv_chart_set_next_value(chart, stocks_series, 1168);
    lv_chart_set_next_value(chart, stocks_series, 1165); lv_chart_set_next_value(chart, stocks_series, 1163);
    lv_chart_set_next_value(chart, stocks_series, 1160); lv_chart_set_next_value(chart, stocks_series, 1163);
    stocks_label = label(st, "NU: waiting for Wi-Fi…  •  SOFI / VTI: last close, unverified", &lv_font_montserrat_14, C_SUB);
    lv_obj_align(stocks_label, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_obj_t *chart_note = label(st, "Chart illustrative, not live data.", &lv_font_montserrat_14, C_SUB);
    lv_obj_align(chart_note, LV_ALIGN_BOTTOM_MID, 0, -20);
  }
  // CALENDAR — November 2026 grid (Nov 1, 2026 = Sunday, verified).
  // Today is recoloured by tick_cb when NTP confirms Nov 2026;
  // otherwise day 1 stays highlighted as the build date. ICS events
  // below come only from the user's ICS feed — never invented.
  {
    lv_obj_t *cal = lv_obj_create(NULL); style_screen(cal); scr_calendar = cal;
    back_button(cal, show_home_cb); lv_obj_align(lv_obj_get_child(cal, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(cal, "November 2026", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(cal, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    const char *dow[7] = {"S","M","T","W","T","F","S"};
    for (int i = 0; i < 7; i++) {
      lv_obj_t *h = label(cal, dow[i], &lv_font_montserrat_14, C_SUB);
      lv_obj_align(h, LV_ALIGN_TOP_LEFT, 32 + i * 130, 150);
    }
    for (int day = 1; day <= 30; day++) {
      int col = (day - 1) % 7, row = (day - 1) / 7;
      char buf[4]; snprintf(buf, sizeof(buf), "%d", day);
      lv_obj_t *d = label(cal, buf, &lv_font_montserrat_24, day == 1 ? 0x0A84FF : C_TEXT);
      lv_obj_align(d, LV_ALIGN_TOP_LEFT, 32 + col * 130, 180 + row * 60);
      if (day <= 31) cal_day_labels[day - 1] = d;
    }
    { lv_obj_t *nb = lv_button_create(cal); lv_obj_set_size(nb, 150, 48); lv_obj_set_pos(nb, 842, 90);
      lv_obj_set_style_radius(nb, 24, 0); lv_obj_set_style_border_width(nb, 0, 0);
      lv_obj_t *nl = lv_label_create(nb); lv_label_set_text(nl, "+ Notes"); lv_obj_center(nl);
      lv_obj_add_event_cb(nb, open_notes_cb, LV_EVENT_CLICKED, NULL); }
    cal_label = label(cal, "Calendar: no ICS URL set.", &lv_font_montserrat_14, C_SUB);
    lv_obj_align(cal_label, LV_ALIGN_BOTTOM_MID, 0, -20);
  }
  // CAMERA / SAFARI / GOOGLE / NOTES / MUSIC / PHOTOS — placeholder
  // screens with honest limits: this board has no camera module and
  // cannot run a full browser, so these do NOT fake photos or web pages.
  {
    lv_obj_t *cam = lv_obj_create(NULL); style_screen(cam);
    back_button(cam, show_home_cb); lv_obj_align(lv_obj_get_child(cam, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(cam, "Camera", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(cam, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    label(cam, "Camera — dropped. No camera, no extra boards.", &lv_font_montserrat_24, C_SUB); lv_obj_align(lv_obj_get_child(cam, 2), LV_ALIGN_TOP_LEFT, 32, 180);
    scr_camera = cam;
    lv_obj_t *web = lv_obj_create(NULL); style_screen(web);
    back_button(web, show_home_cb); lv_obj_align(lv_obj_get_child(web, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(web, "Safari / Google", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(web, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    label(web, "Full web browsing is not available on this board.", &lv_font_montserrat_24, C_SUB); lv_obj_align(lv_obj_get_child(web, 2), LV_ALIGN_TOP_LEFT, 32, 180);
    scr_web = web;
    // SEARCH — works now, no key: Wikipedia summary API, term via Serial.
    scr_search = lv_obj_create(NULL); style_screen(scr_search);
    back_button(scr_search, show_home_cb); lv_obj_align(lv_obj_get_child(scr_search, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_search, "Search", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_search, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    search_label = label(scr_search, "Set a term over Serial: SEARCH <term>", &lv_font_montserrat_24, C_SUB); lv_obj_align(search_label, LV_ALIGN_TOP_LEFT, 32, 180);
    lv_label_set_long_mode(search_label, LV_LABEL_LONG_WRAP); lv_obj_set_width(search_label, 960);
    lv_obj_t *ph = lv_obj_create(NULL); style_screen(ph);
    back_button(ph, show_home_cb); lv_obj_align(lv_obj_get_child(ph, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(ph, "Photos", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(ph, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    label(ph, "Placeholder images only — TF photos not loaded yet. Not a camera feed.", &lv_font_montserrat_14, C_SUB); lv_obj_align(lv_obj_get_child(ph, 2), LV_ALIGN_TOP_LEFT, 32, 180);
    for (int i = 0; i < 4; i++) {
      lv_obj_t *b = lv_button_create(ph); lv_obj_set_size(b, 250, 180);
      lv_obj_set_pos(b, 250 + (i % 2) * 280, 220 + (i / 2) * 180);
      lv_obj_set_style_bg_color(b, lv_color_hex(PHOTO_COLORS[i]), 0); lv_obj_set_style_radius(b, 32, 0); lv_obj_set_style_border_width(b, 0, 0);
      lv_obj_t *l = lv_label_create(b); char pb[16]; snprintf(pb, sizeof(pb), "Photo %d", i + 1); lv_label_set_text(l, pb); lv_obj_center(l);
      lv_obj_add_event_cb(b, photo_open_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
    }
    scr_photos = ph;
    scr_photo_view = lv_obj_create(NULL); style_screen(scr_photo_view);
    { lv_obj_t *bb = lv_button_create(scr_photo_view); lv_obj_set_size(bb, 150, 48); lv_obj_set_pos(bb, 242, 30);
      lv_obj_set_style_radius(bb, 24, 0); lv_obj_set_style_border_width(bb, 0, 0);
      lv_obj_t *bl = lv_label_create(bb); lv_label_set_text(bl, "<  Photos"); lv_obj_center(bl);
      lv_obj_add_event_cb(bb, photo_back_cb, LV_EVENT_CLICKED, NULL); }
    photo_panel = lv_obj_create(scr_photo_view); lv_obj_set_size(photo_panel, 500, 400); lv_obj_set_pos(photo_panel, 262, 100);
    lv_obj_set_style_bg_color(photo_panel, lv_color_hex(PHOTO_COLORS[0]), 0); lv_obj_set_style_bg_opa(photo_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(photo_panel, 32, 0); lv_obj_set_style_border_width(photo_panel, 0, 0);
    { lv_obj_t *pc = label(scr_photo_view, "Photo — placeholder", &lv_font_montserrat_24, C_TEXT); lv_obj_set_pos(pc, 262, 520); }
    photo_clock_label = label(scr_photo_view, "--:--", &lv_font_montserrat_24, C_TEXT); lv_obj_set_pos(photo_clock_label, 640, 520);
    { lv_obj_t *zb = pill_button(scr_photo_view, "Zoom", photo_zoom_cb); lv_obj_set_pos(zb, 630, 520); }
    photo_zoom_label = label(scr_photo_view, "Zoom: 1x", &lv_font_montserrat_24, C_TEXT); lv_obj_set_pos(photo_zoom_label, 262, 555);
  }
  // SPLIT VIEW DELETED 2026-11-01 per Sripad: no split screen, single-app full screen only. scr_split stays null; split_mode is forced false below.
  // TASKS — real open items from Sripad's Week 4 / desk list. Tap to mark done.
  {
    scr_tasks = lv_obj_create(NULL); style_screen(scr_tasks);
    back_button(scr_tasks, show_home_cb); lv_obj_align(lv_obj_get_child(scr_tasks, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_tasks, "Tasks", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_tasks, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    const char *tasks[5] = {"Week 4: Journal Entry 2 hours + approval", "Week 4: README in your own words", "Week 4: holder choice + checkout totals", "HubSpot sign-in by Nov 4", "NU earnings check around Nov 12"};
    for (int i = 0; i < 5; i++) {
      lv_obj_t *c = lv_obj_create(scr_tasks); lv_obj_set_size(c, 960, 60); lv_obj_align(c, LV_ALIGN_TOP_MID, 0, 160 + i * 80); style_card(c);
      lv_obj_t *t = label(c, tasks[i], &lv_font_montserrat_24, C_TEXT); lv_obj_align(t, LV_ALIGN_LEFT_MID, 0, 0);
      lv_obj_add_flag(c, LV_OBJ_FLAG_CLICKABLE);
      lv_obj_add_event_cb(c, task_toggle_cb, LV_EVENT_CLICKED, t);
    }
  }
  // CALCULATOR — working: digits, + - * /, =, C. Left-to-right, no precedence.
  {
    scr_calc = lv_obj_create(NULL); style_screen(scr_calc);
    back_button(scr_calc, show_home_cb); lv_obj_align(lv_obj_get_child(scr_calc, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_calc, "Calculator", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_calc, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    lv_obj_t *dc = lv_obj_create(scr_calc); lv_obj_set_size(dc, 960, 80); lv_obj_align(dc, LV_ALIGN_TOP_MID, 0, 160); style_card(dc);
    calc_display = label(dc, "0", &lv_font_montserrat_48, C_TEXT); lv_obj_align(calc_display, LV_ALIGN_RIGHT_MID, 0, 0);
    const char *keys[16] = {"7","8","9","/","4","5","6","*","1","2","3","-","C","0","=","+"};
    for (int i = 0; i < 16; i++) {
      lv_obj_t *b = lv_button_create(scr_calc);
      lv_obj_set_size(b, 220, 60); lv_obj_set_pos(b, 32 + (i % 4) * 240, 260 + (i / 4) * 80);
      lv_obj_set_style_bg_color(b, lv_color_hex(keys[i][0] == '=' ? 0xFF9500 : C_CARD), 0);
      lv_obj_set_style_radius(b, 16, 0); lv_obj_set_style_border_width(b, 0, 0);
      lv_obj_t *l = lv_label_create(b); lv_label_set_text(l, keys[i]);
      lv_obj_set_style_text_font(l, &lv_font_montserrat_24, 0); lv_obj_center(l);
      lv_obj_add_event_cb(b, calc_press_cb, LV_EVENT_CLICKED, (void *)keys[i]);
    }
  }
  // MUSIC / NOTES — placeholders with honest limits, not fake playback.
  {
    scr_music = lv_obj_create(NULL); style_screen(scr_music);
    back_button(scr_music, show_home_cb); lv_obj_align(lv_obj_get_child(scr_music, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_music, "Music", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_music, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    { lv_obj_t *art = lv_obj_create(scr_music); lv_obj_set_size(art, 300, 260); lv_obj_set_pos(art, 362, 150);
      lv_obj_set_style_bg_color(art, lv_color_hex(0xFF6B6B), 0); lv_obj_set_style_bg_opa(art, LV_OPA_COVER, 0);
      lv_obj_set_style_radius(art, 32, 0); lv_obj_set_style_border_width(art, 0, 0);
      lv_obj_t *g = label(art, "♪", &lv_font_montserrat_48, 0xFFFFFF); lv_obj_center(g); }
    music_track_label = label(scr_music, MUSIC_TITLES[0], &lv_font_montserrat_36, C_TEXT); lv_obj_align(music_track_label, LV_ALIGN_TOP_MID, 0, 430);
    music_artist_label = label(scr_music, MUSIC_ARTISTS[0], &lv_font_montserrat_24, C_SUB); lv_obj_align(music_artist_label, LV_ALIGN_TOP_MID, 0, 480);
    music_state_label = label(scr_music, "Paused (display only)", &lv_font_montserrat_14, C_SUB); lv_obj_align(music_state_label, LV_ALIGN_TOP_MID, 0, 520);
    { lv_obj_t *pr = pill_button(scr_music, "<", music_prev_cb); lv_obj_set_size(pr, 100, 56); lv_obj_align(pr, LV_ALIGN_BOTTOM_MID, -130, -40);
      lv_obj_t *tg = pill_button(scr_music, "Play", music_toggle_cb); lv_obj_set_size(tg, 100, 56); lv_obj_align(tg, LV_ALIGN_BOTTOM_MID, 0, -40);
      lv_obj_t *nx = pill_button(scr_music, ">", music_next_cb); lv_obj_set_size(nx, 100, 56); lv_obj_align(nx, LV_ALIGN_BOTTOM_MID, 130, -40); }
    { lv_obj_t *mn = label(scr_music, "Music — display only. No playback, no speaker on this board.", &lv_font_montserrat_14, C_SUB); lv_obj_align(mn, LV_ALIGN_BOTTOM_MID, 0, -8); }
    scr_notes = lv_obj_create(NULL); style_screen(scr_notes);
    back_button(scr_notes, show_home_cb); lv_obj_align(lv_obj_get_child(scr_notes, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_notes, "Notes", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_notes, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    note_label = label(scr_notes, "No notes yet — set one over Serial: NOTE <text>", &lv_font_montserrat_24, C_SUB); lv_obj_align(note_label, LV_ALIGN_TOP_LEFT, 32, 180);
    { lv_obj_t *rb = pill_button(scr_notes, "Recipe", open_recipe_cb); lv_obj_set_pos(rb, 32, 260); }
    lv_label_set_long_mode(note_label, LV_LABEL_LONG_WRAP); lv_obj_set_width(note_label, 960);
  }
  // MAPS / MAIL — built for Sripad to connect himself (see Serial commands).
  {
    scr_maps = lv_obj_create(NULL); style_screen(scr_maps);
    back_button(scr_maps, show_home_cb); lv_obj_align(lv_obj_get_child(scr_maps, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_maps, "Maps", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_maps, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    maps_label = label(scr_maps, "Maps: set Wi-Fi, then this screen fetches an OpenStreetMap tile.", &lv_font_montserrat_24, C_SUB); lv_obj_align(maps_label, LV_ALIGN_TOP_LEFT, 32, 180);
    lv_label_set_long_mode(maps_label, LV_LABEL_LONG_WRAP); lv_obj_set_width(maps_label, 960);
    scr_mail = lv_obj_create(NULL); style_screen(scr_mail);
    back_button(scr_mail, show_home_cb); lv_obj_align(lv_obj_get_child(scr_mail, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_mail, "Mail", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_mail, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    mail_label = label(scr_mail, "Mail: not connected.\nSet over Serial: MAIL <address> <app-password>", &lv_font_montserrat_24, C_SUB); lv_obj_align(mail_label, LV_ALIGN_TOP_LEFT, 32, 180);
    lv_label_set_long_mode(mail_label, LV_LABEL_LONG_WRAP); lv_obj_set_width(mail_label, 960);
  }
  // CUSTOMIZE — Apple-style edit button: Dark / Light / Tinted, saved in prefs.
  {
    scr_customize = lv_obj_create(NULL); style_screen(scr_customize);
    back_button(scr_customize, show_home_cb); lv_obj_align(lv_obj_get_child(scr_customize, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_customize, "Customize", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_customize, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    lv_obj_t *bd = pill_button(scr_customize, "Dark", theme_dark_cb); lv_obj_set_size(bd, 200, 64); lv_obj_align(bd, LV_ALIGN_TOP_MID, -220, 200);
    lv_obj_t *bl = pill_button(scr_customize, "Light", theme_light_cb); lv_obj_set_size(bl, 200, 64); lv_obj_align(bl, LV_ALIGN_TOP_MID, 0, 200);
    lv_obj_t *bt = pill_button(scr_customize, "Tinted", theme_tint_cb); lv_obj_set_size(bt, 200, 64); lv_obj_align(bt, LV_ALIGN_TOP_MID, 220, 200);
    label(scr_customize, "Dark is the default. Theme is saved and restored on reboot.", &lv_font_montserrat_14, C_SUB); lv_obj_align(lv_obj_get_child(scr_customize, 2), LV_ALIGN_TOP_LEFT, 32, 300);
  }
  // CLOCK PICKER — "Dial Style Colour": lavender screen, large preview
  // face, colour tiles below, check mark + selected name at bottom.
  // Selection is saved in Preferences and recolours lock/home/clock.
  {
    scr_picker = lv_obj_create(NULL); style_screen(scr_picker);
    back_button(scr_picker, show_home_cb); lv_obj_align(lv_obj_get_child(scr_picker, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_picker, "Dial Style Colour", &lv_font_montserrat_36, C_TEXT); lv_obj_align(lv_obj_get_child(scr_picker, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    picker_preview = lv_obj_create(scr_picker); lv_obj_set_size(picker_preview, 300, 300); lv_obj_set_pos(picker_preview, 362, 130);
    lv_obj_set_style_bg_color(picker_preview, lv_color_hex(FACE_COLORS[face_idx]), 0); lv_obj_set_style_bg_opa(picker_preview, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(picker_preview, 40, 0); lv_obj_set_style_border_width(picker_preview, 0, 0);
    { lv_obj_t *pl = label(picker_preview, "10:09", &lv_font_montserrat_48, 0x111111); lv_obj_center(pl); }
    for (int i = 0; i < 6; i++) {
      lv_obj_t *b = lv_button_create(scr_picker); lv_obj_set_size(b, 70, 70); lv_obj_set_pos(b, 262 + i * 85, 470);
      lv_obj_set_style_bg_color(b, lv_color_hex(FACE_COLORS[i]), 0); lv_obj_set_style_radius(b, 20, 0);
      lv_obj_set_style_border_width(b, i == face_idx ? 4 : 0, 0); lv_obj_set_style_border_color(b, lv_color_hex(0x0A84FF), 0);
      lv_obj_add_event_cb(b, face_pick_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
      picker_tiles[i] = b;
    }
    picker_check_label = label(scr_picker, "Select a colour", &lv_font_montserrat_24, C_TEXT); lv_obj_align(picker_check_label, LV_ALIGN_BOTTOM_MID, 0, -20);
  }
  // HOME GRID — weather widget (real Open-Meteo or empty-state) + app
  // grid + bottom row as the dock. No FaceTime (no camera/mic, no
  // calling) and no Safari as a real browser (Search is Wikipedia only).
  // Those are omitted, not faked. Tap any tile to open that app.
  {
    scr_grid = lv_obj_create(NULL); style_screen(scr_grid);
    back_button(scr_grid, show_home_cb); lv_obj_align(lv_obj_get_child(scr_grid, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_grid, "Apps", &lv_font_montserrat_36, C_TEXT); lv_obj_align(lv_obj_get_child(scr_grid, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    lv_obj_t *wcard = lv_obj_create(scr_grid); lv_obj_set_size(wcard, 520, 90); lv_obj_set_pos(wcard, 252, 100); style_card(wcard);
    grid_weather_label = label(wcard, "Weather: waiting for Wi-Fi…", &lv_font_montserrat_24, C_TEXT); lv_obj_align(grid_weather_label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(grid_weather_label, LV_LABEL_LONG_WRAP); lv_obj_set_width(grid_weather_label, 480);
    grid_btn(scr_grid, 260, 205, "C", "Clock", 0x26262A, scr_clock);
    grid_btn(scr_grid, 392, 205, "W", "Weather", 0x246EDC, scr_live);
    grid_btn(scr_grid, 524, 205, "S", "Stocks", 0x141414, scr_stocks);
    grid_btn(scr_grid, 656, 205, "T", "Timer", 0xFF9500, scr_timer);
    grid_btn(scr_grid, 260, 305, "C", "Calendar", 0x26262A, scr_calendar);
    grid_btn(scr_grid, 392, 305, "T", "Tasks", 0xFFFFFF, scr_tasks);
    grid_btn(scr_grid, 524, 305, "C", "Calc", 0x28282C, scr_calc);
    grid_btn(scr_grid, 656, 305, "N", "Notes", 0xFFCC00, scr_notes);
    grid_btn(scr_grid, 260, 405, "S", "Search", 0x007AFF, scr_search);
    grid_btn(scr_grid, 392, 405, "M", "Music", 0xFF2D55, scr_music);
    grid_btn(scr_grid, 524, 405, "P", "Photos", 0x26262A, scr_photos);
    grid_btn(scr_grid, 656, 405, "M", "Maps", 0x34C759, scr_maps);
    grid_btn(scr_grid, 260, 495, "M", "Mail", 0x007AFF, scr_mail);
    grid_btn(scr_grid, 392, 495, "S", "Settings", 0x26262A, scr_settings);
    { lv_obj_t *wb = lv_button_create(scr_grid); lv_obj_set_size(wb, 64, 64); lv_obj_set_pos(wb, 524, 495);
      lv_obj_set_style_bg_color(wb, lv_color_hex(0x0A84FF), 0); lv_obj_set_style_radius(wb, 18, 0); lv_obj_set_style_border_width(wb, 0, 0);
      lv_obj_t *wg = lv_label_create(wb); lv_label_set_text(wg, "W"); lv_obj_set_style_text_font(wg, &lv_font_montserrat_24, 0); lv_obj_center(wg);
      lv_obj_add_event_cb(wb, open_widget_cb, LV_EVENT_CLICKED, NULL);
      label(scr_grid, "Widgets", &lv_font_montserrat_14, C_TEXT); lv_obj_set_pos(lv_obj_get_child(scr_grid, lv_obj_get_child_count(scr_grid) - 1), 524, 563); }
    { lv_obj_t *gb = lv_button_create(scr_grid); lv_obj_set_size(gb, 64, 64); lv_obj_set_pos(gb, 656, 495);
      lv_obj_set_style_bg_color(gb, lv_color_hex(0xC0C0C8), 0); lv_obj_set_style_radius(gb, 18, 0); lv_obj_set_style_border_width(gb, 0, 0);
      lv_obj_t *gg = lv_label_create(gb); lv_label_set_text(gg, "G"); lv_obj_set_style_text_font(gg, &lv_font_montserrat_24, 0); lv_obj_set_style_text_color(gg, lv_color_hex(0x111111), 0); lv_obj_center(gg);
      lv_obj_add_event_cb(gb, gb_open_guestbook_cb, LV_EVENT_CLICKED, NULL);
      label(scr_grid, "Guestbook", &lv_font_montserrat_14, C_TEXT); lv_obj_set_pos(lv_obj_get_child(scr_grid, lv_obj_get_child_count(scr_grid) - 1), 656, 563); }
  }
  // WIDGET SCREEN — huge date, temperature, small analog clock.
  // Buttons open Calendar / Weather / Clock. Values are live or
  // empty-state, never invented.
  {
    scr_widget = lv_obj_create(NULL); style_screen(scr_widget);
    back_button(scr_widget, show_home_cb); lv_obj_align(lv_obj_get_child(scr_widget, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    widget_weekday_label = label(scr_widget, "Set Wi-Fi for date", &lv_font_montserrat_24, C_TEXT); lv_obj_set_pos(widget_weekday_label, 252, 110);
    widget_date_label = label(scr_widget, "--", &lv_font_montserrat_48, C_TEXT); lv_obj_set_pos(widget_date_label, 252, 150);
    widget_temp_label = label(scr_widget, "Weather: waiting for Wi-Fi…", &lv_font_montserrat_24, C_TEXT); lv_obj_set_pos(widget_temp_label, 252, 230);
    lv_label_set_long_mode(widget_temp_label, LV_LABEL_LONG_WRAP); lv_obj_set_width(widget_temp_label, 500);
    widget_clock_label = label(scr_widget, "--:--", &lv_font_montserrat_36, C_TEXT); lv_obj_set_pos(widget_clock_label, 580, 110);
    { const char *wn[3] = {"Calendar", "Weather", "Clock"}; lv_obj_t *wt[3] = {scr_calendar, scr_live, scr_clock};
      for (int i = 0; i < 3; i++) { lv_obj_t *b = lv_button_create(scr_widget); lv_obj_set_size(b, 150, 48); lv_obj_set_pos(b, 252 + i * 168, 480);
        lv_obj_set_style_radius(b, 24, 0); lv_obj_set_style_border_width(b, 0, 0);
        lv_obj_t *l = lv_label_create(b); lv_label_set_text(l, wn[i]); lv_obj_center(l);
        lv_obj_add_event_cb(b, open_screen_cb, LV_EVENT_CLICKED, wt[i]); } }
  }
  // RECIPE + PHOTO CLOCK (second reel, Sripad approved 2026-11-01).
  // Recipe text is user-supplied over Serial only: RECIPE <title> | <ing1, ing2, ...>
  // Never a copied reel recipe, no brand names. Photo clock uses a neutral
  // placeholder panel (no TF driver exists, so TF loading is NOT claimed)
  // and never the user's real wallpaper.
  {
    scr_recipe = lv_obj_create(NULL); style_screen(scr_recipe);
    back_button(scr_recipe, show_home_cb); lv_obj_align(lv_obj_get_child(scr_recipe, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_recipe, "Recipe", &lv_font_montserrat_36, C_TEXT); lv_obj_align(lv_obj_get_child(scr_recipe, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    recipe_title_label = label(scr_recipe, "No recipe yet", &lv_font_montserrat_24, C_TEXT); lv_obj_set_pos(recipe_title_label, 252, 130);
    { lv_obj_t *ph = lv_obj_create(scr_recipe); lv_obj_set_size(ph, 200, 150); lv_obj_set_pos(ph, 252, 170);
      lv_obj_set_style_bg_color(ph, lv_color_hex(0xE8A87C), 0); lv_obj_set_style_bg_opa(ph, LV_OPA_COVER, 0);
      lv_obj_set_style_radius(ph, 24, 0); lv_obj_set_style_border_width(ph, 0, 0);
      lv_obj_t *g = label(ph, "Photo", &lv_font_montserrat_24, 0xFFFFFF); lv_obj_center(g); }
    label(scr_recipe, "Ingredients — tap to check off", &lv_font_montserrat_14, C_SUB); lv_obj_set_pos(lv_obj_get_child(scr_recipe, lv_obj_get_child_count(scr_recipe) - 1), 480, 170);
    for (int i = 0; i < 6; i++) { lv_obj_t *b = lv_button_create(scr_recipe); lv_obj_set_size(b, 320, 40); lv_obj_set_pos(b, 480, 200 + i * 48);
      lv_obj_set_style_radius(b, 16, 0); lv_obj_set_style_border_width(b, 0, 0);
      lv_obj_t *l = lv_label_create(b); lv_label_set_text(l, i < recipe_count ? recipe_ings[i].c_str() : ""); lv_obj_center(l);
      lv_obj_add_event_cb(b, recipe_toggle_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i); recipe_ing_labels[i] = l; }
    { lv_obj_t *st = pill_button(scr_recipe, "Start step timer", recipe_timer_cb); lv_obj_set_pos(st, 252, 480); }
    label(scr_recipe, "Set over Serial: RECIPE <title> | <ing1, ing2, ...>", &lv_font_montserrat_14, C_SUB); lv_obj_set_pos(lv_obj_get_child(scr_recipe, lv_obj_get_child_count(scr_recipe) - 1), 252, 540);
    scr_photoclock = lv_obj_create(NULL); style_screen(scr_photoclock);
    back_button(scr_photoclock, show_home_cb); lv_obj_align(lv_obj_get_child(scr_photoclock, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    photoclock_panel = lv_obj_create(scr_photoclock); lv_obj_set_size(photoclock_panel, 572, 572); lv_obj_set_pos(photoclock_panel, 226, 14);
    lv_obj_set_style_bg_color(photoclock_panel, lv_color_hex(PHOTO_COLORS[0]), 0); lv_obj_set_style_bg_opa(photoclock_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(photoclock_panel, 40, 0); lv_obj_set_style_border_width(photoclock_panel, 0, 0);
    lv_obj_add_flag(photoclock_panel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(photoclock_panel, photoclock_cycle_cb, LV_EVENT_CLICKED, NULL);
    photoclock_time_label = label(scr_photoclock, "10:09", &lv_font_montserrat_48, 0xFFFFFF); lv_obj_set_pos(photoclock_time_label, 260, 60);
    label(scr_photoclock, "Photo clock — neutral placeholder, tap photo to cycle", &lv_font_montserrat_14, 0xFFFFFF); lv_obj_set_pos(lv_obj_get_child(scr_photoclock, lv_obj_get_child_count(scr_photoclock) - 1), 260, 530);
  }

  // GUESTBOOK — sign-first flow (Sripad correction 2026-11-01, square copy only).
  // CHROME ONLY HERE: silver gradient panel + white chrome border on every
  // Guestbook screen. Old letters-first mockups (30/31) are superseded by
  // 32 signature pad, 33 animation, 34 name keyboard, 35 saved list.
  {
    gb_load();
    // Chrome panel helper inline: silver gradient, white border, radius 40.
    auto gb_chrome = [](lv_obj_t *scr) {
      lv_obj_t *pan = lv_obj_create(scr); lv_obj_set_size(pan, 572, 572); lv_obj_set_pos(pan, 226, 14);
      lv_obj_set_style_bg_color(pan, lv_color_hex(0xEDEDF2), 0); lv_obj_set_style_bg_grad_color(pan, lv_color_hex(0x9A9AA2), 0);
      lv_obj_set_style_bg_grad_dir(pan, LV_GRAD_DIR_VER, 0); lv_obj_set_style_bg_opa(pan, LV_OPA_COVER, 0);
      lv_obj_set_style_radius(pan, 40, 0); lv_obj_set_style_border_width(pan, 3, 0); lv_obj_set_style_border_color(pan, lv_color_hex(0xFFFFFF), 0);
      lv_obj_set_style_pad_all(pan, 0, 0); return pan;
    };
    // GUESTBOOK HOME (step 1): Leave your mark, SIGN THE GUESTBOOK,
    // RECENT ENTRIES (up to 3), VIEW ALL, total memories count.
    // The old full-list screen is superseded by home + gallery.
    scr_gb_home = lv_obj_create(NULL); style_screen(scr_gb_home);
    gb_chrome(scr_gb_home);
    { lv_obj_t *bb = back_button(scr_gb_home, show_home_cb); lv_obj_set_pos(bb, 252, 24); }
    label(scr_gb_home, "CORE MEMORIES", &lv_font_montserrat_24, 0x111111); lv_obj_set_pos(lv_obj_get_child(scr_gb_home, lv_obj_get_child_count(scr_gb_home) - 1), 252, 30);
    gb_home_count = label(scr_gb_home, "000 MEMORIES", &lv_font_montserrat_14, 0x555555); lv_obj_set_pos(gb_home_count, 620, 32);
    label(scr_gb_home, "Leave your mark.", &lv_font_montserrat_36, 0x111111); lv_obj_set_pos(lv_obj_get_child(scr_gb_home, lv_obj_get_child_count(scr_gb_home) - 1), 252, 70);
    { lv_obj_t *sb = lv_button_create(scr_gb_home); lv_obj_set_size(sb, 320, 52); lv_obj_set_pos(sb, 352, 125);
      lv_obj_set_style_bg_color(sb, lv_color_hex(0x111111), 0); lv_obj_set_style_radius(sb, 26, 0); lv_obj_set_style_border_width(sb, 0, 0);
      lv_obj_t *sl = lv_label_create(sb); lv_label_set_text(sl, "SIGN THE GUESTBOOK ->"); lv_obj_set_style_text_color(sl, lv_color_hex(0xFFFFFF), 0); lv_obj_center(sl);
      lv_obj_add_event_cb(sb, gb_open_sign_cb, LV_EVENT_CLICKED, NULL); }
    label(scr_gb_home, "RECENT ENTRIES", &lv_font_montserrat_14, 0x555555); lv_obj_set_pos(lv_obj_get_child(scr_gb_home, lv_obj_get_child_count(scr_gb_home) - 1), 252, 195);
    { lv_obj_t *va = lv_button_create(scr_gb_home); lv_obj_set_size(va, 120, 36); lv_obj_set_pos(va, 652, 190);
      lv_obj_set_style_bg_color(va, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_radius(va, 18, 0); lv_obj_set_style_border_width(va, 1, 0); lv_obj_set_style_border_color(va, lv_color_hex(0x9A9AA2), 0);
      lv_obj_t *vl = lv_label_create(va); lv_label_set_text(vl, "VIEW ALL ->"); lv_obj_set_style_text_color(vl, lv_color_hex(0x111111), 0); lv_obj_center(vl);
      lv_obj_add_event_cb(va, gb_open_gallery_cb, LV_EVENT_CLICKED, NULL); }
    gb_home_cont = lv_obj_create(scr_gb_home); lv_obj_set_size(gb_home_cont, 520, 230); lv_obj_set_pos(gb_home_cont, 252, 230);
    lv_obj_set_style_bg_opa(gb_home_cont, LV_OPA_TRANSP, 0); lv_obj_set_style_border_width(gb_home_cont, 0, 0); lv_obj_set_style_pad_all(gb_home_cont, 0, 0);
    lv_obj_set_scrollbar_mode(gb_home_cont, LV_SCROLLBAR_MODE_AUTO); lv_obj_add_flag(gb_home_cont, LV_OBJ_FLAG_SCROLLABLE);
    label(scr_gb_home, "Tap any entry to open it", &lv_font_montserrat_14, 0x555555); lv_obj_set_pos(lv_obj_get_child(scr_gb_home, lv_obj_get_child_count(scr_gb_home) - 1), 252, 470);
    gb_home_rebuild();
    // Legacy list screen object kept only so sq_frame/old refs compile; it is
    // not reachable from the UI (home + gallery supersede it).
    scr_guestbook = lv_obj_create(NULL); style_screen(scr_guestbook); gb_chrome(scr_guestbook);
    // SIGNATURE PAD
    scr_gb_sign = lv_obj_create(NULL); style_screen(scr_gb_sign);
    gb_chrome(scr_gb_sign);
    { lv_obj_t *bb = back_button(scr_gb_sign, gb_open_guestbook_cb); lv_obj_set_pos(bb, 252, 24); }
    label(scr_gb_sign, "Sign with your hand", &lv_font_montserrat_36, 0x111111); lv_obj_set_pos(lv_obj_get_child(scr_gb_sign, lv_obj_get_child_count(scr_gb_sign) - 1), 252, 80);
    label(scr_gb_sign, "Draw your signature below — no typing yet", &lv_font_montserrat_14, 0x555555); lv_obj_set_pos(lv_obj_get_child(scr_gb_sign, lv_obj_get_child_count(scr_gb_sign) - 1), 252, 125);
    gb_pad_panel = lv_obj_create(scr_gb_sign); lv_obj_set_size(gb_pad_panel, GB_PAD_W, GB_PAD_H); lv_obj_set_pos(gb_pad_panel, GB_PAD_X, GB_PAD_Y);
    lv_obj_set_style_bg_color(gb_pad_panel, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_bg_opa(gb_pad_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(gb_pad_panel, 24, 0); lv_obj_set_style_border_width(gb_pad_panel, 2, 0); lv_obj_set_style_border_color(gb_pad_panel, lv_color_hex(0x9A9AA2), 0);
    lv_obj_set_style_pad_all(gb_pad_panel, 0, 0); lv_obj_add_flag(gb_pad_panel, LV_OBJ_FLAG_CLICKABLE); lv_obj_clear_flag(gb_pad_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(gb_pad_panel, gb_pad_cb, LV_EVENT_ALL, NULL);
    label(scr_gb_sign, "Tip: lift your finger when you're done", &lv_font_montserrat_14, 0x555555); lv_obj_set_pos(lv_obj_get_child(scr_gb_sign, lv_obj_get_child_count(scr_gb_sign) - 1), 252, 410);
    { lv_obj_t *un = lv_button_create(scr_gb_sign); lv_obj_set_size(un, 160, 56); lv_obj_set_pos(un, 252, 450);
      lv_obj_set_style_bg_color(un, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_radius(un, 28, 0); lv_obj_set_style_border_width(un, 1, 0); lv_obj_set_style_border_color(un, lv_color_hex(0x9A9AA2), 0);
      lv_obj_t *unl = lv_label_create(un); lv_label_set_text(unl, "UNDO"); lv_obj_set_style_text_color(unl, lv_color_hex(0x111111), 0); lv_obj_center(unl);
      lv_obj_add_event_cb(un, gb_pad_undo_cb, LV_EVENT_CLICKED, NULL);
      lv_obj_t *cl = lv_button_create(scr_gb_sign); lv_obj_set_size(cl, 160, 56); lv_obj_set_pos(cl, 432, 450);
      lv_obj_set_style_bg_color(cl, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_radius(cl, 28, 0); lv_obj_set_style_border_width(cl, 1, 0); lv_obj_set_style_border_color(cl, lv_color_hex(0x9A9AA2), 0);
      lv_obj_t *cll = lv_label_create(cl); lv_label_set_text(cll, "Clear"); lv_obj_set_style_text_color(cll, lv_color_hex(0x111111), 0); lv_obj_center(cll);
      lv_obj_add_event_cb(cl, gb_pad_clear_cb, LV_EVENT_CLICKED, NULL);
      lv_obj_t *dn = lv_button_create(scr_gb_sign); lv_obj_set_size(dn, 160, 56); lv_obj_set_pos(dn, 612, 450);
      lv_obj_set_style_bg_color(dn, lv_color_hex(0x111111), 0); lv_obj_set_style_radius(dn, 28, 0); lv_obj_set_style_border_width(dn, 0, 0);
      lv_obj_t *dl = lv_label_create(dn); lv_label_set_text(dl, "Done  ->"); lv_obj_set_style_text_color(dl, lv_color_hex(0xFFFFFF), 0); lv_obj_center(dl);
      lv_obj_add_event_cb(dn, gb_pad_done_cb, LV_EVENT_CLICKED, NULL); }
    gb_pad_note = label(scr_gb_sign, "", &lv_font_montserrat_14, 0xFF3B30); lv_obj_set_pos(gb_pad_note, 252, 515);
    // ANIMATION (step 3, no generic confetti): ink-to-chrome sheen, stamp
    // press, liquid ripple — then guest details. Tap advances immediately.
    scr_gb_anim = lv_obj_create(NULL); style_screen(scr_gb_anim);
    gb_chrome(scr_gb_anim);
    gb_anim_sig_panel = lv_obj_create(scr_gb_anim); lv_obj_set_size(gb_anim_sig_panel, GB_PAD_W, GB_PAD_H); lv_obj_set_pos(gb_anim_sig_panel, GB_PAD_X, GB_PAD_Y);
    lv_obj_set_style_bg_color(gb_anim_sig_panel, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_bg_opa(gb_anim_sig_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(gb_anim_sig_panel, 24, 0); lv_obj_set_style_border_width(gb_anim_sig_panel, 2, 0); lv_obj_set_style_border_color(gb_anim_sig_panel, lv_color_hex(0x9A9AA2), 0);
    lv_obj_set_style_pad_all(gb_anim_sig_panel, 0, 0); lv_obj_clear_flag(gb_anim_sig_panel, LV_OBJ_FLAG_SCROLLABLE);
    gb_shine_bar = lv_obj_create(scr_gb_anim); lv_obj_set_size(gb_shine_bar, 80, 572); lv_obj_set_pos(gb_shine_bar, -160, 14);
    lv_obj_set_style_bg_color(gb_shine_bar, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_bg_opa(gb_shine_bar, 80, 0); lv_obj_set_style_border_width(gb_shine_bar, 0, 0);
    gb_anim_ripple = lv_obj_create(scr_gb_anim); lv_obj_set_size(gb_anim_ripple, 60, 60); lv_obj_set_pos(gb_anim_ripple, 482, 270);
    lv_obj_set_style_bg_opa(gb_anim_ripple, LV_OPA_TRANSP, 0); lv_obj_set_style_radius(gb_anim_ripple, 1000, 0); lv_obj_set_style_border_width(gb_anim_ripple, 3, 0); lv_obj_set_style_border_color(gb_anim_ripple, lv_color_hex(0xFFFFFF), 0);
    lv_obj_add_flag(gb_anim_ripple, LV_OBJ_FLAG_HIDDEN); lv_obj_clear_flag(gb_anim_ripple, LV_OBJ_FLAG_CLICKABLE);
    { gb_anim_stamp = lv_obj_create(scr_gb_anim); lv_obj_set_size(gb_anim_stamp, 300, 140); lv_obj_set_pos(gb_anim_stamp, 362, 60);
      lv_obj_set_style_bg_color(gb_anim_stamp, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_bg_opa(gb_anim_stamp, LV_OPA_COVER, 0);
      lv_obj_set_style_radius(gb_anim_stamp, 20, 0); lv_obj_set_style_border_width(gb_anim_stamp, 3, 0); lv_obj_set_style_border_color(gb_anim_stamp, lv_color_hex(0x111111), 0);
      lv_obj_set_style_transform_angle(gb_anim_stamp, -80, 0);
      lv_obj_t *sv = lv_label_create(gb_anim_stamp); lv_label_set_text(sv, "SAVED"); lv_obj_set_style_text_font(sv, &lv_font_montserrat_48, 0); lv_obj_set_style_text_color(sv, lv_color_hex(0x111111), 0); lv_obj_align(sv, LV_ALIGN_CENTER, 0, -20);
      lv_obj_t *sl = lv_label_create(gb_anim_stamp); lv_label_set_text(sl, "Signature locked in"); lv_obj_set_style_text_font(sl, &lv_font_montserrat_14, 0); lv_obj_set_style_text_color(sl, lv_color_hex(0x555555), 0); lv_obj_align(sl, LV_ALIGN_CENTER, 0, 35);
      lv_obj_add_flag(gb_anim_stamp, LV_OBJ_FLAG_HIDDEN); }
    label(scr_gb_anim, "Now — what's your name?", &lv_font_montserrat_36, 0x111111); lv_obj_set_pos(lv_obj_get_child(scr_gb_anim, lv_obj_get_child_count(scr_gb_anim) - 1), 252, 430);
    label(scr_gb_anim, "Your signature is locked in — next, your name.", &lv_font_montserrat_14, 0x555555); lv_obj_set_pos(lv_obj_get_child(scr_gb_anim, lv_obj_get_child_count(scr_gb_anim) - 1), 252, 480);
    lv_obj_add_flag(scr_gb_anim, LV_OBJ_FLAG_CLICKABLE); lv_obj_add_event_cb(scr_gb_anim, gb_anim_tap_cb, LV_EVENT_CLICKED, NULL);
    gb_anim_timer = lv_timer_create(gb_anim_timer_cb, 2600, NULL); lv_timer_pause(gb_anim_timer);
    gb_anim_phase_timer = lv_timer_create(gb_anim_phase_cb, 100, NULL); lv_timer_pause(gb_anim_phase_timer);
    gb_replay_timer = lv_timer_create(gb_replay_timer_cb, 60, NULL); lv_timer_pause(gb_replay_timer);
    gb_saved_timer = lv_timer_create(gb_saved_timer_cb, 150, NULL); lv_timer_pause(gb_saved_timer);
    // NAME KEYBOARD (after the signature + animation)
    scr_gb_name = lv_obj_create(NULL); style_screen(scr_gb_name);
    gb_chrome(scr_gb_name);
    { lv_obj_t *bb = back_button(scr_gb_name, gb_back_to_pad_cb); lv_obj_set_pos(bb, 252, 24); }
    label(scr_gb_name, "Almost done", &lv_font_montserrat_36, 0x111111); lv_obj_set_pos(lv_obj_get_child(scr_gb_name, lv_obj_get_child_count(scr_gb_name) - 1), 252, 80);
    label(scr_gb_name, "Your signature is saved — now your name", &lv_font_montserrat_14, 0x555555); lv_obj_set_pos(lv_obj_get_child(scr_gb_name, lv_obj_get_child_count(scr_gb_name) - 1), 252, 125);
    auto gb_field = [](lv_obj_t *scr, int y, int field_idx, lv_obj_t **btn_out, lv_obj_t **lbl_out) {
      lv_obj_t *b = lv_button_create(scr); lv_obj_set_size(b, 520, 44); lv_obj_set_pos(b, 252, y);
      lv_obj_set_style_bg_color(b, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_bg_opa(b, LV_OPA_COVER, 0);
      lv_obj_set_style_radius(b, 16, 0); lv_obj_set_style_border_width(b, 1, 0); lv_obj_set_style_border_color(b, lv_color_hex(0x9A9AA2), 0);
      lv_obj_t *l = lv_label_create(b); lv_label_set_text(l, ""); lv_obj_set_style_text_font(l, &lv_font_montserrat_24, 0); lv_obj_set_style_text_color(l, lv_color_hex(0x111111), 0); lv_obj_set_pos(l, 12, 8);
      lv_obj_add_event_cb(b, gb_field_cb, LV_EVENT_CLICKED, (void *)(intptr_t)field_idx); *btn_out = b; *lbl_out = l;
    };
    gb_field(scr_gb_name, 160, 0, &gb_name_field, &gb_name_field_label);
    gb_field(scr_gb_name, 210, 1, &gb_who_field, &gb_who_field_label);
    gb_name_note = label(scr_gb_name, "", &lv_font_montserrat_14, 0xFF3B30); lv_obj_set_pos(gb_name_note, 252, 260);
    { const char *rows[4] = {"ABCDEF", "GHIJKL", "MNOPQR", "STUVWX"};
      for (int r = 0; r < 4; r++) for (int c = 0; c < 6; c++) {
        lv_obj_t *k = lv_button_create(scr_gb_name); lv_obj_set_size(k, 78, 40); lv_obj_set_pos(k, 252 + c * 86, 285 + r * 46);
        lv_obj_set_style_bg_color(k, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_radius(k, 12, 0); lv_obj_set_style_border_width(k, 1, 0); lv_obj_set_style_border_color(k, lv_color_hex(0x9A9AA2), 0);
        char ch[2] = {rows[r][c], 0}; lv_obj_t *kl = lv_label_create(k); lv_label_set_text(kl, ch); lv_obj_set_style_text_font(kl, &lv_font_montserrat_24, 0); lv_obj_set_style_text_color(kl, lv_color_hex(0x111111), 0); lv_obj_center(kl);
        lv_obj_add_event_cb(k, gb_key_cb, LV_EVENT_CLICKED, (void *)(intptr_t)rows[r][c]);
      }
      auto gb_key2 = [](lv_obj_t *scr, int x, int w, const char *txt, int code) {
        lv_obj_t *k = lv_button_create(scr); lv_obj_set_size(k, w, 40); lv_obj_set_pos(k, x, 469);
        lv_obj_set_style_bg_color(k, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_radius(k, 12, 0); lv_obj_set_style_border_width(k, 1, 0); lv_obj_set_style_border_color(k, lv_color_hex(0x9A9AA2), 0);
        lv_obj_t *kl = lv_label_create(k); lv_label_set_text(kl, txt); lv_obj_set_style_text_font(kl, &lv_font_montserrat_24, 0); lv_obj_set_style_text_color(kl, lv_color_hex(0x111111), 0); lv_obj_center(kl);
        lv_obj_add_event_cb(k, gb_key_cb, LV_EVENT_CLICKED, (void *)(intptr_t)code);
      };
      gb_key2(scr_gb_name, 252, 78, "Y", 'Y'); gb_key2(scr_gb_name, 338, 78, "Z", 'Z');
      gb_key2(scr_gb_name, 424, 164, "Space", ' '); gb_key2(scr_gb_name, 596, 164, "<-", 8);
    }
    { lv_obj_t *sv = lv_button_create(scr_gb_name); lv_obj_set_size(sv, 250, 48); lv_obj_set_pos(sv, 252, 520);
      lv_obj_set_style_bg_color(sv, lv_color_hex(0x111111), 0); lv_obj_set_style_radius(sv, 24, 0); lv_obj_set_style_border_width(sv, 0, 0);
      lv_obj_t *sl = lv_label_create(sv); lv_label_set_text(sl, "Save memory"); lv_obj_set_style_text_color(sl, lv_color_hex(0xFFFFFF), 0); lv_obj_center(sl);
      lv_obj_add_event_cb(sv, gb_save_cb, LV_EVENT_CLICKED, NULL);
      lv_obj_t *bk = lv_button_create(scr_gb_name); lv_obj_set_size(bk, 250, 48); lv_obj_set_pos(bk, 522, 520);
      lv_obj_set_style_bg_color(bk, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_radius(bk, 24, 0); lv_obj_set_style_border_width(bk, 1, 0); lv_obj_set_style_border_color(bk, lv_color_hex(0x9A9AA2), 0);
      lv_obj_t *bl = lv_label_create(bk); lv_label_set_text(bl, "Back"); lv_obj_set_style_text_color(bl, lv_color_hex(0x111111), 0); lv_obj_center(bl);
      lv_obj_add_event_cb(bk, gb_back_to_pad_cb, LV_EVENT_CLICKED, NULL); }
    gb_fields_refresh();
    // MEMORIES GALLERY (step 7) with filters.
    scr_gb_gallery = lv_obj_create(NULL); style_screen(scr_gb_gallery);
    gb_chrome(scr_gb_gallery);
    { lv_obj_t *bb = back_button(scr_gb_gallery, gb_open_guestbook_cb); lv_obj_set_pos(bb, 252, 24); }
    label(scr_gb_gallery, "Memories", &lv_font_montserrat_36, 0x111111); lv_obj_set_pos(lv_obj_get_child(scr_gb_gallery, lv_obj_get_child_count(scr_gb_gallery) - 1), 252, 80);
    { const char *fn[4] = {"All", "Favorites", "With friends", "This week"};
      for (int f = 0; f < 4; f++) { lv_obj_t *b = lv_button_create(scr_gb_gallery); lv_obj_set_size(b, 120, 40); lv_obj_set_pos(b, 252 + f * 130, 125);
        lv_obj_set_style_bg_color(b, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_radius(b, 20, 0); lv_obj_set_style_border_width(b, 1, 0); lv_obj_set_style_border_color(b, lv_color_hex(0x9A9AA2), 0);
        lv_obj_t *l = lv_label_create(b); lv_label_set_text(l, fn[f]); lv_obj_set_style_text_font(l, &lv_font_montserrat_14, 0); lv_obj_set_style_text_color(l, lv_color_hex(0x111111), 0); lv_obj_center(l);
        lv_obj_add_event_cb(b, gb_filter_cb, LV_EVENT_CLICKED, (void *)(intptr_t)f); gb_filter_btns[f] = b; } }
    gb_gallery_cont = lv_obj_create(scr_gb_gallery); lv_obj_set_size(gb_gallery_cont, 520, 360); lv_obj_set_pos(gb_gallery_cont, 252, 180);
    lv_obj_set_style_bg_opa(gb_gallery_cont, LV_OPA_TRANSP, 0); lv_obj_set_style_border_width(gb_gallery_cont, 0, 0); lv_obj_set_style_pad_all(gb_gallery_cont, 0, 0);
    lv_obj_set_scrollbar_mode(gb_gallery_cont, LV_SCROLLBAR_MODE_AUTO); lv_obj_add_flag(gb_gallery_cont, LV_OBJ_FLAG_SCROLLABLE);
    gb_gallery_rebuild();
    // INDIVIDUAL MEMORY VIEW (step 8): replay + favorite star.
    scr_gb_detail = lv_obj_create(NULL); style_screen(scr_gb_detail);
    gb_chrome(scr_gb_detail);
    { lv_obj_t *bb = back_button(scr_gb_detail, gb_open_gallery_cb); lv_obj_set_pos(bb, 252, 24); }
    label(scr_gb_detail, "Memory", &lv_font_montserrat_36, 0x111111); lv_obj_set_pos(lv_obj_get_child(scr_gb_detail, lv_obj_get_child_count(scr_gb_detail) - 1), 252, 80);
    gb_detail_panel = lv_obj_create(scr_gb_detail); lv_obj_set_size(gb_detail_panel, GB_PAD_W, GB_PAD_H); lv_obj_set_pos(gb_detail_panel, GB_PAD_X, GB_PAD_Y);
    lv_obj_set_style_bg_color(gb_detail_panel, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_bg_opa(gb_detail_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(gb_detail_panel, 24, 0); lv_obj_set_style_border_width(gb_detail_panel, 2, 0); lv_obj_set_style_border_color(gb_detail_panel, lv_color_hex(0x9A9AA2), 0);
    lv_obj_set_style_pad_all(gb_detail_panel, 0, 0); lv_obj_clear_flag(gb_detail_panel, LV_OBJ_FLAG_SCROLLABLE);
    gb_detail_seal = lv_obj_create(scr_gb_detail); lv_obj_set_size(gb_detail_seal, 48, 48); lv_obj_set_pos(gb_detail_seal, 720, 165);
    lv_obj_set_style_bg_color(gb_detail_seal, lv_color_hex(0xC0C0C8), 0); lv_obj_set_style_bg_opa(gb_detail_seal, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(gb_detail_seal, 24, 0); lv_obj_set_style_border_width(gb_detail_seal, 2, 0); lv_obj_set_style_border_color(gb_detail_seal, lv_color_hex(0xFFFFFF), 0);
    { lv_obj_t *sl = lv_label_create(gb_detail_seal); lv_label_set_text(sl, "*"); lv_obj_set_style_text_font(sl, &lv_font_montserrat_24, 0); lv_obj_set_style_text_color(sl, lv_color_hex(0x111111), 0); lv_obj_center(sl); }
    lv_obj_add_flag(gb_detail_seal, LV_OBJ_FLAG_HIDDEN);
    gb_detail_name = label(scr_gb_detail, "", &lv_font_montserrat_36, 0x111111); lv_obj_set_pos(gb_detail_name, 252, 415);
    gb_detail_sub = label(scr_gb_detail, "", &lv_font_montserrat_14, 0x555555); lv_obj_set_pos(gb_detail_sub, 252, 460);
    { lv_obj_t *rp = lv_button_create(scr_gb_detail); lv_obj_set_size(rp, 200, 48); lv_obj_set_pos(rp, 252, 495);
      lv_obj_set_style_bg_color(rp, lv_color_hex(0x111111), 0); lv_obj_set_style_radius(rp, 24, 0); lv_obj_set_style_border_width(rp, 0, 0);
      lv_obj_t *rl = lv_label_create(rp); lv_label_set_text(rl, "REPLAY"); lv_obj_set_style_text_color(rl, lv_color_hex(0xFFFFFF), 0); lv_obj_center(rl);
      lv_obj_add_event_cb(rp, gb_replay_cb, LV_EVENT_CLICKED, NULL);
      lv_obj_t *fb = lv_button_create(scr_gb_detail); lv_obj_set_size(fb, 200, 48); lv_obj_set_pos(fb, 472, 495);
      lv_obj_set_style_bg_color(fb, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_radius(fb, 24, 0); lv_obj_set_style_border_width(fb, 1, 0); lv_obj_set_style_border_color(fb, lv_color_hex(0x9A9AA2), 0);
      gb_detail_star = lv_label_create(fb); lv_label_set_text(gb_detail_star, "Favorite"); lv_obj_set_style_text_color(gb_detail_star, lv_color_hex(0x111111), 0); lv_obj_center(gb_detail_star);
      lv_obj_add_event_cb(fb, gb_fav_cb, LV_EVENT_CLICKED, NULL); }
    // SAVED CONFIRMATION (step 5): signature + name + who + date/time, OK.
    scr_gb_saved = lv_obj_create(NULL); style_screen(scr_gb_saved);
    gb_chrome(scr_gb_saved);
    gb_saved_stamp = label(scr_gb_saved, "SAVED ✓", &lv_font_montserrat_36, 0x111111); lv_obj_set_pos(gb_saved_stamp, 252, 80);
    gb_saved_panel = lv_obj_create(scr_gb_saved); lv_obj_set_size(gb_saved_panel, GB_PAD_W, GB_PAD_H); lv_obj_set_pos(gb_saved_panel, GB_PAD_X, GB_PAD_Y);
    lv_obj_set_style_bg_color(gb_saved_panel, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_bg_opa(gb_saved_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(gb_saved_panel, 24, 0); lv_obj_set_style_border_width(gb_saved_panel, 2, 0); lv_obj_set_style_border_color(gb_saved_panel, lv_color_hex(0x9A9AA2), 0);
    lv_obj_set_style_pad_all(gb_saved_panel, 0, 0); lv_obj_clear_flag(gb_saved_panel, LV_OBJ_FLAG_SCROLLABLE);
    gb_saved_name = label(scr_gb_saved, "", &lv_font_montserrat_36, 0x111111); lv_obj_set_pos(gb_saved_name, 252, 415);
    gb_saved_sub = label(scr_gb_saved, "", &lv_font_montserrat_14, 0x555555); lv_obj_set_pos(gb_saved_sub, 252, 460);
    { lv_obj_t *ok = lv_button_create(scr_gb_saved); lv_obj_set_size(ok, 200, 48); lv_obj_set_pos(ok, 412, 495);
      lv_obj_set_style_bg_color(ok, lv_color_hex(0x111111), 0); lv_obj_set_style_radius(ok, 24, 0); lv_obj_set_style_border_width(ok, 0, 0);
      lv_obj_t *ol = lv_label_create(ok); lv_label_set_text(ol, "OK"); lv_obj_set_style_text_color(ol, lv_color_hex(0xFFFFFF), 0); lv_obj_center(ol);
      lv_obj_add_event_cb(ok, gb_open_guestbook_cb, LV_EVENT_CLICKED, NULL); }
    // REPLAY (tap an entry in the list)
    scr_gb_replay = lv_obj_create(NULL); style_screen(scr_gb_replay);
    gb_chrome(scr_gb_replay);
    { lv_obj_t *bb = back_button(scr_gb_replay, gb_back_to_detail_cb); lv_obj_set_pos(bb, 252, 24); }
    label(scr_gb_replay, "Signature", &lv_font_montserrat_36, 0x111111); lv_obj_set_pos(lv_obj_get_child(scr_gb_replay, lv_obj_get_child_count(scr_gb_replay) - 1), 252, 80);
    gb_replay_info = label(scr_gb_replay, "", &lv_font_montserrat_14, 0x555555); lv_obj_set_pos(gb_replay_info, 252, 125);
    gb_trace_dot = lv_obj_create(scr_gb_replay); lv_obj_set_size(gb_trace_dot, 12, 12);
    lv_obj_set_style_bg_color(gb_trace_dot, lv_color_hex(0x0A84FF), 0); lv_obj_set_style_bg_opa(gb_trace_dot, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(gb_trace_dot, 6, 0); lv_obj_set_style_border_width(gb_trace_dot, 0, 0);
    lv_obj_add_flag(gb_trace_dot, LV_OBJ_FLAG_HIDDEN);
    gb_replay_panel = lv_obj_create(scr_gb_replay); lv_obj_set_size(gb_replay_panel, GB_PAD_W, GB_PAD_H); lv_obj_set_pos(gb_replay_panel, GB_PAD_X, GB_PAD_Y);
    lv_obj_set_style_bg_color(gb_replay_panel, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_bg_opa(gb_replay_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(gb_replay_panel, 24, 0); lv_obj_set_style_border_width(gb_replay_panel, 2, 0); lv_obj_set_style_border_color(gb_replay_panel, lv_color_hex(0x9A9AA2), 0);
    lv_obj_set_style_pad_all(gb_replay_panel, 0, 0); lv_obj_clear_flag(gb_replay_panel, LV_OBJ_FLAG_SCROLLABLE);
    { lv_obj_t *rp = lv_button_create(scr_gb_replay); lv_obj_set_size(rp, 250, 48); lv_obj_set_pos(rp, 252, 430);
      lv_obj_set_style_bg_color(rp, lv_color_hex(0x111111), 0); lv_obj_set_style_radius(rp, 24, 0); lv_obj_set_style_border_width(rp, 0, 0);
      lv_obj_t *rl = lv_label_create(rp); lv_label_set_text(rl, "Replay"); lv_obj_set_style_text_color(rl, lv_color_hex(0xFFFFFF), 0); lv_obj_center(rl);
      lv_obj_add_event_cb(rp, [](lv_event_t *e) { (void)e; gb_replay_start(); }, LV_EVENT_CLICKED, NULL);
      lv_obj_t *bk = lv_button_create(scr_gb_replay); lv_obj_set_size(bk, 250, 48); lv_obj_set_pos(bk, 522, 430);
      lv_obj_set_style_bg_color(bk, lv_color_hex(0xFFFFFF), 0); lv_obj_set_style_radius(bk, 24, 0); lv_obj_set_style_border_width(bk, 1, 0); lv_obj_set_style_border_color(bk, lv_color_hex(0x9A9AA2), 0);
      lv_obj_t *bl = lv_label_create(bk); lv_label_set_text(bl, "Back"); lv_obj_set_style_text_color(bl, lv_color_hex(0x111111), 0); lv_obj_center(bl);
      lv_obj_add_event_cb(bk, gb_back_to_detail_cb, LV_EVENT_CLICKED, NULL); }
  }
  // HOME CONTROLS DELETED 2026-11-01 per Sripad (thermostat/scene/door/fan/intercom not needed).
  // HOME PAGES — tiles for the home tileview, built after every
  // target screen exists. Tile-local coordinates (tile is 600x600).
  // Page 1 Clock, Page 2 Widgets, Page 3 Apps. Home controls deleted.
  home_tiles[0] = lv_tileview_add_tile(home_tv, 0, 0, LV_DIR_RIGHT);
  home_tiles[1] = lv_tileview_add_tile(home_tv, 1, 0, (lv_dir_t)(LV_DIR_LEFT | LV_DIR_RIGHT));
  home_tiles[2] = lv_tileview_add_tile(home_tv, 2, 0, LV_DIR_LEFT);
    for (int i = 0; i < 3; i++) { lv_obj_set_style_bg_color(home_tiles[i], lv_color_hex(0x000000), 0); lv_obj_set_style_bg_opa(home_tiles[i], LV_OPA_COVER, 0); lv_obj_set_style_border_width(home_tiles[i], 0, 0); lv_obj_set_style_pad_all(home_tiles[i], 0, 0); }
  // Page 1 — Clock: exact face, face-colour preference, tap opens Clock app.
  {
    lv_obj_t *tl = home_tiles[0];
    bool dark_face = (face_idx == 4); uint32_t tc = dark_face ? 0xFFFFFF : 0x111111;
    home_face_panel = lv_obj_create(tl);
    lv_obj_set_size(home_face_panel, 572, 572); lv_obj_set_pos(home_face_panel, 14, 14);
    lv_obj_set_style_bg_color(home_face_panel, lv_color_hex(FACE_COLORS[face_idx]), 0);
    lv_obj_set_style_bg_opa(home_face_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(home_face_panel, 40, 0);
    lv_obj_set_style_border_width(home_face_panel, 0, 0);
    lv_obj_add_flag(home_face_panel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(home_face_panel, open_clock_cb, LV_EVENT_CLICKED, NULL);
    // Long-press on the clock face still reveals Customize (the old
    // scr_home long-press no longer fires through the tileview).
    lv_obj_add_event_cb(home_face_panel, home_longpress_cb, LV_EVENT_LONG_PRESSED, NULL);
    home_clock_label = label(home_face_panel, "10:09", &lv_font_montserrat_48, tc);
    lv_obj_align(home_clock_label, LV_ALIGN_TOP_MID, 0, 180);
    home_date_label = label(home_face_panel, "Tap for Clock app", &lv_font_montserrat_24, tc);
    lv_obj_align(home_date_label, LV_ALIGN_BOTTOM_MID, 0, -40);
    lv_obj_t *cp = pill_button(tl, "Customize", open_picker_cb); lv_obj_set_size(cp, 160, 48); lv_obj_set_pos(cp, 400, 30);
  }
  // Page 2 — Widgets: huge date, temperature, small clock, app buttons.
  {
    lv_obj_t *tl = home_tiles[1];
    tile_widget_weekday_label = label(tl, "Set Wi-Fi for date", &lv_font_montserrat_24, C_TEXT); lv_obj_set_pos(tile_widget_weekday_label, 40, 60);
    tile_widget_date_label = label(tl, "--", &lv_font_montserrat_48, C_TEXT); lv_obj_set_pos(tile_widget_date_label, 40, 100);
    tile_widget_temp_label = label(tl, "Weather: waiting for Wi-Fi…", &lv_font_montserrat_24, C_TEXT); lv_obj_set_pos(tile_widget_temp_label, 40, 180);
    lv_label_set_long_mode(tile_widget_temp_label, LV_LABEL_LONG_WRAP); lv_obj_set_width(tile_widget_temp_label, 500);
    tile_widget_clock_label = label(tl, "--:--", &lv_font_montserrat_36, C_TEXT); lv_obj_set_pos(tile_widget_clock_label, 380, 60);
    home_greeting_label = label(tl, "GOOD DAY, Profile 1", &lv_font_montserrat_24, C_TEXT); lv_obj_set_pos(home_greeting_label, 40, 260);
    home_hl_label = label(tl, "High/low: waiting for Wi-Fi…", &lv_font_montserrat_24, C_TEXT); lv_obj_set_pos(home_hl_label, 40, 300);
    home_cal_label = label(tl, "Calendar: no ICS URL set.", &lv_font_montserrat_14, C_SUB); lv_obj_set_pos(home_cal_label, 40, 340);
    lv_label_set_long_mode(home_cal_label, LV_LABEL_LONG_WRAP); lv_obj_set_width(home_cal_label, 500);
    // Manual profiles: generic letter avatars, tap to switch.
    // Tap to switch — no face recognition, no camera, no face scan.
    for (int i = 0; i < 6; i++) { lv_obj_t *b = lv_button_create(tl); lv_obj_set_size(b, 56, 56); lv_obj_set_pos(b, 40 + i * 88, 500);
      lv_obj_set_style_radius(b, 28, 0); lv_obj_set_style_border_width(b, i == profile_idx ? 4 : 0, 0); lv_obj_set_style_border_color(b, lv_color_hex(0x0A84FF), 0);
      lv_obj_t *l = lv_label_create(b); char d[2] = {(char)('1' + i), 0}; lv_label_set_text(l, d); lv_obj_center(l);
      lv_obj_add_event_cb(b, profile_pick_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i); profile_btns[i] = b; }
    label(tl, "Tap to switch — no face recognition", &lv_font_montserrat_14, C_SUB); lv_obj_set_pos(lv_obj_get_child(tl, lv_obj_get_child_count(tl) - 1), 40, 570);
    const char *wn[3] = {"Calendar", "Weather", "Clock"}; lv_obj_t *wt[3] = {scr_calendar, scr_live, scr_clock};
    for (int i = 0; i < 3; i++) { lv_obj_t *b = lv_button_create(tl); lv_obj_set_size(b, 150, 48); lv_obj_set_pos(b, 40 + i * 168, 420);
      lv_obj_set_style_radius(b, 24, 0); lv_obj_set_style_border_width(b, 0, 0);
      lv_obj_t *l = lv_label_create(b); lv_label_set_text(l, wn[i]); lv_obj_center(l);
      lv_obj_add_event_cb(b, open_screen_cb, LV_EVENT_CLICKED, wt[i]); }
  }
  // Page 3 — Apps grid: weather card + 16 tiles, tap opens that app.
  {
    lv_obj_t *tl = home_tiles[2];
    lv_obj_t *wcard = lv_obj_create(tl); lv_obj_set_size(wcard, 520, 70); lv_obj_set_pos(wcard, 40, 20); style_card(wcard);
    tile_grid_weather_label = label(wcard, "Weather: waiting for Wi-Fi…", &lv_font_montserrat_24, C_TEXT); lv_obj_align(tile_grid_weather_label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(tile_grid_weather_label, LV_LABEL_LONG_WRAP); lv_obj_set_width(tile_grid_weather_label, 480);
    grid_btn(tl, 48, 110, "C", "Clock", 0x26262A, scr_clock);
    grid_btn(tl, 180, 110, "W", "Weather", 0x246EDC, scr_live);
    grid_btn(tl, 312, 110, "S", "Stocks", 0x141414, scr_stocks);
    grid_btn(tl, 444, 110, "T", "Timer", 0xFF9500, scr_timer);
    grid_btn(tl, 48, 220, "C", "Calendar", 0x26262A, scr_calendar);
    grid_btn(tl, 180, 220, "T", "Tasks", 0xFFFFFF, scr_tasks);
    grid_btn(tl, 312, 220, "C", "Calc", 0x28282C, scr_calc);
    grid_btn(tl, 444, 220, "N", "Notes", 0xFFCC00, scr_notes);
    grid_btn(tl, 48, 330, "S", "Search", 0x007AFF, scr_search);
    grid_btn(tl, 180, 330, "M", "Music", 0xFF2D55, scr_music);
    grid_btn(tl, 312, 330, "P", "Photos", 0x26262A, scr_photos);
    grid_btn(tl, 444, 330, "M", "Maps", 0x34C759, scr_maps);
    grid_btn(tl, 48, 440, "M", "Mail", 0x007AFF, scr_mail);
    grid_btn(tl, 180, 440, "S", "Settings", 0x26262A, scr_settings);
    grid_btn(tl, 312, 440, "W", "Widgets", 0x007AFF, scr_widget);
    grid_btn(tl, 444, 440, "R", "Recipe", 0xFF9500, scr_recipe);
  }
  // Page dots + prev/next arrows, fixed on scr_home above the tileview.
  // Dots move on scroll end; arrows are the no-swipe fallback.
  {
    lv_obj_t *bar = lv_obj_create(scr_home); lv_obj_set_size(bar, 120, 24); lv_obj_set_pos(bar, 452, 560);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x000000), 0); lv_obj_set_style_bg_opa(bar, LV_OPA_50, 0);
    lv_obj_set_style_radius(bar, 12, 0); lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_CLICKABLE); lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
    for (int i = 0; i < 3; i++) {
      home_dots[i] = lv_obj_create(scr_home); lv_obj_set_size(home_dots[i], 10, 10); lv_obj_set_pos(home_dots[i], 462 + i * 26, 567);
      lv_obj_set_style_bg_color(home_dots[i], lv_color_hex(i == 0 ? 0x0A84FF : 0x8E8E93), 0); lv_obj_set_style_bg_opa(home_dots[i], LV_OPA_COVER, 0);
      lv_obj_set_style_radius(home_dots[i], 5, 0); lv_obj_set_style_border_width(home_dots[i], 0, 0);
      lv_obj_clear_flag(home_dots[i], LV_OBJ_FLAG_CLICKABLE); lv_obj_clear_flag(home_dots[i], LV_OBJ_FLAG_SCROLLABLE);
    }
    lv_obj_t *pv = lv_button_create(scr_home); lv_obj_set_size(pv, 32, 24); lv_obj_set_pos(pv, 412, 560);
    lv_obj_set_style_radius(pv, 12, 0); lv_obj_set_style_border_width(pv, 0, 0);
    lv_obj_t *pl = lv_label_create(pv); lv_label_set_text(pl, "<"); lv_obj_center(pl); lv_obj_add_event_cb(pv, home_page_prev_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *nx = lv_button_create(scr_home); lv_obj_set_size(nx, 32, 24); lv_obj_set_pos(nx, 580, 560);
    lv_obj_set_style_radius(nx, 12, 0); lv_obj_set_style_border_width(nx, 0, 0);
    lv_obj_t *nl = lv_label_create(nx); lv_label_set_text(nl, ">"); lv_obj_center(nl); lv_obj_add_event_cb(nx, home_page_next_cb, LV_EVENT_CLICKED, NULL);
  }
  // HOME GRID navigation: home itself is the clock (tap opens scr_grid,
  // see home_face_panel above). Customize (long-press, top of panel)
  // opens the face-colour picker, not the old theme screen.
  cust_btn = pill_button(scr_home, "Customize", open_picker_cb); lv_obj_set_size(cust_btn, 200, 56); lv_obj_set_pos(cust_btn, 580, 30); lv_obj_add_flag(cust_btn, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_event_cb(scr_home, home_longpress_cb, LV_EVENT_LONG_PRESSED, NULL);
  // LOCK SCREEN — large translucent time over a drawn wallpaper
  // (blue sky gradient, not a photo). Tap anywhere to unlock to Home.
  scr_lock = lv_obj_create(NULL); style_screen(scr_lock);
  // SQUARE lock: blue panel inside the silver frame, black side bars.
  lock_panel = lv_obj_create(scr_lock);
  lv_obj_set_size(lock_panel, 572, 572); lv_obj_set_pos(lock_panel, 226, 14);
  lv_obj_set_style_bg_color(lock_panel, lv_color_hex(0x6FB3FF), 0);
  lv_obj_set_style_bg_opa(lock_panel, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(lock_panel, 40, 0);
  lv_obj_set_style_border_width(lock_panel, 0, 0);
  lv_obj_clear_flag(lock_panel, LV_OBJ_FLAG_CLICKABLE);
  lock_time_label = label(scr_lock, "10:09", &lv_font_montserrat_48, 0x111111);
  lv_obj_align(lock_time_label, LV_ALIGN_TOP_LEFT, 260, 60);
  lv_obj_t *lock_date = label(scr_lock, "Sunday, Nov 1 - Tap to unlock", &lv_font_montserrat_24, 0x111111);
  lv_obj_align(lock_date, LV_ALIGN_TOP_LEFT, 260, 530);
  lv_obj_add_flag(scr_lock, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(scr_lock, show_home_cb, LV_EVENT_CLICKED, NULL);
  apply_face();
  // SQUARE FRAMES: centred 600x600 silver frame on every screen.
  sq_frame(scr_home); sq_frame(scr_timer); sq_frame(scr_clock);
  sq_frame(scr_settings); sq_frame(scr_live); sq_frame(scr_stocks);
  sq_frame(scr_calendar); sq_frame(scr_search); sq_frame(scr_photos);
  sq_frame(scr_tasks); sq_frame(scr_calc); // scr_split deleted
  sq_frame(scr_music); sq_frame(scr_notes); sq_frame(scr_maps);
  sq_frame(scr_mail); sq_frame(scr_customize); sq_frame(scr_camera);
  sq_frame(scr_web); sq_frame(scr_lock);
  sq_frame(scr_picker); sq_frame(scr_grid); sq_frame(scr_widget);
  sq_frame(scr_photo_view);
  sq_frame(scr_recipe); sq_frame(scr_photoclock);
  sq_frame(scr_guestbook); sq_frame(scr_gb_sign); sq_frame(scr_gb_anim); sq_frame(scr_gb_name); sq_frame(scr_gb_replay); sq_frame(scr_gb_home); sq_frame(scr_gb_gallery); sq_frame(scr_gb_detail); sq_frame(scr_gb_saved);
  refresh_segments();
  lv_screen_load(scr_lock);
  lv_timer_create(tick_cb, 1000, NULL);
}

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = (uint32_t)(area->x2 - area->x1 + 1);
  uint32_t h = (uint32_t)(area->y2 - area->y1 + 1);
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, (int32_t)w, (int32_t)h);
  tft.pushPixels((uint16_t *)px_map, (int32_t)(w * h));
  tft.endWrite();
  lv_display_flush_ready(disp);
}
static void touch_cb(lv_indev_t *indev, lv_indev_data_t *data) {
  (void)indev;
  uint16_t x = 0, y = 0;
  bool touched = tft.getTouch(&x, &y);
  data->point.x = (int16_t)x; data->point.y = (int16_t)y;
  data->state = touched ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}
static void try_wifi() {
  String ssid = prefs.getString("ssid", "");
  String pass = prefs.getString("pass", "");
  if (ssid.length() == 0) return;
  WiFi.begin(ssid.c_str(), pass.c_str());
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 10000) delay(500);
  if (WiFi.status() == WL_CONNECTED) {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    ntp_ok = true;
    fetch_real_data(); fetch_maps(); fetch_mail();
  }
}
static void serial_wifi() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.startsWith("ICS ")) {
    prefs.putString("ics", line.substring(4));
    Serial.println("ICS URL saved.");
    fetch_real_data();
    return;
  }
  if (line.startsWith("SEARCH ")) {
    String term = line.substring(7); term.trim();
    Serial.println("Searching Wikipedia...");
    fetch_search(term);
    return;
  }
  if (line.startsWith("ALARM ")) {
    int colon = line.indexOf(':');
    if (colon > 0) { alarm_h = line.substring(6, colon).toInt(); alarm_m = line.substring(colon + 1).toInt(); Serial.println("Alarm set."); }
    return;
  }
  if (line.startsWith("NOTE ")) {
    String note = line.substring(5);
    prefs.putString("note", note);
    if (note_label) lv_label_set_text(note_label, note.c_str());
    Serial.println("Note saved.");
    return;
  }
  if (line.startsWith("MAPS ")) {
    int sp = line.indexOf(' ', 5);
    if (sp > 0) { map_lat = line.substring(5, sp).toFloat(); map_lon = line.substring(sp + 1).toFloat(); Serial.println("Map centre saved."); fetch_maps(); }
    return;
  }
  if (line.startsWith("MAIL ")) {
    int sp = line.indexOf(' ', 5);
    if (sp > 0) { prefs.putString("mail_user", line.substring(5, sp)); prefs.putString("mail_pass", line.substring(sp + 1)); Serial.println("Mail saved. Fetching..."); fetch_mail(); }
    return;
  }
  if (line.startsWith("RECIPE ")) {
    String rest = line.substring(7); int bar = rest.indexOf('|');
    recipe_title = (bar >= 0 ? rest.substring(0, bar) : rest); recipe_title.trim();
    recipe_count = 0; for (int i = 0; i < 6; i++) { recipe_ings[i] = ""; recipe_checked[i] = false; }
    if (bar >= 0) { String ing = rest.substring(bar + 1); int pos = 0; while (recipe_count < 6) { int c = ing.indexOf(',', pos); String one = (c < 0 ? ing.substring(pos) : ing.substring(pos, c)); one.trim(); if (one.length() > 0) recipe_ings[recipe_count++] = one; if (c < 0) break; pos = c + 1; } }
    prefs.putString("recipe_title", recipe_title); recipe_refresh(); Serial.println("Recipe saved.");
    return;
  }
  if (line.startsWith("PROFILE ")) {
    int sp = line.indexOf(' ', 8);
    if (sp > 0) { int n = line.substring(8, sp).toInt(); String nm = line.substring(sp + 1); nm.trim(); if (n >= 1 && n <= 6 && nm.length() > 0) { profile_names[n - 1] = nm; profile_idx = n - 1; prefs.putInt("profile", profile_idx); greeting_update(); Serial.println("Profile saved."); } }
    return;
  }
  if (line.startsWith("FINNHUB ")) {
    // API key set by the user himself, over Serial only — never hardcoded,
    // never pasted in chat, never written to the repo.
    prefs.putString("finnhub", line.substring(8));
    Serial.println("Finnhub key saved.");
    return;
  }
  if (!line.startsWith("WIFI ")) return;
  int sp = line.indexOf(' ', 5);
  if (sp < 0) return;
  prefs.putString("ssid", line.substring(5, sp));
  prefs.putString("pass", line.substring(sp + 1));
  Serial.println("Wi-Fi saved. Reconnecting...");
  try_wifi();
}

void setup() {
  Serial.begin(115200);
  prefs.begin("walldisplay", false);
  split_mode = false; // SPLIT DELETED 2026-11-01: always single-app full screen
  tft.begin();
  tft.setBrightness(200);
  lv_init();
  static lv_color_t buf[1024 * 40];
  lv_display_t *disp = lv_display_create(1024, 600);
  lv_display_set_buffers(disp, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, flush_cb);
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touch_cb);
  build_ui();
  try_wifi();
}

void loop() {
  lv_tick_inc(5);
  lv_timer_handler();
  serial_wifi();
  static unsigned long last_fetch = 0;
  if (WiFi.status() == WL_CONNECTED && millis() - last_fetch > 600000UL) {
    last_fetch = millis();
    fetch_real_data();
  }
  delay(5);
}

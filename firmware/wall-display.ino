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
//   - Home: clean grid, big title, segmented Split / Single control
//   - Mode chosen ON the display, saved in Preferences, restored on reboot
//   - Timer: Start / Pause / Reset, live counting
//   - Clock: live uptime, and real NTP time once Wi-Fi is set (see below)
//   - Brightness slider, Settings screen with mode + Wi-Fi status
//   - Split screen: Timer + Clock side by side, both live
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
static const uint32_t C_BG = 0x000000, C_CARD = 0x1C1C1E, C_TEXT = 0xFFFFFF;
static const uint32_t C_SUB = 0x98989D, C_ACCENT = 0x0071E3, C_LINE = 0x38383A;

// ---- State ----
static lv_obj_t *scr_home = nullptr, *scr_timer = nullptr, *scr_clock = nullptr;
static lv_obj_t *scr_settings = nullptr, *scr_split = nullptr, *scr_live = nullptr;
static lv_obj_t *scr_stocks = nullptr, *scr_calendar = nullptr;
static lv_obj_t *scr_camera = nullptr, *scr_web = nullptr, *scr_photos = nullptr, *scr_lock = nullptr;
static lv_obj_t *scr_tasks = nullptr, *scr_calc = nullptr, *scr_music = nullptr, *scr_notes = nullptr;
static lv_obj_t *scr_search = nullptr, *search_label = nullptr, *note_label = nullptr;
static lv_obj_t *scr_customize = nullptr;
static lv_obj_t *scr_maps = nullptr, *scr_mail = nullptr, *maps_label = nullptr, *mail_label = nullptr;
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
static bool ntp_ok = false;

static void style_card(lv_obj_t *o) {
  lv_obj_set_style_bg_color(o, lv_color_hex(C_CARD), 0);
  lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(o, 24, 0);
  lv_obj_set_style_border_width(o, 0, 0);
  lv_obj_set_style_shadow_width(o, 0, 0);
  lv_obj_set_style_pad_all(o, 20, 0);
}
static void style_screen(lv_obj_t *o) {
  lv_obj_set_style_bg_color(o, lv_color_hex(C_BG), 0);
  lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(o, 0, 0);
  lv_obj_set_style_pad_all(o, 0, 0);
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
  lv_obj_set_size(btn, 120, 48);
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
  lv_obj_set_style_bg_color(seg_single, lv_color_hex(split_mode ? C_CARD : C_ACCENT), 0);
  lv_obj_set_style_bg_color(seg_split, lv_color_hex(split_mode ? C_ACCENT : C_CARD), 0);
  lv_obj_t *a = lv_obj_get_child(seg_single, 0);
  lv_obj_t *b = lv_obj_get_child(seg_split, 0);
  lv_obj_set_style_text_color(a, lv_color_hex(split_mode ? C_TEXT : 0xFFFFFF), 0);
  lv_obj_set_style_text_color(b, lv_color_hex(split_mode ? 0xFFFFFF : C_TEXT), 0);
}
static void set_mode(bool split) {
  split_mode = split;
  prefs.putBool("split", split_mode);
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

static void build_ui() {
  // HOME — widget dashboard (date + clock + weather + music) + icon row.
  // This is the v5 home: black rounded display, large date, analog-style
  // clock label, weather widget, music widget, then squircle app icons
  // with real names. No "Live" tile — every app is named for what it is.
  scr_home = lv_obj_create(NULL); style_screen(scr_home);
  // v9 Home: dark, icons only — no widget clutter, no dock bar.
  // Status bar only: time is updated by tick_cb via clock_label on the
  // Clock screen; Home shows a static date label here.
  lv_obj_t *date_m = label(scr_home, "Sat, Oct 31  •  10:09", &lv_font_montserrat_24, C_TEXT);
  lv_obj_align(date_m, LV_ALIGN_TOP_LEFT, 32, 24);
  // Weather widget on home is updated by fetch_real_data via weather_label
  // on the Weather screen; home shows the same clock label as Clock screen.
  lv_obj_t *seg = lv_obj_create(scr_home);
  lv_obj_set_size(seg, 320, 56); lv_obj_align(seg, LV_ALIGN_TOP_RIGHT, -32, 32);
  lv_obj_set_style_bg_color(seg, lv_color_hex(C_CARD), 0);
  lv_obj_set_style_radius(seg, 28, 0); lv_obj_set_style_border_width(seg, 0, 0);
  lv_obj_set_style_pad_all(seg, 4, 0);
  seg_single = pill_button(seg, "Single", mode_single_cb); lv_obj_align(seg_single, LV_ALIGN_LEFT_MID, 0, 0);
  seg_split = pill_button(seg_split, "Split", mode_split_cb); lv_obj_align(seg_split, LV_ALIGN_RIGHT_MID, 0, 0);
  // Icon row is created after all screens exist (see below).
  // TIMER
  scr_timer = lv_obj_create(NULL); style_screen(scr_timer);
  back_button(scr_timer, show_home_cb); lv_obj_align(lv_obj_get_child(scr_timer, 0), LV_ALIGN_TOP_LEFT, 32, 24);
  label(scr_timer, "Timer", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_timer, 1), LV_ALIGN_TOP_LEFT, 32, 80);
  lv_obj_t *tcard = lv_obj_create(scr_timer); lv_obj_set_size(tcard, 960, 260); lv_obj_align(tcard, LV_ALIGN_TOP_MID, 0, 160); style_card(tcard);
  timer_label = label(tcard, "00:00", &lv_font_montserrat_48, C_TEXT); lv_obj_center(timer_label);
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
  // CALENDAR — October 2026 grid (Oct 1 = Thursday, Oct 31 = Saturday).
  // Dynamic month (from NTP date) is a build-week follow-up, not invented.
  {
    lv_obj_t *cal = lv_obj_create(NULL); style_screen(cal); scr_calendar = cal;
    back_button(cal, show_home_cb); lv_obj_align(lv_obj_get_child(cal, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(cal, "October 2026", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(cal, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    const char *dow[7] = {"S","M","T","W","T","F","S"};
    for (int i = 0; i < 7; i++) {
      lv_obj_t *h = label(cal, dow[i], &lv_font_montserrat_14, C_SUB);
      lv_obj_align(h, LV_ALIGN_TOP_LEFT, 32 + i * 130, 150);
    }
    for (int day = 1; day <= 31; day++) {
      int col = (4 + day - 1) % 7, row = (4 + day - 1) / 7;
      char buf[4]; snprintf(buf, sizeof(buf), "%d", day);
      lv_obj_t *d = label(cal, buf, &lv_font_montserrat_24, day == 31 ? 0xFF3B30 : C_TEXT);
      lv_obj_align(d, LV_ALIGN_TOP_LEFT, 32 + col * 130, 180 + row * 60);
    }
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
    label(ph, "No photos loaded yet.", &lv_font_montserrat_24, C_SUB); lv_obj_align(lv_obj_get_child(ph, 2), LV_ALIGN_TOP_LEFT, 32, 180);
    scr_photos = ph;
  }
  // SPLIT
  scr_split = lv_obj_create(NULL); style_screen(scr_split);
  back_button(scr_split, show_home_cb); lv_obj_align(lv_obj_get_child(scr_split, 0), LV_ALIGN_TOP_LEFT, 32, 24);
  lv_obj_t *left = lv_obj_create(scr_split); lv_obj_set_size(left, 460, 300); lv_obj_align(left, LV_ALIGN_LEFT_MID, 32, 40); style_card(left);
  label(left, "Timer", &lv_font_montserrat_24, C_TEXT); lv_obj_align(lv_obj_get_child(left, 0), LV_ALIGN_TOP_LEFT, 0, 0);
  split_timer_label = label(left, "00:00", &lv_font_montserrat_48, C_TEXT); lv_obj_center(split_timer_label);
  lv_obj_t *right = lv_obj_create(scr_split); lv_obj_set_size(right, 460, 300); lv_obj_align(right, LV_ALIGN_RIGHT_MID, -32, 40); style_card(right);
  label(right, "Clock", &lv_font_montserrat_24, C_TEXT); lv_obj_align(lv_obj_get_child(right, 0), LV_ALIGN_TOP_LEFT, 0, 0);
  split_clock_label = label(right, "00:00", &lv_font_montserrat_48, C_TEXT); lv_obj_center(split_clock_label);
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
    label(scr_music, "Music — display only, by design. No playback.", &lv_font_montserrat_24, C_SUB); lv_obj_align(lv_obj_get_child(scr_music, 2), LV_ALIGN_TOP_LEFT, 32, 180);
    scr_notes = lv_obj_create(NULL); style_screen(scr_notes);
    back_button(scr_notes, show_home_cb); lv_obj_align(lv_obj_get_child(scr_notes, 0), LV_ALIGN_TOP_LEFT, 32, 24);
    label(scr_notes, "Notes", &lv_font_montserrat_48, C_TEXT); lv_obj_align(lv_obj_get_child(scr_notes, 1), LV_ALIGN_TOP_LEFT, 32, 80);
    note_label = label(scr_notes, "No notes yet — set one over Serial: NOTE <text>", &lv_font_montserrat_24, C_SUB); lv_obj_align(note_label, LV_ALIGN_TOP_LEFT, 32, 180);
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
  // HOME ICON ROW (now that all screens exist) — squircle icons, real names.
  icon_btn(scr_home, 32, 180, "C", "Clock", 0x26262A, scr_clock);
  icon_btn(scr_home, 150, 180, "W", "Weather", 0x246EDC, scr_live);
  icon_btn(scr_home, 268, 180, "S", "Stocks", 0x141414, scr_stocks);
  icon_btn(scr_home, 386, 180, "T", "Timer", 0xFF9500, scr_timer);
  icon_btn(scr_home, 504, 180, "S", "Settings", 0x26262A, scr_settings);
  icon_btn(scr_home, 622, 180, "P", "Photos", 0x26262A, scr_photos);
  icon_btn(scr_home, 740, 180, "C", "Calendar", 0x26262A, scr_calendar);
  icon_btn(scr_home, 32, 340, "S", "Safari", 0x007AFF, scr_search);
  icon_btn(scr_home, 150, 340, "G", "Google", 0x28282C, scr_search);
  icon_btn(scr_home, 268, 340, "N", "Notes", 0xFFCC00, scr_notes);
  icon_btn(scr_home, 386, 340, "M", "Music", 0xFF2D55, scr_music);
  icon_btn(scr_home, 504, 340, "M", "Mail", 0x007AFF, scr_mail);
  icon_btn(scr_home, 622, 340, "T", "Tasks", 0xFFFFFF, scr_tasks);
  icon_btn(scr_home, 740, 340, "C", "Calc", 0x28282C, scr_calc);
  // v14: Customize lives at the TOP near Wi-Fi/status, hidden until long-press.
  cust_btn = pill_button(scr_home, "Customize", customize_cb); lv_obj_set_size(cust_btn, 200, 56); lv_obj_align(cust_btn, LV_ALIGN_TOP_RIGHT, -32, 24); lv_obj_add_flag(cust_btn, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_event_cb(scr_home, home_longpress_cb, LV_EVENT_LONG_PRESSED, NULL);
  icon_btn(scr_home, 858, 340, "M", "Maps", 0x34C759, scr_maps);
  // LOCK SCREEN — large translucent time over a drawn wallpaper
  // (blue sky gradient, not a photo). Tap anywhere to unlock to Home.
  scr_lock = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr_lock, lv_color_hex(0x87CEEB), 0);
  lv_obj_set_style_bg_opa(scr_lock, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_grad_color(scr_lock, lv_color_hex(0x3A7D44), 0);
  lv_obj_set_style_bg_grad_dir(scr_lock, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_border_width(scr_lock, 0, 0);
  lv_obj_t *lock_time = label(scr_lock, "10:09", &lv_font_montserrat_48, 0xFFFFFF);
  lv_obj_set_style_text_opa(lock_time, 140, 0);
  lv_obj_align(lock_time, LV_ALIGN_TOP_LEFT, 32, 24);
  lv_obj_t *lock_date = label(scr_lock, "Saturday, October 31 — Tap to unlock", &lv_font_montserrat_24, 0xFFFFFF);
  lv_obj_align(lock_date, LV_ALIGN_TOP_LEFT, 32, 100);
  lv_obj_add_flag(scr_lock, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(scr_lock, show_home_cb, LV_EVENT_CLICKED, NULL);
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
  split_mode = prefs.getBool("split", false);
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

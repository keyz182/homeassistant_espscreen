#pragma once
// A tile's choice from a fixed set of words (2026-11-25): its display, its inline control, a lock's guard, a wide
// card's controls and a calendar's first view (runtime_model.h Tile). Each was a std::string, 24 bytes a field on a
// 32-bit chip; a byte that says which word of its set it holds costs one, in every tile of every board.
//
// It reads as the string did: compared with a word (`t.display == "watch"`), set from the option's text
// (`tile.controls = string(options["controls"], 24)`), empty or not, and its text where a word is handed on
// (`c_str()`). The first word of a set is a new tile's. The sets hold every word the app sends (the catalogue's
// displays and controls, catalogue/_tile.yaml and the type files) and the empty word where the option may be missing.
//
// A word that is not in its set (a newer app's) is OTHER, as an unknown string was before: equal to no word of the
// set, not empty, and "?" where its text is asked for, which no code compares with. A comparison with a word that is
// not in the set is a mistake in the code, not in the data: the host tests stop on it (tests/test_tile_words.cpp).
//
// Free of LVGL and of ESPHome.
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace tile_words {

template <class Set> class Word {
 public:
  static constexpr uint8_t OTHER = 0xFF;
  static constexpr size_t COUNT = sizeof(Set::words) / sizeof(Set::words[0]);
  static_assert(COUNT < OTHER, "a set's words fit in a byte beside OTHER");

  Word &operator=(std::string_view text) { code_ = find(text); return *this; }
  bool operator==(std::string_view word) const {
    known(word);
    return code_ != OTHER && word == Set::words[code_];
  }
  bool operator!=(std::string_view word) const { return !(*this == word); }
  bool empty() const { return code_ != OTHER && Set::words[code_][0] == '\0'; }
  // The word's text, as long as the program runs; "?" for a word of a newer app.
  const char *c_str() const { return code_ == OTHER ? "?" : Set::words[code_]; }
  uint8_t code() const { return code_; }

  static uint8_t find(std::string_view text) {
    for (size_t i = 0; i < COUNT; ++i)
      if (text == Set::words[i]) return uint8_t(i);
    return OTHER;
  }

 private:
  static void known(std::string_view word) {
#ifndef USE_ESP32
    assert(find(word) != OTHER && "compared with a word its set does not hold");
#endif
    (void) word;
  }
  uint8_t code_ = 0;
};

// What the tile shows (catalogue `displays`); "standard" when the option is missing.
struct Displays {
  static constexpr const char *words[] = {"standard", "", "analog", "cover", "dial", "digital", "favorite", "flip",
                                          "forecast", "graph", "live", "map", "meter", "sunpath", "watch"};
};
// The small slider of a wide card; "none" when the option is missing.
struct Inlines {
  static constexpr const char *words[] = {"none", "", "slider"};
};
// How far a lock's tile may go (catalogue/lock.yaml `guards`); "confirm" when the option is missing.
struct Guards {
  static constexpr const char *words[] = {"confirm", "", "lock_only"};
};
// A wide card's direct controls (catalogue `controls`); empty keeps the plain card. "none" is what an older app may send.
struct Controls {
  static constexpr const char *words[] = {
      "",         "none",     "brightness", "brightness_presets", "buttons", "buttons_tilt", "map",
      "map_rooms", "mode",    "playback",   "position",           "position_tilt", "presets", "rooms",
      "routines", "run",      "setpoint",   "setpoint_mode",      "slider",  "speed",        "stepper",
      "tilt",     "toggle",   "volume",     "volume_playback"};
};
// A calendar's first view on its full screen (screen_manager calendar_feed.py START_VIEWS); empty is its own choice.
struct Views {
  static constexpr const char *words[] = {"", "auto", "up_next", "day", "week", "month"};
};

}  // namespace tile_words

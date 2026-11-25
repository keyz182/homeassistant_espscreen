// tile_words::Word: a tile's fixed-set fields read as the strings they replaced (components/smart_display/tile_words.h).
#include "components/smart_display/tile_words.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

using namespace tile_words;

int main() {
  // A new tile holds the first word of each set: the old string defaults.
  Word<Displays> display;
  Word<Inlines> inline_control;
  Word<Guards> guard;
  Word<Controls> controls;
  Word<Views> view;
  assert(display == "standard" && !display.empty());
  assert(inline_control == "none" && guard == "confirm");
  assert(controls.empty() && controls == "" && view.empty());

  // Set from the option's text, as page_receiver does, and compared both ways.
  controls = std::string("brightness_presets");
  assert(controls == "brightness_presets" && controls != "brightness" && !controls.empty());
  assert(std::strcmp(controls.c_str(), "brightness_presets") == 0);
  display = "watch";
  assert(display == "watch" && display != "standard");
  view = std::string("week");
  assert(view == "week" && std::string(view.c_str()) == "week");

  // Empty text is the empty word, so a missing option can still fall back to the default.
  display = std::string();
  assert(display.empty() && display != "standard");

  // A word of a newer app: no word of the set, not empty, "?" as text.
  controls = "a_newer_control";
  assert(controls.code() == Word<Controls>::OTHER);
  assert(!controls.empty() && controls != "" && controls != "none" && controls != "slider");
  assert(std::strcmp(controls.c_str(), "?") == 0);
  guard = "open_only";
  assert(guard != "confirm" && guard != "lock_only" && !guard.empty());

  // Every word of a set comes back as itself.
  for (const char *w : Controls::words) {
    controls = w;
    assert(controls == w && std::strcmp(controls.c_str(), w) == 0);
  }

  static_assert(sizeof(Word<Controls>) == 1, "a word is a byte");
  std::puts("tile_words: ok");
  return 0;
}

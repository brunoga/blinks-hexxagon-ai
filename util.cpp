#include "util.h"

#include <blinklib.h>

#include "game_map.h"
#include "game_message.h"

namespace util {

bool NoSleepButtonSingleClicked() {
  return buttonSingleClicked() && !hasWoken();
}

}  // namespace util

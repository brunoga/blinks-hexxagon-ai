#include <blinklib.h>

#include "blink_state.h"
#include "game_map.h"
#include "src/blinks-broadcast/manager.h"

void setup() {}

void loop() {
  if (buttonSingleClicked()) {
    blink::state::SetPlayer(game::player::GetNext(blink::state::GetPlayer()));
  }

  if (!game::map::MaybeDownload()) {
    broadcast::manager::Process();

    // TODO(bga): Handle game states here.
  }

  blink::state::Render();
}

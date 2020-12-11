#include <blinklib.h>

#include "blink_state.h"
#include "game_map.h"
#include "game_state.h"
#include "game_state_play.h"
#include "src/blinks-broadcast/manager.h"

void setup() {}

void loop() {
  if (buttonSingleClicked()) {
    blink::state::SetPlayer(game::player::GetNext(blink::state::GetPlayer()));
  }

  if (!game::map::MaybeDownload()) {
    broadcast::manager::Process();

    if (game::state::Get() < GAME_STATE_PLAY) {
      // Reset map if the state is IDLE or SETUP.
      game::map::Reset();
    } else if (game::state::Get() == GAME_STATE_PLAY) {
      // We are in the play state. Delegate to the state-specific handler.
      game::state::play::Handler();
    }
  }

  blink::state::Render();
}

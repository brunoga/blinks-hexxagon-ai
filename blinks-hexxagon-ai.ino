#include <blinklib.h>

#include "blink_state.h"
#include "game_map.h"
#include "game_message.h"
#include "game_state.h"
#include "game_state_play.h"
#include "util.h"

void setup() {
  blink::state::Reset();
  blink::state::SetPlayer(1);  // Start with first real player.
}

void loop() {
  game::message::Process();

  bool button_clicked = util::NoSleepButtonSingleClicked();

  if (game::map::Downloaded()) {
    switch (game::state::Get()) {
      case GAME_STATE_PLAY:
        game::state::play::Handler();
        break;
      default:
        game::map::Reset();
        break;
    }
  } else {
    if (button_clicked) {
      byte next_player = game::player::GetNext(blink::state::GetPlayer());

      blink::state::SetPlayer(next_player == 0 ? 1 : next_player);
    }

    game::state::play::Reset();
  }

  blink::state::Render();
}

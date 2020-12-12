#include <blinklib.h>

#include "blink_state.h"
#include "game_map.h"
#include "game_message.h"
#include "game_state.h"
#include "game_state_play.h"
#include "util.h"

void setup() { blink::state::Reset(); }

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
  } else if (button_clicked) {
    blink::state::SetPlayer(game::player::GetNext(blink::state::GetPlayer()));
  }

  blink::state::Render();
}

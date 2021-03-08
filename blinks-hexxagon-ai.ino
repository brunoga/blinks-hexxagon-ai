#include <blinklib.h>

#include "blink_state.h"
#include "blink_state_face.h"
#include "game_map.h"
#include "game_map_download.h"
#include "game_message.h"
#include "game_state.h"
#include "game_state_play.h"
#include "util.h"

void setup() {
  randomize();

  blink::state::Reset();  // Called to correctly initialize map_requested_face.
  blink::state::SetPlayer(1);  // Start with first real player.
}

void loop() {
  blink::state::face::handler::ProcessTop();

  bool button_clicked = util::NoSleepButtonSingleClicked();
  bool button_double_clicked = buttonDoubleClicked();

  // Process any map donwload in progress.
  game::map::download::Process();

  // And also process any pending game messages.
  game::message::Process();

  if (game::map::download::Downloaded()) {
    byte state = game::state::Get();
    if (state >= GAME_STATE_PLAY && state < GAME_STATE_END) {
      // We only care about the play state.
      game::state::play::Handler();
    } else {
      // We reset the map whenever we get to any state that is not the play
      // one. A new map will be automatically downloaded when we are back to
      // the play state in the game.
      game::map::Reset();
      game::map::download::Reset();
      game::state::play::Reset();  // Just in case.
    }

    game::state::Set(state);
  } else {
    if (button_double_clicked) {
      blink::state::StartLevelSelection();
    } else if (button_clicked) {
      if (blink::state::GetLevelSelection()) {
        blink::state::NextAILevel();
        blink::state::StartLevelSelection();
      } else {
        // Switch to next available player.
        byte next_player = game::player::GetNext(blink::state::GetPlayer());

        // Make sure we filter out the empty player.
        blink::state::SetPlayer(next_player == 0 ? 1 : next_player);
      }
    }
  }

  blink::state::Render();

  hasWoken();

  blink::state::face::handler::ProcessBottom();
}

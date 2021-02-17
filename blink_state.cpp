#include "blink_state.h"

#include "game_map_download.h"
#include "game_state.h"
#include "game_state_generic_render.h"
#include "game_state_no_map_render.h"
#include "game_state_play_render.h"

#define BLINK_STATE_MAX_AI_LEVEL 4  // First level is 0.
#define BLINK_STATE_LEVEL_SELECTION_TIMEOUT 2000
namespace blink {

struct State {
  byte player;
  byte ai_level;
  Timer level_selection_timer_;
};
static State state_;

namespace state {

void SetPlayer(byte player) { state_.player = player; }

byte GetPlayer() { return state_.player; }

void Render() {
  if (GetLevelSelection()) {
    setColor(MAKECOLOR_5BIT_RGB(8, 8, 8));
    for (byte face = 0; face <= GetAILevel(); ++face) {
      setColorOnFace(game::player::GetColor(GetPlayer()), face);
    }

    return;
  }

  if (!game::map::download::Downloaded()) {
    game::state::no_map::Render();

    return;
  }

  byte state = game::state::Get();
  if (state > GAME_STATE_SETUP_VALIDATE && state < GAME_STATE_END) {
    // We only care about the play state.
    game::state::play::Render();
  } else {
    game::state::generic::Render();
  }
}

void NextAILevel() {
  state_.ai_level =
      (state_.ai_level == BLINK_STATE_MAX_AI_LEVEL) ? 0 : state_.ai_level + 1;
}

byte GetAILevel() { return state_.ai_level; }

void StartLevelSelection() {
  state_.level_selection_timer_.set(BLINK_STATE_LEVEL_SELECTION_TIMEOUT);
}

bool GetLevelSelection() { return !state_.level_selection_timer_.isExpired(); }

void Reset() {
  state_.player = 0;
  state_.ai_level = 0;
}

}  // namespace state

}  // namespace blink

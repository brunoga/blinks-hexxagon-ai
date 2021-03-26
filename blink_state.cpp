#include "blink_state.h"

#include "game_map_download.h"
#include "game_state.h"
#include "game_state_generic_render.h"
#include "game_state_no_map_render.h"
#include "game_state_play_render.h"

#define BLINK_STATE_MAX_AI_LEVEL 2  // First level is 0.
#define BLINK_STATE_LEVEL_SELECTION_TIMEOUT 2000

#define BLINK_STATE_COLOR_OVERRIDE_TIMEOUT 200

namespace blink {

struct State {
  byte player;
  byte ai_level;
  Timer level_selection_timer_;
};
static State state_;

namespace state {

static Timer color_override_timer_;
static bool in_level_selection_;

void SetPlayer(byte player) { state_.player = player; }

byte __attribute__((noinline)) GetPlayer() { return state_.player; }

void StartColorOverride() {
  color_override_timer_.set(BLINK_STATE_COLOR_OVERRIDE_TIMEOUT);
}

bool GetColorOverride() { return !color_override_timer_.isExpired(); }

void Render() {
  if (GetColorOverride()) {
    setColor(WHITE);
    return;
  }

  if (GetLevelSelection()) {
    setColor(OFF);
    for (byte face = 0; face <= GetAILevel(); ++face) {
      setColorOnFace(game::player::GetColor(GetPlayer()), face * 2);
    }

    return;
  }

  if (!game::map::download::Downloaded()) {
    game::state::no_map::Render();

    return;
  }

  byte state = game::state::Get();
  if (state >= GAME_STATE_PLAY && state < GAME_STATE_END) {
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
  if (!in_level_selection_) StartColorOverride();

  in_level_selection_ = true;
  state_.level_selection_timer_.set(BLINK_STATE_LEVEL_SELECTION_TIMEOUT);
}

bool GetLevelSelection() {
  bool expired = state_.level_selection_timer_.isExpired();
  if (expired && in_level_selection_) {
    in_level_selection_ = false;
    StartColorOverride();
  }

  return !expired;
}

void Reset() {
  state_.player = 0;
  state_.ai_level = 0;
}

}  // namespace state

}  // namespace blink

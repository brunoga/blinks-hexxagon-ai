#include "blink_state.h"

#include "game_map.h"
#include "game_state.h"
#include "game_state_generic_render.h"
#include "game_state_no_map_render.h"
#include "game_state_play_render.h"

#define BLINK_STATE_MAX_AI_LEVEL 1  // First level is 0.

namespace blink {

struct State {
  byte player;
  byte map_requested_face;
  byte ai_level;
};
static State state_;

namespace state {

void SetPlayer(byte player) { state_.player = player; }

byte GetPlayer() { return state_.player; }

void SetMapRequestedFace(byte connected_face) {
  state_.map_requested_face = connected_face;
}

byte GetMapRequestedFace() { return state_.map_requested_face; }

void Render() {
  FaceValue face_value = {0, false, 0};

  FOREACH_FACE(face) {
    if (face == GetMapRequestedFace()) {
      face_value.map_requested = true;
    } else {
      face_value.map_requested = false;
    }

    setValueSentOnFace(face_value.as_byte, face);
  }

  if (!game::map::Downloaded()) {
    game::state::no_map::Render();

    return;
  }

  switch (game::state::GetSpecific()) {
    case GAME_STATE_PLAY:
      game::state::play::Render();
      break;
    default:
      game::state::generic::Render();
      break;
  }
}

void NextAILevel() {
  state_.ai_level =
      (state_.ai_level == BLINK_STATE_MAX_AI_LEVEL) ? 0 : state_.ai_level + 1;
}

byte GetAILevel() { return state_.ai_level; }

void Reset() {
  state_.player = 0;
  state_.map_requested_face = FACE_COUNT;
}

}  // namespace state

}  // namespace blink

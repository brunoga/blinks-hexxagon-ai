#include "blink_state.h"

#include "game_map.h"
#include "game_state_no_map_render.h"

namespace blink {

struct State {
  byte player;
  byte map_requested_face;
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
  } else {
    setColor(OFF);
  }

  // TODO(bga): Render based on game states.
}

void Reset() {
  state_.player = 0;
  state_.map_requested_face = FACE_COUNT;
}

}  // namespace state

}  // namespace blink

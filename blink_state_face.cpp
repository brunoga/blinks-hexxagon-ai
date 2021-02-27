#include "blink_state_face.h"

#include "game_state.h"
namespace blink {

namespace state {

namespace face {

namespace handler {

static Value previous_value_[FACE_COUNT];

static byte hexxagon_face_;

void __attribute__((noinline)) ProcessTop() {
  hexxagon_face_ = FACE_COUNT;

  FOREACH_FACE(face) {
    if (isValueReceivedOnFaceExpired(face)) {
      continue;
    }

    Value value = {.as_byte = getLastValueReceivedOnFace(face)};

    if (value.hexxagon && !value.ai) {
      hexxagon_face_ = face;
    }

    if (value.reset_state != previous_value_[face].reset_state &&
        value.reset_state) {
      game::state::Set(GAME_STATE_IDLE);
    }

    previous_value_[face] = value;
  }
}

void ProcessBottom() {
  FOREACH_FACE(face) {
    Value output_value = {/*map_requested=*/face == hexxagon_face_,
                          /*hexxagon=*/false,
                          /*color_override=*/false,
                          /*reset_state=*/false,
                          /*ai=*/true,
                          /*player=*/0};
    setValueSentOnFace(output_value.as_byte, face);
  }
}

byte HexxagonFace() { return hexxagon_face_; }

}  // namespace handler

}  // namespace face

}  // namespace state

}  // namespace blink

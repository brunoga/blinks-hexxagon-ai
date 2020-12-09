#include "blink_state.h"

namespace blink {

struct State {
  byte connected_face;
};
static State state_;

namespace state {

void SetConnectedFace(byte connected_face) {
  state_.connected_face = connected_face;
}

byte GetConnectedFace() { return state_.connected_face; }

void Render() {
  FaceValue face_value = {0, false, 0};

  FOREACH_FACE(face) {
    if (face == GetConnectedFace()) {
      face_value.map_requested = true;
    } else {
      face_value.map_requested = false;
    }

    setValueSentOnFace(face_value.as_byte, face);
  }
}

void Reset() { state_.connected_face = FACE_COUNT; }

}  // namespace state

}  // namespace blink

#include "blink_state_face.h"

namespace blink {

namespace state {

namespace face {

namespace handler {

static Value previous_value_[FACE_COUNT];

static byte hexxagon_face_;

void ProcessTop() {
  hexxagon_face_ = FACE_COUNT;

  FOREACH_FACE(face) {
    if (isValueReceivedOnFaceExpired(face)) {
      continue;
    }

    Value value = {.as_byte = getLastValueReceivedOnFace(face)};

    if (value.hexxagon && !value.ai) {
      hexxagon_face_ = face;
    }

    previous_value_[face] = value;
  }
}

void ProcessBottom() {
  Value output_value = {/*unused=*/0,
                        /*hexxagon=*/false,
                        /*color_override=*/false,
                        /*reset_state=*/false,
                        /*ai=*/true,
                        /*player=*/0};
  setValueSentOnAllFaces(output_value.as_byte);
}

byte HexxagonFace() { return hexxagon_face_; }

}  // namespace handler

}  // namespace face

}  // namespace state

}  // namespace blink

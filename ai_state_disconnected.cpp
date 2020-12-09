#include "ai_state_disconnected.h"

#include <blinklib.h>

#include "ai_state.h"
#include "blink_state.h"

namespace ai {

namespace state {

namespace disconnected {

void Handler() {
  FOREACH_FACE(face) {
    if (isValueReceivedOnFaceExpired(face)) continue;

    blink::state::SetConnectedFace(face);
    ai::state::Set(AI_STATE_CONNECTED);

    break;
  }
}

}  // namespace disconnected

}  // namespace state

}  // namespace ai
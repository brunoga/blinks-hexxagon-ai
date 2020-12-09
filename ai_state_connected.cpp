#include "ai_state_connected.h"

#include <blinklib.h>

#include "ai_state.h"
#include "blink_state.h"

namespace ai {

namespace state {

namespace connected {

void Handler() {
  if (isDatagramReadyOnFace(blink::state::GetConnectedFace())) {
    // There is a datagram ready, so it looks our neighbor is sending us the
    // map. Download it.
    ai::state::Set(AI_STATE_DOWNLOADING_MAP);
  }
}

}  // namespace connected

}  // namespace state

}  // namespace ai

#include <blinklib.h>

#include "ai_state.h"
#include "ai_state_connected.h"
#include "ai_state_disconnected.h"
#include "ai_state_downloading_map.h"
#include "blink_state.h"
#include "src/blinks-broadcast/manager.h"

void setup() {}

void loop() {
  broadcast::manager::Process();

  switch (ai::state::Get()) {
    case AI_STATE_DISCONNECTED:
      ai::state::disconnected::Handler();
      break;
    default: {
      byte connected_face = blink::state::GetConnectedFace();

      if (connected_face == FACE_COUNT ||
          isValueReceivedOnFaceExpired(connected_face)) {
        // Looks like we got disconnected. Reset.
        ai::state::Reset();
        blink::state::Reset();

        return;
      }

      switch (ai::state::Get()) {
        case AI_STATE_CONNECTED:
          ai::state::connected::Handler();
          break;
        case AI_STATE_DOWNLOADING_MAP:
          ai::state::downloading_map::Handler();
          break;
        case AI_STATE_READY:
          break;
      }
      break;
    }
  }

  blink::state::Render();
}

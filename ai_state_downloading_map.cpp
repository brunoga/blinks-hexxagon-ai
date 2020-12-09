#include "ai_state_downloading_map.h"

#include "ai_state.h"

namespace ai {

namespace state {

namespace downloading_map {

void Handler() {
  switch (ai::state::GetSpecific()) {
    case AI_STATE_DOWNLOADING_MAP_RECEIVE_SIZE:
      break;
    case AI_STATE_DOWNLOADING_MAP_DOWNLOAD:
      break;
  }
}

}  // namespace downloading_map

}  // namespace state

}  // namespace ai

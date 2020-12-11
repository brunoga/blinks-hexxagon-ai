#include "game_state_no_map_render.h"

#include "blink_state.h"
#include "game_player.h"

namespace game {

namespace state {

namespace no_map {

void Render() {
  if (blink::state::GetMapRequestedFace() != FACE_COUNT) {
    setColor(game::player::GetColor(blink::state::GetPlayer()));
  } else {
    setColor(dim(game::player::GetColor(blink::state::GetPlayer()), 127));
  }
}

}  // namespace no_map

}  // namespace state

}  // namespace game

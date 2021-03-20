#include "game_state_no_map_render.h"

#include "blink_state.h"
#include "blink_state_face.h"
#include "game_player.h"
#include "render_animation.h"

namespace game {

namespace state {

namespace no_map {

void Render() {
  // Render 1 spinning face when not connected to a Blink and 2 spinning faces
  // when connected.
  render::animation::Spinner(
      OFF, 125,
      blink::state::face::handler::HexxagonFace() != FACE_COUNT ? 1 : 2);
}

}  // namespace no_map

}  // namespace state

}  // namespace game

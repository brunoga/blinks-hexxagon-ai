#include "game_state_no_map_render.h"

#include "blink_state.h"
#include "blink_state_face.h"
#include "game_player.h"
#include "render_animation.h"

namespace game {

namespace state {

namespace no_map {

void Render() {
  byte faces = 1 + (blink::state::face::handler::HexxagonFace() != FACE_COUNT);

  // Render 1 spinning face when not connected to a Blink and 2 spinning faces
  // when connected.
  render::animation::Spinner(game::player::GetColor(blink::state::GetPlayer()),
                             MAKECOLOR_5BIT_RGB(8, 8, 8), faces, 100);
}

}  // namespace no_map

}  // namespace state

}  // namespace game

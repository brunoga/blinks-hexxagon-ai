#include "game_state_no_map_render.h"

#include "blink_state.h"
#include "game_player.h"
#include "render_animation.h"

namespace game {

namespace state {

namespace no_map {

void Render() {
  byte faces = 1;

  if (blink::state::GetMapRequestedFace() == FACE_COUNT) {
    faces = 2;
  }

  // Render 1 spinning face when not connected to a Blink and 2 spinning faces
  // when connected.
  render::animation::Spinner(game::player::GetColor(blink::state::GetPlayer()),
                             MAKECOLOR_5BIT_RGB(8, 8, 8), faces, 100);
}

}  // namespace no_map

}  // namespace state

}  // namespace game

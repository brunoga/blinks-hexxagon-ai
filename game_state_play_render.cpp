#include "game_state_play_render.h"

#include "blink_state.h"
#include "game_player.h"
#include "render_animation.h"

namespace game {

namespace state {

namespace play {

void Render() {
  // Fast spinning 3 faces.
  render::animation::Spinner(game::player::GetColor(blink::state::GetPlayer()),
                             MAKECOLOR_5BIT_RGB(15, 15, 15), 3, 100);
}

}  // namespace play

}  // namespace state

}  // namespace game

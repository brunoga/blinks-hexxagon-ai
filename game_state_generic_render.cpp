#include "game_state_generic_render.h"

#include "blink_state.h"
#include "game_player.h"
#include "render_animation.h"

namespace game {

namespace state {

namespace generic {

void Render() {
  // Slow spinning 3 faces.
  render::animation::Spinner(OFF, 100, 1);
}

}  // namespace generic

}  // namespace state

}  // namespace game

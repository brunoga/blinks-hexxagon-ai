#include "game_state_play_render.h"

#include "blink_state.h"
#include "game_player.h"
#include "game_state.h"
#include "render_animation.h"

namespace game {

namespace state {

namespace play {

void Render() {
  byte slowdown = 100;
  Color color = OFF;
  if (game::state::GetPlayer() == blink::state::GetPlayer()) {
    slowdown = 50;
    color = WHITE;
  }

  render::animation::Spinner(color, slowdown, 1);
}

}  // namespace play

}  // namespace state

}  // namespace game

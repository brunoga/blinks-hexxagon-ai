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
  if (game::state::GetPlayer() == blink::state::GetPlayer()) {
    slowdown = 50;
  }

  render::animation::Spinner(game::player::GetColor(blink::state::GetPlayer()),
                             MAKECOLOR_5BIT_RGB(15, 15, 15), 3, slowdown);
}

}  // namespace play

}  // namespace state

}  // namespace game

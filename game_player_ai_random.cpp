#include "game_player_ai_random.h"

#include "blink_state.h"
#include "game_player_ai.h"

namespace game {

namespace player {

namespace ai {

namespace random {

bool GetMove(position::Coordinates* origin, position::Coordinates* target) {
  return game::player::ai::GetRandomMove(blink::state::GetPlayer(), origin,
                                         target);
}

}  // namespace random

}  // namespace ai

}  // namespace player

}  // namespace game
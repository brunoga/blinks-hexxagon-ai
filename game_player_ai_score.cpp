#include "game_player_ai_score.h"

#include "blink_state.h"
#include "game_map.h"
#include "game_player_ai.h"

namespace game {

namespace player {

namespace ai {

namespace score {

bool GetMove(position::Coordinates* origin, position::Coordinates* target) {
  position::Coordinates possible_origin;
  position::Coordinates possible_target;
  int16_t selected_score = -32768;

  while (game::map::GetNextPossibleMove(blink::state::GetPlayer(), false,
                                        &possible_origin, &possible_target)) {
    int16_t score = ComputeMoveScore(blink::state::GetPlayer(), false,
                                     possible_origin, possible_target);

    if (score > selected_score) {
      *origin = possible_origin;
      *target = possible_target;

      selected_score = score;
    }
  }

  return true;
}

}  // namespace score

}  // namespace ai

}  // namespace player

}  // namespace game

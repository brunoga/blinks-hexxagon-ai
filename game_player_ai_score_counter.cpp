#include "game_player_ai_score_counter.h"

#include "blink_state.h"
#include "game_map.h"
#include "game_player_ai.h"

namespace game {

namespace player {

namespace ai {

namespace score {

namespace counter {

bool GetMove(position::Coordinates* origin, position::Coordinates* target) {
  position::Coordinates possible_origin;
  position::Coordinates possible_target;
  int16_t selected_score = -1000;

  while (game::map::GetNextPossibleMove(blink::state::GetPlayer(), false,
                                        &possible_origin, &possible_target)) {
    // Compute score for current possible move.
    int16_t player_score = ComputeMoveScore(blink::state::GetPlayer(), false,
                                            possible_origin, possible_target);

    if (player_score < selected_score) {
      // Score is too low. No need to do anything else.
      continue;
    }

    // Now set origin and target positions to the selected ones.
    game::map::SetMoveOrigin(possible_origin.x, possible_origin.y);
    game::map::SetMoveTarget(possible_target.x, possible_target.y);

    // And commit this move to the scratch.
    game::map::CommitMove(true);

    // Now compute the highest counter score for all remaining players.
    int16_t highest_counter_score = -1000;
    for (byte i = 1; i < GAME_PLAYER_MAX_PLAYERS; i++) {
      if (i != blink::state::GetPlayer()) {
        position::Coordinates counter_possible_origin;
        position::Coordinates counter_possible_target;
        while (game::map::GetNextPossibleMove(i, true, &counter_possible_origin,
                                              &counter_possible_target)) {
          int16_t counter_score = ComputeMoveScore(
              i, true, counter_possible_origin, counter_possible_target);
          if (counter_score > highest_counter_score) {
            highest_counter_score = counter_score;
          }
        }
      }
    }

    game::map::CommitMove(true);

    // Compute the final score for this move
    int16_t score = player_score - highest_counter_score;

    if (score > selected_score) {
      *origin = possible_origin;
      *target = possible_target;

      selected_score = score;
    }
  }

  return true;
}

}  // namespace counter

}  // namespace score

}  // namespace ai

}  // namespace player

}  // namespace game

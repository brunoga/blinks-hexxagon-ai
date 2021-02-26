#include "game_player_ai_score_counter.h"

#include "blink_state.h"
#include "game_map.h"
#include "game_player_ai.h"

namespace game {

namespace player {

namespace ai {

namespace score {

namespace counter {

static position::Coordinates possible_origin_;
static position::Coordinates possible_target_;
static int16_t selected_score_ = -1000;

static word origin_iterator_;
static word target_iterator_;

bool GetMove(position::Coordinates* origin, position::Coordinates* target) {
  int16_t score = -1000;
  if (GetNextScoredPossibleMove(blink::state::GetPlayer(), false,
                                &possible_origin_, &possible_target_, &score,
                                &origin_iterator_, &target_iterator_)) {
    if (score < selected_score_) {
      // Score is too low.Try next possible move.
      return false;
    }

    // Now set origin and target positions to the current ones.
    game::map::SetMoveOrigin(possible_origin_.x, possible_origin_.y);
    game::map::SetMoveTarget(possible_target_.x, possible_target_.y);

    // And commit this move to the scratch.
    game::map::CommitMove(true);

    // Now compute the highest counter score for all remaining players.
    position::Coordinates counter_possible_origin;
    position::Coordinates counter_possible_target;
    int16_t counter_selected_score = -1000;

    for (byte counter_player = 1; counter_player < GAME_PLAYER_MAX_PLAYERS;
         counter_player++) {
      if (counter_player == blink::state::GetPlayer()) continue;

      int16_t counter_score = -1000;
      word counter_origin_iterator = 0;
      word counter_target_iterator = 0;
      while (GetNextScoredPossibleMove(
          counter_player, true, &counter_possible_origin,
          &counter_possible_target, &counter_score, &counter_origin_iterator,
          &counter_target_iterator)) {
        if (counter_score > counter_selected_score) {
          counter_selected_score = counter_score;
        }
      }
    }

    // Compute the final score for this move
    int16_t move_score = score - counter_selected_score;

    if (move_score > selected_score_) {
      *origin = possible_origin_;
      *target = possible_target_;

      selected_score_ = move_score;
    }

    return false;
  }

  // TODO(bga): If we reached here, there are no more possible moves and this
  // return below is not enough to indicate that. Maybe use an extra parameter
  // to indicate end of moves so callers stop trying calling us?
  bool result = (selected_score_ != -1000);

  selected_score_ = -1000;

  return result;
}

}  // namespace counter

}  // namespace score

}  // namespace ai

}  // namespace player

}  // namespace game

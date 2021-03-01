#include "game_player_ai_score_counter.h"

#include "blink_state.h"
#include "game_map.h"
#include "game_player_ai.h"

namespace game {

namespace player {

namespace ai {

namespace score {

namespace counter {

static word move_origin_iterator_;
static word move_target_iterator_;
static position::Coordinates move_origin_;
static position::Coordinates move_target_;
static int16_t move_score_ = -1000;

static word counter_move_origin_iterator_;
static word counter_move_target_iterator_;
static byte counter_player_ = 0;

static int16_t move_selected_score_ = -1000;

bool GetMove(position::Coordinates* origin, position::Coordinates* target) {
  if (move_score_ == -1000) {
    // Move score has default value so we are not currently processing any move.
    // Obtain next move and commit it to scratch.
    if (GetNextScoredPossibleMove(
            blink::state::GetPlayer(), false, &move_origin_, &move_target_,
            &move_score_, &move_origin_iterator_, &move_target_iterator_)) {
      // Now set origin and target positions to given ones.
      game::map::SetMoveOrigin(move_origin_.x, move_origin_.y);
      game::map::SetMoveTarget(move_target_.x, move_target_.y);

      // And commit this move to the scratch.
      game::map::CommitMove(true);

      return false;
    } else {
      // TODO(bga): If we reached here, there are no more possible moves and
      // this return below is not enough to indicate that. Maybe use an extra
      // parameter to indicate end of moves so callers stop trying calling us?
      bool result = (move_selected_score_ != -1000);

      move_selected_score_ = -1000;

      return result;
    }
  }

  // We have a move score. Process counter moves.

  // Start with player one and increment from there.
  counter_player_++;

  if (counter_player_ < GAME_PLAYER_MAX_PLAYERS) {
    // Valid player.

    if (counter_player_ == blink::state::GetPlayer()) {
      // But this is ourselves, so skip.
      return false;
    }

    // Process all possible moves for this player.
    static position::Coordinates counter_move_origin;
    static position::Coordinates counter_move_target;
    int16_t counter_move_score = -1000;
    int16_t counter_move_selected_score = -1000;
    while (GetNextScoredPossibleMove(
        counter_player_, true, &counter_move_origin, &counter_move_target,
        &counter_move_score, &counter_move_origin_iterator_,
        &counter_move_target_iterator_)) {
      if (counter_move_score > counter_move_selected_score) {
        counter_move_selected_score = counter_move_score;
      }
    }

    // Compute the final score for this move
    int16_t final_score = move_score_ - counter_move_selected_score;

    if (final_score > move_selected_score_) {
      *origin = move_origin_;
      *target = move_target_;

      move_selected_score_ = final_score;
    }
  } else {
    counter_player_ = 0;
    move_score_ = -1000;
  }

  return false;
}

}  // namespace counter

}  // namespace score

}  // namespace ai

}  // namespace player

}  // namespace game

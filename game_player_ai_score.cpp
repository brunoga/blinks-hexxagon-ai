#include "game_player_ai_score.h"

#include "blink_state.h"
#include "game_map.h"
#include "game_player_ai.h"

namespace game {

namespace player {

namespace ai {

namespace score {

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
    if (score > selected_score_) {
      *origin = possible_origin_;
      *target = possible_target_;

      selected_score_ = score;
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

}  // namespace score

}  // namespace ai

}  // namespace player

}  // namespace game

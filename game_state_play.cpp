#include "game_state_play.h"

#include "blink_state.h"
#include "game_map.h"
#include "game_message.h"
#include "game_player_ai_conditional_random.h"
#include "game_player_ai_random.h"
#include "game_player_ai_score.h"
#include "game_state.h"

namespace game {

namespace state {

namespace play {

static position::Coordinates origin_;
static position::Coordinates target_;

static bool got_move_;
static bool origin_done_;
static bool target_done_;

void Handler() {
  bool current_player = (game::state::GetPlayer() == blink::state::GetPlayer());

  switch (game::state::Get()) {
    case GAME_STATE_PLAY_SELECT_ORIGIN:
      if (game::state::Changed()) {
        got_move_ = false;
        origin_done_ = false;
        target_done_ = false;
      }

      if (!current_player || origin_done_) {
        return;
      }

      if (!got_move_) {
        game::map::ResetPossibleMoveIterators();

        switch (blink::state::GetAILevel()) {
          case 0:
            got_move_ = game::player::ai::random::GetMove(&origin_, &target_);
            break;
          case 1:
            got_move_ = game::player::ai::conditional_random::GetMove(&origin_,
                                                                      &target_);
            break;
          case 2:
            got_move_ =
                random(9) > 6
                    ? game::player::ai::score::GetMove(&origin_, &target_)
                    : game::player::ai::conditional_random::GetMove(&origin_,
                                                                    &target_);
            break;
          case 3:
            got_move_ =
                random(9) < 3
                    ? game::player::ai::score::GetMove(&origin_, &target_)
                    : game::player::ai::conditional_random::GetMove(&origin_,
                                                                    &target_);

            break;
          case 4:
            got_move_ = game::player::ai::score::GetMove(&origin_, &target_);
            break;
        }

        if (!got_move_) return;
      }

      if (game::message::SendSelectOrigin(origin_.x, origin_.y)) {
        game::map::SetMoveOrigin(origin_.x, origin_.y);
        origin_done_ = true;
      }

      break;
    case GAME_STATE_PLAY_SELECT_TARGET:
      if (!current_player || !got_move_ || target_done_) {
        return;
      }

      if (game::message::SendSelectTarget(target_.x, target_.y)) {
        game::map::SetMoveTarget(target_.x, target_.y);
        target_done_ = true;
      }

      break;
    case GAME_STATE_PLAY_RESOLVE_MOVE:
      game::map::CommitMove();
      break;
  }
}

}  // namespace play

}  // namespace state

}  // namespace game

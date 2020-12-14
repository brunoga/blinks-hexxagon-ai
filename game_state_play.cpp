#include "game_state_play.h"

#include "blink_state.h"
#include "game_map.h"
#include "game_message.h"
#include "game_state.h"

namespace game {

namespace state {

namespace play {

static bool done_origin_;
static bool done_target_;

void Handler() {
  bool current_player = (game::state::GetPlayer() == blink::state::GetPlayer());

  switch (game::state::GetSpecific()) {
    case GAME_STATE_PLAY_SELECT_ORIGIN: {
      if (done_origin_ || !current_player) return;

      // This is just for testing. We always look for the first viable origin.
      game::map::ResetOriginIterator();

      const game::map::Data* data = game::map::GetNextViableOrigin();
      if (data != nullptr) {
        if (game::message::SendSelectOrigin(data->x, data->y)) {
          game::map::SetMoveOrigin(data->x, data->y);
          done_origin_ = true;
        }
      }

      break;
    }
    case GAME_STATE_PLAY_SELECT_TARGET: {
      if (done_target_ || !current_player) return;

      // This is just for testing. We always look for the first viable target.
      game::map::ResetTargetIterator();

      const game::map::Data* data =
          game::map::GetNextViableTarget(game::map::GetMoveOrigin());
      if (data != nullptr) {
        if (game::message::SendSelectTarget(data->x, data->y)) {
          game::map::SetMoveTarget(data->x, data->y);
          done_target_ = true;
        }
      }

      break;
    }
    case GAME_STATE_PLAY_RESOLVE_MOVE:
      game::map::CommitMove();
      Reset();
      break;
    default:
      break;
  }
}

void Reset() {
  done_origin_ = false;
  done_target_ = false;
}

}  // namespace play

}  // namespace state

}  // namespace game

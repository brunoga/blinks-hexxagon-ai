#include "game_player_ai.h"

#include "game_map.h"

namespace game {

namespace player {

namespace ai {

bool GetRandomMove(byte player, position::Coordinates* origin,
                   position::Coordinates* target) {
  // Count number of possible moves.
  word num_moves = 0;
  while (game::map::GetNextPossibleMove(player, origin, target)) {
    num_moves++;
  }

  if (num_moves == 0) return false;

  // Pick one.
  word move_index = ::random(num_moves - 1);

  // Search for it and return when found.
  word current_index = 0;
  while (game::map::GetNextPossibleMove(player, origin, target)) {
    if (current_index == move_index) {
      return true;
    }

    current_index++;
  }

  // We should never reach this, but just in case.
  return false;
}

}  // namespace ai

}  // namespace player

}  // namespace game

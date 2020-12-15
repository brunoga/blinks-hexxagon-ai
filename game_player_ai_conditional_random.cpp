#include "game_player_ai_conditional_random.h"

#include "blink_state.h"
#include "game_map.h"
#include "game_player_ai.h"

namespace game {

namespace player {

namespace ai {

namespace conditional_random {

bool GetMove(position::Coordinates* origin, position::Coordinates* target) {
  while (game::player::ai::GetRandomMove(blink::state::GetPlayer(), origin,
                                         target)) {
    // We have a move. Check if it increases our number of pieces.
    if (position::coordinates::Distance(*origin, *target) == 1) {
      // It does. No need to check anything else.
      return true;
    }

    // Ok, so this is a jump move. Check if we will capture at least one enemy
    // piece.
    byte total_neighbors;
    byte player_neighbors;
    byte enemy_neighbors;
    game::map::CountNeighbors(blink::state::GetPlayer(), *target,
                              &total_neighbors, &player_neighbors,
                              &enemy_neighbors);
    if (enemy_neighbors > 0) {
      // Target position has at least 1 enemy neighbor.
      return true;
    }
  }

  // If we got to this point, we did not find any move that satisfies our
  // requirements. Return a random move.
  return game::player::ai::GetRandomMove(blink::state::GetPlayer(), origin,
                                         target);
}

}  // namespace conditional_random

}  // namespace ai

}  // namespace player

}  // namespace game
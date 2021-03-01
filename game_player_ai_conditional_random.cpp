#include "game_player_ai_conditional_random.h"

#include "blink_state.h"
#include "game_map.h"

#define GAME_PLAYER_AI_CONDITIONAL_RANDOM_MAX_ATTEMPTS 5

namespace game {

namespace player {

namespace ai {

namespace conditional_random {

static word origin_iterator_;
static word target_iterator_;

static word num_possible_moves_;

static bool get_random_move(byte player, position::Coordinates* origin,
                            position::Coordinates* target) {
  if (num_possible_moves_ == 0) {
    // Count number of possible moves.
    while (game::map::GetNextPossibleMove(
        player, false, origin, target, &origin_iterator_, &target_iterator_)) {
      num_possible_moves_++;
    }

    if (num_possible_moves_ == 0) {
      return false;
    }
  }

  // Pick one.
  word move_index = random(num_possible_moves_ - 1);

  // Search for it and return when found.
  word current_index = 0;
  while (game::map::GetNextPossibleMove(player, false, origin, target,
                                        &origin_iterator_, &target_iterator_)) {
    if (current_index == move_index) {
      break;
    }

    current_index++;
  }

  origin_iterator_ = 0;
  target_iterator_ = 0;

  return true;
}

bool GetMove(position::Coordinates* origin, position::Coordinates* target) {
  for (byte i = 0; i < GAME_PLAYER_AI_CONDITIONAL_RANDOM_MAX_ATTEMPTS; i++) {
    if (!get_random_move(blink::state::GetPlayer(), origin, target)) {
      return false;
    }

    // We have a move. Check if it increases our number of pieces.
    if (position::coordinates::Distance(*origin, *target) == 1) {
      // It does. No need to check anything else.
      num_possible_moves_ = 0;
      return true;
    }

    // Ok, so this is a jump move. Check if we will capture at least one
    // enemy piece.
    byte total_neighbors;
    byte player_neighbors;
    byte enemy_neighbors;
    bool kill_move;
    game::map::CountNeighbors(blink::state::GetPlayer(), false, *target,
                              &total_neighbors, &player_neighbors,
                              &enemy_neighbors, &kill_move);
    if (enemy_neighbors > 0) {
      // Target position has at least 1 enemy neighbor.
      num_possible_moves_ = 0;
      return true;
    }
  }

  // If we got to this point, we did not find any move that satisfies our
  // requirements. Return a random move.
  bool result = get_random_move(blink::state::GetPlayer(), origin, target);

  num_possible_moves_ = 0;

  return result;
}

}  // namespace conditional_random

}  // namespace ai

}  // namespace player

}  // namespace game
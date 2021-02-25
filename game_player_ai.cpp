#include "game_player_ai.h"

#include "game_map.h"

// Use integer coeficients to avoid floating point operations.
#define GAME_PLAYER_AI_SCORE_BORDER_SCORE 1
#define GAME_PLAYER_AI_SCORE_NO_JUMP_SCORE \
  20  // Jump costs 1 captured position.
#define GAME_PLAYER_AI_SCORE_JOIN_MULTIPLIER 4
#define GAME_PLAYER_AI_SCORE_LEAVE_MULTIPLIER 4
#define GAME_PLAYER_AI_SCORE_STANDARD_MULTIPLIER 20

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

int16_t ComputeMoveScore(byte player, const ::position::Coordinates& origin,
                         const ::position::Coordinates& target) {
  byte origin_total_neighbors;
  byte origin_player_neighbors;
  byte origin_enemy_neighbors;
  game::map::CountNeighbors(player, origin, &origin_total_neighbors,
                            &origin_player_neighbors, &origin_enemy_neighbors);

  byte target_total_neighbors;
  byte target_player_neighbors;
  byte target_enemy_neighbors;
  game::map::CountNeighbors(player, target, &target_total_neighbors,
                            &target_player_neighbors, &target_enemy_neighbors);

  // Are we moving to a border position?
  bool target_is_border = (target_total_neighbors < FACE_COUNT);

  // Are we duplicating or moving (jumping)?
  bool target_is_jump = (position::coordinates::Distance(origin, target) > 1);

  // This is the main component of our score. The higher the number of enemies
  // we capture, the better.
  int16_t target_enemies_score =
      target_enemy_neighbors * GAME_PLAYER_AI_SCORE_STANDARD_MULTIPLIER;

  // We prefer clusters, so moving near other positions we control is also
  // good.
  int16_t target_player_score =
      target_player_neighbors * GAME_PLAYER_AI_SCORE_JOIN_MULTIPLIER;

  // Score for keeping the origin position. Currently we get a fixed positive
  // score if we decide to not jump or a negative score based on the number
  // of player neighboors we are leaving behind otherwise.
  int16_t origin_player_score =
      (target_is_jump
           ? -(origin_player_neighbors * GAME_PLAYER_AI_SCORE_LEAVE_MULTIPLIER)
           : GAME_PLAYER_AI_SCORE_NO_JUMP_SCORE);

  // Borders are better than no borders, so we give a small score boost if we
  // duplicate/move to a border.
  int16_t target_border_score =
      (target_is_border ? GAME_PLAYER_AI_SCORE_BORDER_SCORE : 0);

  // Final score is just a sum of the scores above. Lots of room for tweaking
  // here.
  return target_enemies_score + target_player_score + origin_player_score +
         target_border_score;
}

}  // namespace ai

}  // namespace player

}  // namespace game

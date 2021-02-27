#include "game_player_ai.h"

#include "game_map.h"

// Use integer coeficients to avoid floating point operations.
#define GAME_PLAYER_AI_SCORE_BORDER_SCORE 1
#define GAME_PLAYER_AI_SCORE_NO_JUMP_SCORE \
  10  // Jump costs 1/2 captured position.
#define GAME_PLAYER_AI_SCORE_JOIN_MULTIPLIER 4
#define GAME_PLAYER_AI_SCORE_LEAVE_MULTIPLIER 4
#define GAME_PLAYER_AI_SCORE_STANDARD_MULTIPLIER 20

namespace game {

namespace player {

namespace ai {

static int16_t compute_move_score(byte player, bool use_scratch,
                                  const ::position::Coordinates& origin,
                                  const ::position::Coordinates& target) {
  byte origin_total_neighbors;
  byte origin_player_neighbors;
  byte origin_enemy_neighbors;
  game::map::CountNeighbors(player, use_scratch, origin,
                            &origin_total_neighbors, &origin_player_neighbors,
                            &origin_enemy_neighbors);

  byte target_total_neighbors;
  byte target_player_neighbors;
  byte target_enemy_neighbors;
  game::map::CountNeighbors(player, use_scratch, target,
                            &target_total_neighbors, &target_player_neighbors,
                            &target_enemy_neighbors);

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
  // of player neighbors we are leaving behind otherwise.
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

bool __attribute__((noinline))
GetNextScoredPossibleMove(byte player, bool use_scratch,
                          position::Coordinates* origin,
                          position::Coordinates* target, int16_t* score,
                          word* origin_iterator, word* target_iterator) {
  if (game::map::GetNextPossibleMove(player, use_scratch, origin, target,
                                     origin_iterator, target_iterator)) {
    *score = compute_move_score(player, use_scratch, *origin, *target);

    return true;
  }

  return false;
}

}  // namespace ai

}  // namespace player

}  // namespace game

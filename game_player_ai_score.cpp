#include "game_player_ai_score.h"

#include "blink_state.h"
#include "game_map.h"

// Use integer coeficients to avoid floating point operations.
#define GAME_PLAYER_AI_SCORE_BORDER_SCORE 1
#define GAME_PLAYER_AI_SCORE_JOIN_MULTIPLIER 4
#define GAME_PLAYER_AI_SCORE_LEAVE_MULTIPLIER 4
#define GAME_PLAYER_AI_SCORE_STANDARD_MULTIPLIER 20

namespace game {

namespace player {

namespace ai {

namespace score {

bool GetMove(position::Coordinates* origin, position::Coordinates* target) {
  position::Coordinates selected_origin;
  position::Coordinates selected_target;
  int16_t selected_score = -32768;

  while (game::map::GetNextPossibleMove(blink::state::GetPlayer(), origin,
                                        target)) {
    byte origin_total_neighbors;
    byte origin_player_neighbors;
    byte origin_enemy_neighbors;
    game::map::CountNeighbors(blink::state::GetPlayer(), *origin,
                              &origin_total_neighbors, &origin_player_neighbors,
                              &origin_enemy_neighbors);

    byte target_total_neighbors;
    byte target_player_neighbors;
    byte target_enemy_neighbors;
    game::map::CountNeighbors(blink::state::GetPlayer(), *target,
                              &target_total_neighbors, &target_player_neighbors,
                              &target_enemy_neighbors);

    bool target_is_border = (target_total_neighbors < FACE_COUNT);
    bool target_is_jump =
        (position::coordinates::Distance(*origin, *target) > 1);

    int16_t target_enemies_score =
        target_enemy_neighbors * GAME_PLAYER_AI_SCORE_STANDARD_MULTIPLIER;
    int16_t target_player_score =
        target_player_neighbors * GAME_PLAYER_AI_SCORE_JOIN_MULTIPLIER;
    int16_t origin_player_score =
        (target_is_jump ? -(origin_player_neighbors *
                            GAME_PLAYER_AI_SCORE_LEAVE_MULTIPLIER)
                        : GAME_PLAYER_AI_SCORE_STANDARD_MULTIPLIER);
    int16_t target_border_score =
        (target_is_border ? GAME_PLAYER_AI_SCORE_BORDER_SCORE : 0);

    int16_t score = target_enemies_score + target_player_score +
                    origin_player_score + target_border_score;

    if (score > selected_score) {
      selected_origin = *origin;
      selected_target = *target;
      selected_score = score;
    }
  }

  *origin = selected_origin;
  *target = selected_target;

  return true;
}

}  // namespace score

}  // namespace ai

}  // namespace player

}  // namespace game

#include "game_player_ai_score.h"

#include "blink_state.h"
#include "game_map.h"

// Use integer coeficients to avoid floating point operations.
#define GAME_PLAYER_AI_SCORE_BORDER_SCORE 1
#define GAME_PLAYER_AI_SCORE_NO_JUMP_SCORE \
  10  // Half of the standard multiplier below so that we will still prefer to
      // capture more pieces when there are only 2 enemy pieces at the target
      // position. This might be overfitting so double check. It does fixes some
      // pathological cases.
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

    // Are we moving to a border position?
    bool target_is_border = (target_total_neighbors < FACE_COUNT);

    // Are we duplicating or moving (jumping)?
    bool target_is_jump =
        (position::coordinates::Distance(*origin, *target) > 1);

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
        (target_is_jump ? -(origin_player_neighbors *
                            GAME_PLAYER_AI_SCORE_LEAVE_MULTIPLIER)
                        : GAME_PLAYER_AI_SCORE_NO_JUMP_SCORE);

    // Borders are better than no borders, so we give a small score boost if we
    // duplicate/move to a border.
    int16_t target_border_score =
        (target_is_border ? GAME_PLAYER_AI_SCORE_BORDER_SCORE : 0);

    // Final score is just a sum of the scores above. Lots of room for tweaking
    // here.
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

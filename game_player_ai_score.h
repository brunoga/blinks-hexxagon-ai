#ifndef GAME_PLAYER_AI_SCORE_H_
#define GAME_PLAYER_AI_SCORE_H_

#include "src/blinks-position/coordinates.h"

namespace game {

namespace player {

namespace ai {

namespace score {

bool GetMove(position::Coordinates* origin, position::Coordinates* target);

}  // namespace score

}  // namespace ai

}  // namespace player

}  // namespace game

#endif
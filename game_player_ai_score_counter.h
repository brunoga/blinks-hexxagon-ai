#ifndef GAME_PLAYER_AI_SCORE_COUNTER_H_
#define GAME_PLAYER_AI_SCORE_COUNTER_H_

#include "src/blinks-position/coordinates.h"

namespace game {

namespace player {

namespace ai {

namespace score {

namespace counter {

bool GetMove(position::Coordinates* origin, position::Coordinates* target);

}  // namespace counter

}  // namespace score

}  // namespace ai

}  // namespace player

}  // namespace game

#endif
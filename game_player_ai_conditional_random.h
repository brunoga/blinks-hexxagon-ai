#ifndef GAME_PLAYER_AI_CONDITIONAL_RANDOM_H_
#define GAME_PLAYER_AI_CONDITIONAL_RANDOM_H_

#include "src/blinks-position/coordinates.h"

namespace game {

namespace player {

namespace ai {

namespace conditional_random {

bool GetMove(position::Coordinates* origin, position::Coordinates* target);

}  // namespace conditional_random

}  // namespace ai

}  // namespace player

}  // namespace game

#endif
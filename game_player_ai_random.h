#ifndef GAME_PLAYER_AI_RANDOM_H_
#define GAME_PLAYER_AI_RANDOM_H_

#include "src/blinks-position/coordinates.h"

namespace game {

namespace player {

namespace ai {

namespace random {

bool GetMove(position::Coordinates* origin, position::Coordinates* target);

}  // namespace random

}  // namespace ai

}  // namespace player

}  // namespace game

#endif
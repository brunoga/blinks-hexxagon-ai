#ifndef GAME_PLAYER_AI_H_
#define GAME_PLAYER_AI_H_

#include "src/blinks-position/coordinates.h"

namespace game {

namespace player {

namespace ai {

int16_t ComputeMoveScore(byte player, bool use_scratch,
                         const ::position::Coordinates& origin,
                         const ::position::Coordinates& target);

}  // namespace ai

}  // namespace player

}  // namespace game

#endif
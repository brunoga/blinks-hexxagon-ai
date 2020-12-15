#ifndef GAME_PLAYER_AI_H_
#define GAME_PLAYER_AI_H_

#include "src/blinks-position/coordinates.h"

namespace game {

namespace player {

namespace ai {

bool GetRandomMove(byte player, position::Coordinates* origin,
                   position::Coordinates* target);

}  // namespace ai

}  // namespace player

}  // namespace game

#endif
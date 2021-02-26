#ifndef GAME_PLAYER_AI_H_
#define GAME_PLAYER_AI_H_

#include "src/blinks-position/coordinates.h"

namespace game {

namespace player {

namespace ai {

bool GetNextScoredPossibleMove(byte player, bool use_scratch,
                               position::Coordinates* origin,
                               position::Coordinates* target, int16_t* score,
                               word* origin_iterator, word* target_iterator);

}  // namespace ai

}  // namespace player

}  // namespace game

#endif
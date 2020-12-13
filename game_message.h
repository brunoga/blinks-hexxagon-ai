#ifndef GAME_MESSAGE_H_
#define GAME_MESSAGE_H_

#include <blinklib.h>

namespace game {

namespace message {

void Process();

bool SendSelectOrigin(int8_t x, int8_t y);
bool SendSelectTarget(int8_t x, int8_t y);

}  // namespace message

}  // namespace game

#endif
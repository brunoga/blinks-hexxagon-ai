#ifndef BLINK_STATE_H_
#define BLINK_STATE_H_

#include <blinklib.h>

namespace blink {

namespace state {

void SetPlayer(byte player);
byte GetPlayer();

void NextAILevel();
byte GetAILevel();

void StartLevelSelection();
bool GetLevelSelection();

void Render();

void Reset();

}  // namespace state

}  // namespace blink

#endif
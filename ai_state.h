#ifndef AI_STATE_H_
#define AI_STATE_H_

#include <blinklib.h>

#define AI_STATE_DISCONNECTED 0
#define AI_STATE_CONNECTED 1
#define AI_STATE_DOWNLOADING_MAP 2
#define AI_STATE_READY 3

namespace ai {

namespace state {

void Set(byte state);
byte Get();

void SetSpecific(byte state);
byte GetSpecific();

void Reset();

}  // namespace state

}  // namespace ai

#endif
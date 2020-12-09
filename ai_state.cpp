#include "ai_state.h"

namespace ai {

struct State {
  byte state;
  byte specific_state;
};
static State state_;

namespace state {

void Set(byte state) { state_.state = state; }

byte Get() { return state_.state; }

void SetSpecific(byte specific_state) {
  state_.specific_state = specific_state;
}

byte GetSpecific() { return state_.specific_state; }

void Reset() {
  state_.state = AI_STATE_DISCONNECTED;
  state_.specific_state = 0;
}

}  // namespace state

}  // namespace ai


#include "game_state.h"

#include "game_map.h"
#include "game_player.h"

namespace game {

struct State {
  byte current;
  byte previous;
  byte player;
  bool from_network;
};
static State state_;

namespace state {

void Set(byte state, bool from_network) {
  state_.previous = state_.current;
  state_.current = state;

  state_.from_network = from_network;
}

byte __attribute__((noinline)) Get() { return state_.current; }

void SetPlayer(byte player) { state_.player = player; }

byte __attribute__((noinline)) GetPlayer() { return state_.player; }

void Reset() {
  state_.current = GAME_STATE_IDLE;
  state_.previous = GAME_STATE_IDLE;
  state_.player = 0;
  state_.from_network = false;
}

bool Changed() { return state_.current != state_.previous; }

bool FromNetwork() { return state_.from_network; }

}  // namespace state

}  // namespace game

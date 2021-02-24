#include "game_player.h"

#define MAPLE MAKECOLOR_5BIT_RGB(31, 6, 0)     // red/orange
#define SPRUCE CYAN                            // cyan
#define CHERRY MAKECOLOR_5BIT_RGB(30, 12, 18)  // pink/lavender
#define ELM MAKECOLOR_5BIT_RGB(25, 25, 0)      // yellow/orange

namespace game {

namespace player {

static const Color colors_[] = {GREEN, MAPLE, SPRUCE, CHERRY, ELM};

byte GetNext(byte current_player) {
  byte next_player = current_player + 1;

  return next_player == GAME_PLAYER_MAX_PLAYERS ? GAME_PLAYER_NO_PLAYER
                                                : next_player;
}

Color __attribute__((noinline)) GetColor(byte player) {
  return colors_[player];
}

}  // namespace player

}  // namespace game

#ifndef GAME_MAP_H_
#define GAME_MAP_H_

#include <blinklib.h>

#include "game_player.h"
#include "src/blinks-position/position.h"

#define GAME_MAP_MAX_BLINKS 128

namespace game {

namespace map {

struct Data {
  int16_t x : 6;
  int16_t y : 6;
  uint16_t player : 4;
};

Data* Get();
byte GetSize();
void SetSize(byte size);

void SetMoveOrigin(int8_t x, int8_t y);
position::Coordinates GetMoveOrigin();

void SetMoveTarget(int8_t x, int8_t y);
position::Coordinates GetMoveTarget();

// Computes the player move from origin to destination, updating the game
// map accordingly.
void CommitMove(bool use_scratch);

// Sets all map data to invalid values.
void Reset();

// AI-specific functions.

bool GetNextPossibleMove(byte player, bool use_scratch,
                         position::Coordinates* origin,
                         position::Coordinates* target, word* origin_iterator,
                         word* target_iterator);

void CountNeighbors(byte player, bool use_scratch,
                    position::Coordinates coordinates, byte* total_neighbors,
                    byte* player_neighbors, byte* enemy_neighbors);

}  // namespace map

}  // namespace game

#endif
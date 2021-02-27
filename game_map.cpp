#include "game_map.h"

#include <string.h>

#include "blink_state.h"
#include "game_state.h"

#define GAME_MAP_MAX_MODIFIED_POSITIONS_PER_PLAY 8

namespace game {

static game::map::Data map_[GAME_MAP_MAX_BLINKS];

namespace map {

struct Scratch {
  byte map_index;
  byte player;
};

static byte index_;

static Scratch scratch_[GAME_MAP_MAX_MODIFIED_POSITIONS_PER_PLAY];
static byte scratch_index_;

static bool move_commited_;

struct MoveData {
  position::Coordinates origin;
  position::Coordinates target;
};
static MoveData move_data_;

static byte get_player_at_index(byte map_index, bool use_scratch) {
  if (use_scratch) {
    for (byte i = 0; i < scratch_index_; i++) {
      if (scratch_[i].map_index == map_index) {
        // Found in scratch, return it.
        return scratch_[i].player;
      }
    }
  }

  return map_[map_index].player;
}

static void __attribute__((noinline))
update_blinks(position::Coordinates coordinates, byte player,
              bool update_neighbors, bool use_scratch) {
  for (byte i = 0; i < index_; ++i) {
    if (((map_[i].x == coordinates.x) && (map_[i].y == coordinates.y)) ||
        (update_neighbors && (get_player_at_index(i, use_scratch) != 0) &&
         (position::coordinates::Distance(
              coordinates, {(int8_t)map_[i].x, (int8_t)map_[i].y}) == 1))) {
      // This is either the position we are updating or we also want to update
      // neighboors and this is a non-empty neighbor (we just needed to update
      // neighbors of different players, but that would be an extra check and
      // use more storage due to that. Updating a player to itself is harmless).
      if (use_scratch) {
        scratch_[scratch_index_] = {i, player};
        scratch_index_++;
      } else {
        map_[i].player = player;
      }
    }
  }
}

Data* Get() { return map_; }

byte GetSize() { return index_; }

void SetSize(byte size) { index_ = size; }

static void set_data(int8_t x, int8_t y, position::Coordinates* coordinates) {
  coordinates->x = x;
  coordinates->y = y;

  move_commited_ = false;
}

void SetMoveOrigin(int8_t x, int8_t y) { set_data(x, y, &move_data_.origin); }

position::Coordinates GetMoveOrigin() { return move_data_.origin; }

void SetMoveTarget(int8_t x, int8_t y) { set_data(x, y, &move_data_.target); }

position::Coordinates GetMoveTarget() { return move_data_.target; }

void __attribute__((noinline)) CommitMove(bool use_scratch) {
  scratch_index_ = 0;

  if (move_commited_) return;

  if (position::coordinates::Distance(move_data_.origin, move_data_.target) >
      1) {
    update_blinks(move_data_.origin, 0, false, use_scratch);
  }

  update_blinks(move_data_.target, game::state::GetPlayer(), true, use_scratch);

  move_commited_ = true;
}

void Reset() {
  index_ = 0;
  scratch_index_ = 0;
}

bool GetNextPossibleMove(byte player, bool use_scratch,
                         position::Coordinates* origin,
                         position::Coordinates* target, word* origin_iterator,
                         word* target_iterator) {
  while (*origin_iterator < index_) {
    if (get_player_at_index(*origin_iterator, use_scratch) == player) {
      while (*target_iterator < index_) {
        if (get_player_at_index(*target_iterator, use_scratch) ==
            GAME_PLAYER_NO_PLAYER) {
          byte distance = position::coordinates::Distance(
              {(int8_t)map_[*origin_iterator].x,
               (int8_t)map_[*origin_iterator].y},
              {(int8_t)map_[*target_iterator].x,
               (int8_t)map_[*target_iterator].y});

          if (distance <= 2) {
            origin->x = map_[*origin_iterator].x;
            origin->y = map_[*origin_iterator].y;
            target->x = map_[*target_iterator].x;
            target->y = map_[*target_iterator].y;

            (*target_iterator)++;

            return true;
          }
        }

        (*target_iterator)++;
      }

      *target_iterator = 0;
    }

    (*origin_iterator)++;
  }

  *origin_iterator = 0;

  return false;
}

void CountNeighbors(byte player, bool use_scratch,
                    position::Coordinates coordinates, byte* total_neighbors,
                    byte* player_neighbors, byte* enemy_neighbors) {
  *total_neighbors = 0;
  *player_neighbors = 0;
  *enemy_neighbors = 0;
  for (byte i = 0; i < index_; ++i) {
    byte map_player = get_player_at_index(i, use_scratch);
    if (position::coordinates::Distance(
            coordinates, {(int8_t)map_[i].x, (int8_t)map_[i].y}) == 1) {
      (*total_neighbors)++;
      if (map_player != 0 && map_player != player) {
        (*enemy_neighbors)++;
      } else if (map_player == player) {
        (*player_neighbors)++;
      }
    }
  }
}

}  // namespace map

}  // namespace game

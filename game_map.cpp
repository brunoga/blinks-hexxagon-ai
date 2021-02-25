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

Statistics stats_;

static bool move_commited_;

struct MoveData {
  position::Coordinates origin;
  position::Coordinates target;
};
static MoveData move_data_;

static byte origin_iterator_[GAME_PLAYER_MAX_PLAYERS];
static byte target_iterator_[GAME_PLAYER_MAX_PLAYERS];

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

static bool can_move(const Data& data, bool use_scratch) {
  for (byte i = 0; i < index_; ++i) {
    if ((get_player_at_index(i, use_scratch) == 0) &&
        (position::coordinates::Distance(
             {(int8_t)data.x, (int8_t)data.y},
             {(int8_t)map_[i].x, (int8_t)map_[i].y}) <= 2)) {
      return true;
    }
  }

  return false;
}

void ComputeMapStats() {
  memset(&stats_, 0, sizeof(Statistics));

  for (byte i = 0; i < index_; ++i) {
    const Data& map_data = map_[i];
    // Update number of players.
    if (map_data.player != 0 &&
        stats_.player[map_data.player].blink_count == 0) {
      stats_.player_count++;
    }

    // Update player blink count.
    stats_.player[map_data.player].blink_count++;

    // Update player can move.
    if (can_move(map_data, false)) {
      stats_.player[map_data.player].can_move = true;
    }

    // Update empty space in range.
    if (!(stats_.local_blink_empty_space_in_range)) {
      stats_.local_blink_empty_space_in_range =
          ((map_data.player == 0) &&
           (position::Distance(position::Coordinates{
                (int8_t)map_data.x, (int8_t)map_data.y}) <= 2));
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

  if (!use_scratch) ComputeMapStats();

  move_commited_ = true;
}

const Statistics& GetStatistics() { return stats_; }

bool __attribute__((noinline)) ValidState() {
  return (stats_.player[0].blink_count > 0) && (stats_.player_count > 1);
}

void Reset() {
  index_ = 0;
  scratch_index_ = 0;

  ComputeMapStats();
}

bool GetNextPossibleMove(byte player, bool use_scratch,
                         position::Coordinates* origin,
                         position::Coordinates* target) {
  for (; origin_iterator_[player] < index_; origin_iterator_[player]++) {
    if (get_player_at_index(origin_iterator_[player], use_scratch) != player) {
      continue;
    }

    if (!can_move(map_[origin_iterator_[player]], use_scratch)) continue;

    for (; target_iterator_[player] < index_; target_iterator_[player]++) {
      if (get_player_at_index(target_iterator_[player], use_scratch) != 0)
        continue;

      if (position::coordinates::Distance(
              {(int8_t)map_[origin_iterator_[player]].x,
               (int8_t)map_[origin_iterator_[player]].y},
              {(int8_t)map_[target_iterator_[player]].x,
               (int8_t)map_[target_iterator_[player]].y}) > 2) {
        continue;
      }

      origin->x = map_[origin_iterator_[player]].x;
      origin->y = map_[origin_iterator_[player]].y;
      target->x = map_[target_iterator_[player]].x;
      target->y = map_[target_iterator_[player]].y;

      // Increase iterator otherwise on the next iteration we would be at the
      // same point, which would lead us getting here again and so on, resulting
      // in an possible infinite loop.
      target_iterator_[player]++;

      return true;
    }

    target_iterator_[player] = 0;
  }

  origin_iterator_[player] = 0;

  return false;
}

void CountNeighbors(byte player, position::Coordinates coordinates,
                    bool use_scratch, byte* total_neighbors,
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

void ResetPossibleMoveIterators() {
  memset(&target_iterator_, 0, GAME_PLAYER_MAX_PLAYERS);
  memset(&origin_iterator_, 0, GAME_PLAYER_MAX_PLAYERS);
}

}  // namespace map

}  // namespace game

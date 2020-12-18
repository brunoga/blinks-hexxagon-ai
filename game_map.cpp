#include "game_map.h"

#include <string.h>

#include "blink_state.h"
#include "game_state.h"

#define GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA 0
#define GAME_MAP_DOWNLOAD_STATE_DOWNLOAD 1

#define GAME_MAP_DOWNLOAD_METADATA_SIZE 3

#define MESSAGE_MAP_DOWNLOAD 6  // Only used for starting the map download.

// 2 byte entries (4 bytes total). Chosen to be smaller than the maximum size of
// datagram we use otherwise.
#define GAME_MAP_DOWNLOAD_MAX_CHUNK_SIZE 2

namespace game {

static game::map::Data map_[GAME_MAP_MAX_BLINKS];

namespace map {

static byte index_;
static byte download_index_;
static byte download_state_;

Statistics stats_;

static bool move_commited_;

struct MoveData {
  position::Coordinates origin;
  position::Coordinates target;
};
static MoveData move_data_;

static byte origin_iterator_;
static byte target_iterator_;

static void __attribute__((noinline))
update_blinks(position::Coordinates coordinates, byte player,
              bool update_neighbors) {
  for (byte i = 0; i < index_; ++i) {
    if (((map_[i].x == coordinates.x) && (map_[i].y == coordinates.y)) ||
        (update_neighbors && (map_[i].player != 0) &&
         (position::coordinates::Distance(
              coordinates, {(int8_t)map_[i].x, (int8_t)map_[i].y}) == 1))) {
      // This is either the position we are updating or we also want to update
      // neighboors and this is a non-empty neighbor (we just needed to update
      // neighbors of different players, but that would be an extra check and
      // use more storage due to that. Updating a player to itself is harmless).
      map_[i].player = player;
    }
  }
}

static void update_map_requested_face() {
  byte map_requested_face = blink::state::GetMapRequestedFace();

  // Check if we are currently connected.
  if (map_requested_face != FACE_COUNT) {
    // We are, did we get disconnected?
    if (isValueReceivedOnFaceExpired(map_requested_face)) {
      // Yep. Reset everything.
      Reset();
      blink::state::SetMapRequestedFace(FACE_COUNT);
    }

    return;
  }

  // We need a map. Search for a face to request it from.
  FOREACH_FACE(face) {
    if (!isValueReceivedOnFaceExpired(face)) {
      // Found a face to hopefuly request a map from.
      blink::state::SetMapRequestedFace(face);

      return;
    }
  }
}

bool can_move(const Data& data) {
  for (byte i = 0; i < index_; ++i) {
    if ((map_[i].player == 0) &&
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
    for (byte j = 0; j < index_; ++j) {
      if ((map_[j].player == 0) &&
          (position::coordinates::Distance(
               {(int8_t)map_data.x, (int8_t)map_data.y},
               {(int8_t)map_[j].x, (int8_t)map_[j].y}) <= 2)) {
        stats_.player[map_data.player].can_move = true;
        break;
      }
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

static void set_data(int8_t x, int8_t y, position::Coordinates* coordinates) {
  coordinates->x = x;
  coordinates->y = y;

  move_commited_ = false;
}

void SetMoveOrigin(int8_t x, int8_t y) { set_data(x, y, &move_data_.origin); }

position::Coordinates GetMoveOrigin() { return move_data_.origin; }

void SetMoveTarget(int8_t x, int8_t y) { set_data(x, y, &move_data_.target); }

position::Coordinates GetMoveTarget() { return move_data_.target; }

void __attribute__((noinline)) CommitMove() {
  if (move_commited_) return;

  if (position::coordinates::Distance(move_data_.origin, move_data_.target) >
      1) {
    update_blinks(move_data_.origin, 0, false);
  }

  update_blinks(move_data_.target, game::state::GetPlayer(), true);

  ComputeMapStats();

  move_commited_ = true;
}

const Statistics& GetStatistics() { return stats_; }

bool __attribute__((noinline)) ValidState() {
  return (stats_.player[0].blink_count > 0) && (stats_.player_count > 1);
}

bool MaybeDownload() {
  update_map_requested_face();

  byte face = blink::state::GetMapRequestedFace();

  if (face == FACE_COUNT || Downloaded()) return false;

  byte len = getDatagramLengthOnFace(face);

  if (len > 0) {
    switch (download_state_) {
      case GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA:
        if (len == GAME_MAP_DOWNLOAD_METADATA_SIZE &&
            getDatagramOnFace(face)[0] == MESSAGE_MAP_DOWNLOAD) {
          index_ = getDatagramOnFace(face)[1];

          // TODO(bga): Add a function to do this as it will also be done in
          // game message.
          game::state::Data data;
          data.as_byte = getDatagramOnFace(face)[2];

          game::state::Set(data.state, true);
          game::state::SetSpecific(data.specific_state, true);
          game::state::SetPlayer(data.next_player + 1);

          download_state_ = GAME_MAP_DOWNLOAD_STATE_DOWNLOAD;
        }

        break;
      case GAME_MAP_DOWNLOAD_STATE_DOWNLOAD:
        bool is_chunk_len = (len == (GAME_MAP_DOWNLOAD_MAX_CHUNK_SIZE * 2));
        bool is_last_chunk_len = (((len / 2) + download_index_) == index_);

        if (is_chunk_len || is_last_chunk_len) {
          memcpy(&map_[download_index_], getDatagramOnFace(face), len);
          download_index_ += (len / 2);
        }
        break;
    }

    markDatagramReadOnFace(face);
  }

  return true;
}

bool Downloaded() { return ((index_ > 0) && (download_index_ == index_)); }

void Reset() {
  index_ = 0;
  download_index_ = 0;
  download_state_ = GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA;

  ComputeMapStats();
}

bool GetNextPossibleMove(byte player, position::Coordinates* origin,
                         position::Coordinates* target) {
  for (; origin_iterator_ < index_; ++origin_iterator_) {
    if (map_[origin_iterator_].player != player) continue;

    if (!can_move(map_[origin_iterator_])) continue;

    for (; target_iterator_ < index_; ++target_iterator_) {
      if (map_[target_iterator_].player != 0) continue;

      if (position::coordinates::Distance({(int8_t)map_[origin_iterator_].x,
                                           (int8_t)map_[origin_iterator_].y},
                                          {(int8_t)map_[target_iterator_].x,
                                           (int8_t)map_[target_iterator_].y}) >
          2) {
        continue;
      }

      origin->x = map_[origin_iterator_].x;
      origin->y = map_[origin_iterator_].y;
      target->x = map_[target_iterator_].x;
      target->y = map_[target_iterator_].y;

      // Increase iterator otherwise on the next iteration we would be at the
      // same point, which would lead us getting here again and so on, resulting
      // in an possible infinite loop.
      target_iterator_++;

      return true;
    }

    target_iterator_ = 0;
  }

  origin_iterator_ = 0;

  return false;
}

void CountNeighbors(byte player, position::Coordinates coordinates,
                    byte* total_neighbors, byte* player_neighbors,
                    byte* enemy_neighbors) {
  *total_neighbors = 0;
  *player_neighbors = 0;
  *enemy_neighbors = 0;
  for (byte i = 0; i < index_; ++i) {
    if (position::coordinates::Distance(
            coordinates, {(int8_t)map_[i].x, (int8_t)map_[i].y}) == 1) {
      (*total_neighbors)++;
      if (map_[i].player != 0 && map_[i].player != player) {
        (*enemy_neighbors)++;
      } else if (map_[i].player == player) {
        (*player_neighbors)++;
      }
    }
  }
}

void ResetPossibleMoveIterators() {
  target_iterator_ = 0;
  origin_iterator_ = 0;
}

}  // namespace map

}  // namespace game

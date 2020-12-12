#include "game_map.h"

#include <string.h>

#include "blink_state.h"
#include "game_state.h"

#define GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA 0
#define GAME_MAP_DOWNLOAD_STATE_DOWNLOAD 1

#define GAME_MAP_DOWNLOAD_MAX_CHUNK_SIZE 5

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

static Data* find_entry_in_map(int8_t x, int8_t y) {
  for (byte i = 0; i < index_; ++i) {
    if (x == map_[i].x && y == map_[i].y) {
      return &map_[i];
    }
  }

  return nullptr;
}

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
      index_ = 0;
      download_index_ = 0;
      download_state_ = GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA;

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
        if (len == 2) {
          index_ = getDatagramOnFace(face)[0];

          // TODO(bga): Add a function to do this as it will also be done in
          // game message.
          game::state::Data data;
          data.as_byte = getDatagramOnFace(face)[1];

          game::state::Set(data.state, true);
          game::state::SetSpecific(data.specific_state, true);
          game::state::SetPlayer(data.next_player + 1);
        }

        download_state_ = GAME_MAP_DOWNLOAD_STATE_DOWNLOAD;
        break;
      case GAME_MAP_DOWNLOAD_STATE_DOWNLOAD:
        if (len <= GAME_MAP_DOWNLOAD_MAX_CHUNK_SIZE) {
          memcpy(&map_[download_index_], getDatagramOnFace(face), len);
          download_index_ += len;
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

}  // namespace map

}  // namespace game

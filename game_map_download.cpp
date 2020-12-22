#include "game_map_download.h"

#include <blinklib.h>
#include <string.h>

#include "blink_state.h"
#include "game_map.h"
#include "game_state.h"

#define GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA 0
#define GAME_MAP_DOWNLOAD_STATE_DOWNLOAD 1

#define GAME_MAP_DOWNLOAD_METADATA_SIZE 3

#define MESSAGE_MAP_DOWNLOAD 6  // Only used for starting the map download.

// 2 byte entries (4 bytes total). Chosen to be smaller than the maximum size of
// datagram we use otherwise.
#define GAME_MAP_DOWNLOAD_MAX_CHUNK_SIZE 2

namespace game {

namespace map {

namespace download {

static byte index_;
static byte state_;

static void update_map_requested_face() {
  byte map_requested_face = blink::state::GetMapRequestedFace();

  // Check if we are currently connected.
  if (map_requested_face != FACE_COUNT) {
    // We are, did we get disconnected?
    if (isValueReceivedOnFaceExpired(map_requested_face)) {
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

bool Process() {
  update_map_requested_face();

  byte face = blink::state::GetMapRequestedFace();

  if (face == FACE_COUNT || Downloaded()) return false;

  byte len = getDatagramLengthOnFace(face);

  if (len > 0) {
    switch (state_) {
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

          state_ = GAME_MAP_DOWNLOAD_STATE_DOWNLOAD;
        }

        break;
      case GAME_MAP_DOWNLOAD_STATE_DOWNLOAD:
        bool is_chunk_len = (len == (GAME_MAP_DOWNLOAD_MAX_CHUNK_SIZE * 2));
        bool is_last_chunk_len = (((len / 2) + index_) == index_);

        if (is_chunk_len || is_last_chunk_len) {
          memcpy(&(game::map::Get()[index_]), getDatagramOnFace(face), len);
          index_ += (len / 2);
        }
        break;
    }

    markDatagramReadOnFace(face);
  }

  return true;
}

bool Downloaded() {
  return ((game::map::GetSize() > 0) && (index_ == game::map::GetSize()));
}

void Reset() {
  state_ = GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA;
  index_ = 0;
}

}  // namespace download

}  // namespace map

}  // namespace game
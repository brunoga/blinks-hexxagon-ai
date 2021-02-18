#include "game_map_download.h"

#include <blinklib.h>
#include <string.h>

#include "blink_state_face.h"
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
static byte previous_hexxagon_face_ = FACE_COUNT;

bool Process() {
  byte current_hexxagon_face = blink::state::face::handler::HexxagonFace();
  if (current_hexxagon_face != previous_hexxagon_face_) {
    if (previous_hexxagon_face_ != FACE_COUNT) {
      resetPendingDatagramOnFace(previous_hexxagon_face_);
    }

    index_ = 0;
    state_ = GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA;
    previous_hexxagon_face_ = current_hexxagon_face;
  }

  if (current_hexxagon_face == FACE_COUNT || Downloaded()) {
    return false;
  }

  byte len = getDatagramLengthOnFace(current_hexxagon_face);

  if (len > 0) {
    switch (state_) {
      case GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA:
        if (len == GAME_MAP_DOWNLOAD_METADATA_SIZE &&
            getDatagramOnFace(current_hexxagon_face)[0] ==
                MESSAGE_MAP_DOWNLOAD) {
          game::map::SetSize(getDatagramOnFace(current_hexxagon_face)[1]);

          // TODO(bga): Add a function to do this as it will also be done in
          // game message.
          game::state::Data data = {
              .as_byte = getDatagramOnFace(current_hexxagon_face)[2]};

          game::state::Set(data.state, true);
          game::state::SetPlayer(data.next_player);

          state_ = GAME_MAP_DOWNLOAD_STATE_DOWNLOAD;
        }

        break;
      case GAME_MAP_DOWNLOAD_STATE_DOWNLOAD:
        bool is_chunk_len = (len == (GAME_MAP_DOWNLOAD_MAX_CHUNK_SIZE * 2));
        bool is_last_chunk_len = (((len / 2) + index_) == game::map::GetSize());

        if (is_chunk_len || is_last_chunk_len) {
          memcpy(&(game::map::Get()[index_]),
                 getDatagramOnFace(current_hexxagon_face), len);
          index_ += (len / 2);
        }
        break;
    }
  }

  // Mark datagram read on all faces as we are processing the map and need to
  // make sure we are not blocking messages on other faces.
  //
  // TODO(bga): We can improve this by handling the normal message processing
  // and the map download processing in the same loop.
  FOREACH_FACE(face) { markDatagramReadOnFace(face); }

  return true;
}

bool Downloaded() { return (index_ != 0) && (index_ == game::map::GetSize()); }

void Reset() {
  index_ = 0;
  state_ = GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA;
}

}  // namespace download

}  // namespace map

}  // namespace game
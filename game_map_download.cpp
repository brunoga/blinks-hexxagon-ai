#include "game_map_download.h"

#include <blinklib.h>
#include <string.h>

#include "blink_state_face.h"
#include "game_map.h"
#include "game_state.h"

#define GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA 0
#define GAME_MAP_DOWNLOAD_STATE_DOWNLOAD 1

#define GAME_MAP_DOWNLOAD_METADATA_SIZE 1

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

void Process() {
  byte current_hexxagon_face = blink::state::face::handler::HexxagonFace();
  if (current_hexxagon_face != previous_hexxagon_face_) {
    if (previous_hexxagon_face_ != FACE_COUNT) {
      resetPendingDatagramOnFace(previous_hexxagon_face_);

      game::state::Set(GAME_STATE_IDLE);
    }

    index_ = 0;
    state_ = GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA;
    previous_hexxagon_face_ = current_hexxagon_face;
  }

  if (current_hexxagon_face == FACE_COUNT || Downloaded()) {
    return;
  }

  byte len = getDatagramLengthOnFace(current_hexxagon_face);

  if (len > 0) {
    if (getDatagramOnFace(current_hexxagon_face)[0] != MESSAGE_MAP_DOWNLOAD) {
      return;
    }

    len--;
    const byte* datagram = &(getDatagramOnFace(current_hexxagon_face)[1]);

    markDatagramReadOnFace(current_hexxagon_face);

    switch (state_) {
      case GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA:
        if (len == GAME_MAP_DOWNLOAD_METADATA_SIZE) {
          game::map::SetSize(datagram[0]);

          state_ = GAME_MAP_DOWNLOAD_STATE_DOWNLOAD;
        }

        break;
      case GAME_MAP_DOWNLOAD_STATE_DOWNLOAD:
        bool is_chunk_len = (len == (GAME_MAP_DOWNLOAD_MAX_CHUNK_SIZE * 2));
        bool is_last_chunk_len = (((len / 2) + index_) == game::map::GetSize());

        if (is_chunk_len || is_last_chunk_len) {
          memcpy(&(game::map::Get()[index_]), datagram, len);
          index_ += (len / 2);
        }

        if (Downloaded()) {
          // Switch to generic play state which will prevent the map from
          // beign reset but will not do anything other than that, We should
          // receive a new game state change message soon and that will get
          // things moving.
          game::state::Set(GAME_STATE_PLAY);
        }

        break;
    }
  }
}

bool Downloaded() { return (index_ != 0) && (index_ == game::map::GetSize()); }

void Reset() {
  index_ = 0;
  state_ = GAME_MAP_DOWNLOAD_STATE_RECEIVE_METADATA;
}

}  // namespace download

}  // namespace map

}  // namespace game
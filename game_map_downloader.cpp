#include "game_map_downloader.h"

#define GAME_MAP_DOWNLOADER_WAIT_FOR_MAP_SIZE 0
#define GAME_MAP_DOWNLOADER_RECEIVE_MAP 1
#define GAME_MAP_DOWNLOADER_DONE 3

namespace game {

namespace map {

namespace downloader {

byte state_;

bool Run(byte face) {
  if (!isValueReceivedOnFaceExpired(face)) {
    // TODO(bga): What we actually want to know is if the face was connected and
    // got disconnected. If it did, then we need to reset the map and the
    // downloader state.
    return false;
  }

  switch (state_) {
    case GAME_MAP_DOWNLOADER_WAIT_FOR_MAP_SIZE:
      // TODO(bga): Try to read a single byte with the actual map size. This
      // will be used to know when it transferred successfuly and, possibly, to
      // show an animation related to the amount of data transferred).

      // TODO(bga): Maybe we also need to transfer other informnation here, like
      // the current game state. We will have to see how long a download takes.
      // We mighjt also need another state after data is transferred for that
      // (or just use the done state).
      break;
    case GAME_MAP_DOWNLOADER_RECEIVE_MAP:
      // TODO(bga): Read data in blocks of n bytes (n should not be too big, as
      // it interferes with loop timing, or too small, as it then will take
      // longer than needed to transfer everything.)
      break;
    case GAME_MAP_DOWNLOADER_DONE:
      // TODO(bga): We got all data, do whatever we need to do when that
      // happens.
  }
}

}  // namespace downloader

}  // namespace map

}  // namespace game
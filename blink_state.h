#ifndef BLINK_STATE_H_
#define BLINK_STATE_H_

#include <blinklib.h>

namespace blink {

namespace state {

union FaceValue {
  struct {
    byte unused : 4;         // Unused for now.
    bool map_requested : 1;  // Map data was requested on this face.
    byte player : 3;         // 0 means empty.
  };

  byte as_byte;
};

void SetPlayer(byte player);
byte GetPlayer();

void SetMapRequestedFace(byte map_requested_face);
byte GetMapRequestedFace();

void Render();

void Reset();

}  // namespace state

}  // namespace blink

#endif
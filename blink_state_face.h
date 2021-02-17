#ifndef BLINK_STATE_FACE_H_
#define BLINK_STATE_FACE_H_

#include <blinklib.h>

namespace blink {

namespace state {

namespace face {

union Value {
  struct {
    bool unused : 1;          // Unused for now.
    bool hexxagon : 1;        // Hexxagon connected to this face.
    bool color_override : 1;  // If true, start color override.
    bool reset_state : 1;     // If this bit changes, then we should reset.
    bool ai : 1;              // AI connected to this face.
    byte player : 3;          // 0 means empty.
  };

  byte as_byte;
};

namespace handler {

// Must be called at the top of loop().
void ProcessTop();

// Must be called at the bottom of the loop and must always be executed (careful
// with early returns).
void ProcessBottom();

// Returns the highest numbered face that has an Hexxagon Blink connected.
byte HexxagonFace();

}  // namespace handler

}  // namespace face

}  // namespace state

}  // namespace blink

#endif
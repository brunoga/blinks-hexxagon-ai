#include "render_animation.h"

namespace render {

namespace animation {

static Timer timer_;

void Spinner(const Color& foreground, const Color& background, byte num_faces,
             byte slowdown) {
  if (timer_.isExpired()) {
    timer_.set((FACE_COUNT * slowdown) - 1);
  }

  byte start_face = (FACE_COUNT - 1) - (timer_.getRemaining() / slowdown);

  byte step = FACE_COUNT / num_faces;

  FOREACH_FACE(face) {
    byte current_face = (start_face + face) % FACE_COUNT;
    if ((face % step) == 0) {
      setColorOnFace(foreground, current_face);
    } else {
      setColorOnFace(background, current_face);
    }
  }
}

void ResetTimer() { timer_.set(0); }

}  // namespace animation

}  // namespace render

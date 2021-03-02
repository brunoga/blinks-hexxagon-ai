#include "render_animation.h"

namespace render {

namespace animation {

void Spinner(const Color& foreground, const Color& background, byte num_faces,
             byte slowdown) {
  byte start_face = (millis() / slowdown) % FACE_COUNT;

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

}  // namespace animation

}  // namespace render

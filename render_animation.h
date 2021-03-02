#ifndef RENDER_ANIMATION_H_
#define RENDER_ANIMATION_H_

#include <blinklib.h>

namespace render {

namespace animation {

void Spinner(const Color& foreground, const Color& background, byte num_faces,
             byte slowdown);

}  // namespace animation

}  // namespace render

#endif
#include "render_animation.h"

#include "blink_state.h"
#include "game_player.h"

namespace render {

namespace animation {

static void player(Color background, byte start_face) {
  // It is actually cheaper to call setColorOnFace() multiple times below than
  // to use a variable and call it only once.
  FOREACH_FACE(face) {
    byte adjusted_face = (face + start_face) % FACE_COUNT;
    if (game::player::GetLitFace(blink::state::GetPlayer(), face)) {
      setColorOnFace(game::player::GetColor(blink::state::GetPlayer()),
                     adjusted_face);
    } else {
      setColorOnFace(background, adjusted_face);
    }
  }
}

void Spinner(Color background, byte slowdown, byte pause) {
  byte start_face = (millis() / slowdown) % (FACE_COUNT * pause);
  if (start_face >= FACE_COUNT) {
    start_face = FACE_COUNT - 1;
  }

  player(background, start_face);
}

}  // namespace animation

}  // namespace render

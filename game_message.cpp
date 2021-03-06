#include "game_message.h"

#include <string.h>

#include "blink_state_face.h"
#include "game_map.h"
#include "game_state.h"
#include "render_animation.h"
#include "src/blinks-broadcast/message.h"
#include "src/blinks-broadcast/message_tracker.h"

#define MESSAGE_GAME_STATE_CHANGE 1
#define MESSAGE_SELECT_ORIGIN 2
#define MESSAGE_SELECT_TARGET 3
namespace game {

namespace message {

static void game_state_change(const byte* payload) {
  game::state::Data data;
  data.as_byte = payload[0];

  game::state::Set(data.state, true);
  game::state::SetPlayer(data.next_player);
}

static void select_origin(const byte* payload) {
  game::map::SetMoveOrigin((int8_t)payload[0], (int8_t)payload[1]);
}

static void select_target(const byte* payload) {
  game::map::SetMoveTarget((int8_t)payload[0], (int8_t)payload[1]);
}

void Process() {
  FOREACH_FACE(face) {
    if (!isDatagramReadyOnFace(face)) continue;

    const broadcast::Message* message =
        (const broadcast::Message*)getDatagramOnFace(face);

    if (!broadcast::message::tracker::Tracked(message->header)) {
      broadcast::message::tracker::Track(message->header);

      switch (message->header.id) {
        case MESSAGE_GAME_STATE_CHANGE:
          game_state_change(message->payload);
          break;
        case MESSAGE_SELECT_ORIGIN:
          select_origin(message->payload);
          break;
        case MESSAGE_SELECT_TARGET:
          select_target(message->payload);
          break;
      }
    }

    markDatagramReadOnFace(face);
  }
}

static bool sendMessage(byte message_id, const byte* payload,
                        byte payload_size) {
  byte hexxagon_face = blink::state::face::handler::HexxagonFace();
  if (hexxagon_face == FACE_COUNT) return false;

  broadcast::Message message;
  message.header.id = message_id;
  message.header.sequence = broadcast::message::tracker::NextSequence();

  if (payload != nullptr) {
    memcpy(message.payload, payload, payload_size);
  }

  return sendDatagramOnFace(&message, payload_size + 1, hexxagon_face);
}

bool SendSelectOrigin(int8_t x, int8_t y) {
  byte payload[2] = {(byte)x, (byte)y};
  return sendMessage(MESSAGE_SELECT_ORIGIN, payload, 2);
}

bool SendSelectTarget(int8_t x, int8_t y) {
  byte payload[2] = {(byte)x, (byte)y};
  return sendMessage(MESSAGE_SELECT_TARGET, payload, 2);
}

}  // namespace message

}  // namespace game

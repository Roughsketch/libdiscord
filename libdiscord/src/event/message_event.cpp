#include "event/message_event.h"

namespace discord
{
  message_event::message_event(connection_state* owner, rapidjson::Value& data) : message(owner, data)
  {
  }
}

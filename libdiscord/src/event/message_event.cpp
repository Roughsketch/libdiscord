#include "event/message_event.h"

namespace discord
{
  MessageEvent::MessageEvent(ConnectionState* owner, rapidjson::Value& data) : Message(owner, data)
  {
  }
}

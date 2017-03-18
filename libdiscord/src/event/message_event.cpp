#include "event/message_event.h"

namespace discord
{
  message_event::message_event(connection_state* owner, rapidjson::Value& data) : connection_object(owner)
  {
    m_message = message(owner, data);
  }

  std::string discord::message_event::content() const
  {
    return m_message.content();
  }

  message message_event::respond(std::string content) const
  {
    return m_message.respond(content);
  }
}

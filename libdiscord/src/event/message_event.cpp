#include "event/message_event.h"

namespace discord
{
  message_event::message_event(const std::string& token, rapidjson::Value& data) : event(token)
  {
    m_message = message(token, data);
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

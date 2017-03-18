#pragma once

#include "event/general.h"
#include "message.h"

namespace discord
{
  class message_event : public connection_object
  {
    std::stringstream m_stream;
    message m_message;
  public:
    explicit message_event(connection_state* owner, rapidjson::Value& data);

    std::string content() const;
    message respond(std::string content) const;
  };

  class message_deleted_event : public connection_object
  {
  public:
    explicit message_deleted_event(connection_state* owner, rapidjson::Value& data) : connection_object(owner)
    {
    }
    explicit message_deleted_event(connection_state* owner, snowflake message_id, snowflake channel_id) : connection_object(owner)
    {
    }
  };

  class typing_event : public connection_object
  {
  public:
    explicit typing_event(connection_state* owner, rapidjson::Value& data) : connection_object(owner)
    {
    }
  };
}

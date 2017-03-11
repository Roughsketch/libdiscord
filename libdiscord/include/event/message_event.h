#pragma once

#include "event/general.h"
#include "message.h"

namespace discord
{
  class message_event : public bot_ownable
  {
    std::stringstream m_stream;
    message m_message;
  public:
    explicit message_event(const bot* owner, rapidjson::Value& data);

    std::string content() const;
    message respond(std::string content) const;
  };

  class message_deleted_event : public event
  {
  public:
    explicit message_deleted_event(const std::string& token, rapidjson::Value& data) : event(token)
    {
    }
    explicit message_deleted_event(const std::string& token, snowflake message_id, snowflake channel_id) : event(token)
    {
    }
  };

  class typing_event : public event
  {
  public:
    explicit typing_event(const std::string& token, rapidjson::Value& data) : event(token)
    {
    }
  };
}

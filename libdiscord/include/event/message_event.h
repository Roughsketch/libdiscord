#pragma once

#include "event/general.h"

namespace discord
{
  class message_event : public event
  {
  public:
    explicit message_event(std::string token, rapidjson::Value& data) : event(token)
    {
    }

    std::string content() const;
  };

  class message_deleted_event : public event
  {
  public:
    explicit message_deleted_event(std::string token, rapidjson::Value& data) : event(token)
    {
    }
    explicit message_deleted_event(std::string token, snowflake message_id, snowflake channel_id) : event(token)
    {
    }
  };

  class typing_event : public event
  {
  public:
    explicit typing_event(std::string token, rapidjson::Value& data) : event(token)
    {
    }
  };
}
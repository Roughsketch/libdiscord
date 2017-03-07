#pragma once

#include "event/general.h"

namespace discord
{
  class message_event : public event
  {
  public:
    explicit message_event(bot& owner, rapidjson::Value& data) : event(owner)
    {
    }

    std::string content() const;
  }

  class message_deleted_event : public event
  {
  public:
    explicit message_deleted_event(bot& owner, rapidjson::Value& data) : event(owner)
    {
    }
    explicit message_deleted_event(bot& owner, snowflake message_id, snowflake channel_id) : event(owner)
    {
    }
  }

  class typing_event : public event
  {
  public:
    explicit typing_event(bot& owner, rapidjson::Value& data) : event(owner)
    {
    }
  }
}
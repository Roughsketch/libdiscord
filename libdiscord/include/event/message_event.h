#pragma once

#include "event/general.h"
#include "message.h"

namespace discord
{
  class message_event : public message
  {
    std::stringstream m_stream;
  public:
    explicit message_event(connection_state* owner, rapidjson::Value& data);

    ~message_event() {
      auto str = m_stream.str();

      if (!str.empty())
      {
        respond(m_stream.str());
      }
    }

    template <typename U>
    message_event& operator<<(U& obj)
    {
      m_stream << obj;
      return *this;
    }
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

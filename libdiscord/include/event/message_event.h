#pragma once

#include "event/general.h"
#include "message.h"

namespace discord
{
  class MessageEvent : public Message
  {
    std::stringstream m_stream;
  public:
    explicit MessageEvent(ConnectionState* owner, rapidjson::Value& data);

    MessageEvent(const MessageEvent& other)
    {
      this->Message::operator=(other);
      m_stream << other.m_stream.str();
    }

    ~MessageEvent() {
      auto str = m_stream.str();

      if (!str.empty())
      {
        respond(m_stream.str());
      }
    }

    template <typename U>
    MessageEvent& operator<<(U& obj)
    {
      m_stream << obj;
      return *this;
    }
  };

  class MessageDeletedEvent : public ConnectionObject
  {
  public:
    explicit MessageDeletedEvent(ConnectionState* owner, rapidjson::Value& data) : ConnectionObject(owner)
    {
    }
    explicit MessageDeletedEvent(ConnectionState* owner, Snowflake message_id, Snowflake channel_id) : ConnectionObject(owner)
    {
    }
  };

  class TypingEvent : public ConnectionObject
  {
  public:
    explicit TypingEvent(ConnectionState* owner, rapidjson::Value& data) : ConnectionObject(owner)
    {
    }
  };
}

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "common.h"

namespace discord
{
  class channel;
  class emoji;
  class gateway;
  class guild;
  class user;

  class message_event;
  class message_deleted_event;
  class presence_event;
  class typing_event;

  class bot
  {
    std::unique_ptr<gateway> m_gateway;

    std::string m_token;
    std::unique_ptr<user> m_profile;
    std::unordered_map<uint64_t, guild> m_guilds;
    std::unordered_map<uint64_t, channel> m_private_channels;

    std::string m_prefix;

    //  Event callbacks
    std::function<void(message_event)> m_on_message;
    std::function<void(message_event)> m_on_message_edited;
    std::function<void(message_deleted_event)> m_on_message_deleted;
    std::function<void(emoji)> m_on_emoji_created;
    std::function<void(emoji)> m_on_emoji_deleted;
    std::function<void(emoji)> m_on_emoji_updated;
    std::function<void(presence_event)> m_on_presence;
    std::function<void(typing_event)> m_on_typing;
    std::unordered_map<std::string, std::function<void(message_event&)>> m_commands;

    void update_emojis(rapidjson::Value& data);
  public:
    explicit bot(std::string token, std::string prefix = "");

    void run(bool async = false) const;
    std::string token() const;
    void on_dispatch(std::string event_name, rapidjson::Value& data);
  };
}

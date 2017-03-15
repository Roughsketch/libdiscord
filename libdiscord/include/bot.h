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
  class presence;
  class user;

  class message_event;
  class message_deleted_event;
  class typing_event;

  class bot
  {
    std::unique_ptr<gateway> m_gateway;

    const std::string m_token;
    std::unique_ptr<user> m_profile;
    std::unordered_map<uint64_t, guild> m_guilds;
    std::unordered_map<uint64_t, channel> m_private_channels;

    std::string m_prefix;

    //  Event callbacks
    std::function<void()> m_on_ready;
    std::function<void(message_event&)> m_on_message;
    std::function<void(message_event&)> m_on_message_edited;
    std::function<void(message_deleted_event&)> m_on_message_deleted;
    std::function<void(emoji&)> m_on_emoji_created;
    std::function<void(emoji&)> m_on_emoji_deleted;
    std::function<void(emoji&)> m_on_emoji_updated;
    std::function<void(presence&)> m_on_presence;
    std::function<void(typing_event&)> m_on_typing;
    std::unordered_map<std::string, std::function<void(message_event&)>> m_commands;

    void update_emojis(rapidjson::Value& data);
  public:
    explicit bot(std::string token, std::string prefix = "");
    ~bot();

    const std::string& token() const;
    const user& profile() const;
    std::vector<guild> guilds() const;

    void on_ready(std::function<void()> callback);
    void on_message(std::function<void(message_event&)> callback);
    void on_message_edited(std::function<void(message_event&)> callback);
    void on_message_deleted(std::function<void(message_deleted_event&)> callback);
    void on_emoji_created(std::function<void(emoji&)> callback);
    void on_emoji_deleted(std::function<void(emoji&)> callback);
    void on_emoji_updated(std::function<void(emoji&)> callback);
    void on_presence(std::function<void(presence&)> callback);
    void on_typing(std::function<void(typing_event&)> callback);
    void add_command(std::string name, std::function<void(message_event&)> callback);

    void run(bool async = false) const;
    void on_dispatch(std::string event_name, rapidjson::Value& data);

  };
}

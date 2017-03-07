#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "common.h"

namespace discord
{
  class channel;
  class gateway;
  class guild;
  class user;

  class message_event;
  class message_deleted_event;
  class presence_event;
  class typing_event;

  struct bot_data
  {
    std::string token;
    std::unique_ptr<user> profile;
    std::unordered_map<uint64_t, guild> guilds;
    std::unordered_map<uint64_t, channel> private_channels;
    
    std::string prefix;

    //  Event callbacks
    std::function<void(message_event)> on_message;
    std::function<void(message_event)> on_message_edited;
    std::function<void(message_deleted_event)> on_message_deleted;
    std::function<void(presence_event)> on_presence;
    std::function<void(typing_event)> on_typing;
    std::unordered_map<std::string, std::function<void(message_event&)>> commands;
  };

  class bot
  {
    std::unique_ptr<gateway> m_gateway;
    bot_data m_data;
  public:
    explicit bot(std::string token, std::string prefix = "");

    void run(bool async = false) const;
    std::string token() const;
  };
}

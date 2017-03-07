#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "snowflake.h"

namespace discord
{
  class channel;
  class gateway;
  class guild;
  class user;

  class bot
  {
    std::string m_token;
    std::unique_ptr<gateway> m_gateway;

    snowflake m_client_id;
    std::unique_ptr<user> m_profile;

    std::unordered_map<snowflake, guild> m_guilds;
    std::unordered_map<snowflake, channel> m_private_channels;

    std::string m_prefix;

    explicit bot(std::string token, std::string prefix = "");
  public:
    static std::shared_ptr<bot> create(std::string token, std::string prefix = "");

    void run(bool async = false) const;
    std::string token() const;
  };
}

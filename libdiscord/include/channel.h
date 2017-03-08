#pragma once

#include "common.h"

namespace discord
{
  class bot;

  class channel : public identifiable
  {
    std::string m_token;
    snowflake m_guild_id;
  public:
    channel();
    explicit channel(std::string token, rapidjson::Value& data);

    channel& operator=(const channel& other);

    snowflake guild_id() const;
  };
}
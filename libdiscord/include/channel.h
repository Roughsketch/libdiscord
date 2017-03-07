#pragma once

#include "common.h"

namespace discord
{
  class bot;

  class channel : public identifiable
  {
    bot& m_owner;
    snowflake m_guild_id;
  public:
    explicit channel(bot& owner, rapidjson::Value& data);

    channel(const channel& other);

    snowflake guild_id() const;
  };
}
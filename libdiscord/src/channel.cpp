#include "channel.h"
#include "bot.h"

namespace discord
{
  channel::channel(bot& owner, rapidjson::Value& data) : m_owner(owner)
  {
  }

  channel::channel(const channel& other) : m_owner(other.m_owner)
  {
  }

  snowflake channel::guild_id() const
  {
    return m_guild_id;
  }
}

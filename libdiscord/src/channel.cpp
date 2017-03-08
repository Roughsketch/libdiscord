#include "channel.h"
#include "bot.h"

namespace discord
{
  channel::channel(std::string token, rapidjson::Value& data) : m_token(token)
  {
  }

  snowflake channel::guild_id() const
  {
    return m_guild_id;
  }
}

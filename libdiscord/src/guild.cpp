#include "guild.h"
#include "bot.h"

namespace discord
{
  guild::guild(bot& owner, rapidjson::Value& data) : m_owner(owner)
  {
  }

  guild::guild(const guild& other) : m_owner(other.m_owner)
  {
  }
}

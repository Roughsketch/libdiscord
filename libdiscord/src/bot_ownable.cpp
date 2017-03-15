#include "bot_ownable.h"
#include "channel.h"
#include "guild.h"
#include "user.h"

namespace discord
{
  bot_ownable::bot_ownable() : m_owner(nullptr)
  {
  }

  bot_ownable::bot_ownable(const bot* owner) : m_owner(owner)
  {
  }

  const bot* bot_ownable::owner() const
  {
    return m_owner;
  }

  const guild& bot_ownable::find_guild(snowflake id) const
  {
    return m_owner->find_guild(id);
  }
}

#include "connection_object.h"
#include "connection_state.h"
#include "guild.h"

namespace discord
{
  connection_object::connection_object() : m_owner(nullptr)
  {
  }

  connection_object::connection_object(connection_state* owner) : m_owner(owner)
  {
  }

  connection_state* connection_object::owner() const
  {
    return m_owner;
  }

  const guild& connection_object::find_guild(snowflake id) const
  {
    return m_owner->find_guild(id);
  }
}

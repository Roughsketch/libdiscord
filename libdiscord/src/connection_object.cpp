#include "connection_object.h"
#include "connection_state.h"
#include "guild.h"

namespace discord
{
  ConnectionObject::ConnectionObject() : m_owner(nullptr)
  {
  }

  ConnectionObject::ConnectionObject(ConnectionState* owner) : m_owner(owner)
  {
  }

  ConnectionState* ConnectionObject::owner() const
  {
    return m_owner;
  }

  std::unique_ptr<Guild> ConnectionObject::find_guild(Snowflake id) const
  {
    return m_owner->find_guild(id);
  }
}

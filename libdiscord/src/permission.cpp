#include "permission.h"

namespace discord
{
  Permission::Permission()
  {
    m_permissions = 0;
  }

  Permission::Permission(uint32_t bits)
  {
    m_permissions = bits;
  }

  Permission::Permission(rapidjson::Value& data)
  {
    m_permissions = data.GetInt();
  }

  uint32_t Permission::get() const
  {
    return m_permissions;
  }

  void Permission::add(PermissionFlag permission)
  {
    m_permissions |= permission;
  }

  void Permission::remove(PermissionFlag permission)
  {
    m_permissions &= ~permission;
  }
}

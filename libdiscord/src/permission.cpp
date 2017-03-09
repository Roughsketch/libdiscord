#include "permission.h"

namespace discord
{
  permission::permission()
  {
    m_permissions = 0;
  }

  permission::permission(uint32_t bits)
  {
    m_permissions = bits;
  }

  permission::permission(rapidjson::Value& data)
  {
    m_permissions = data.GetInt();
  }

  uint32_t permission::get() const
  {
    return m_permissions;
  }

  void permission::add(permission_flag permission)
  {
    m_permissions |= permission;
  }

  void permission::remove(permission_flag permission)
  {
    m_permissions &= ~permission;
  }
}

#pragma once

#include "common.h"
#include "permission.h"

namespace discord
{
  class Role : public Identifiable
  {
    std::string m_name;
    uint32_t m_color;
    bool m_hoist;
    int32_t m_position;
    Permission m_permissions;
    bool m_managed;
    bool m_mentionable;
  public:
    Role();
    explicit Role(rapidjson::Value& data);
  };
}
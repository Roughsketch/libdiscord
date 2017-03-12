#pragma once

#include "common.h"
#include "permission.h"

namespace discord
{
  class role : public identifiable
  {
    std::string m_name;
    uint32_t m_color;
    bool m_hoist;
    int32_t m_position;
    permission m_permissions;
    bool m_managed;
    bool m_mentionable;
  public:
    role();
    explicit role(rapidjson::Value& data);
  };
}
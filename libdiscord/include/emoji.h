#pragma once

#include "common.h"

namespace discord
{
  class emoji : public identifiable
  {
    std::string m_name;
    std::vector<snowflake> m_roles;
    bool m_require_colons;
    bool m_managed;
  public:
    emoji();
    explicit emoji(rapidjson::Value& data);

    std::string name() const;
    std::vector<snowflake> roles() const;
    std::string mention() const;
  };
}
#pragma once

#include "common.h"

namespace discord
{
  class Emoji : public Identifiable
  {
    std::string m_name;
    std::vector<Snowflake> m_roles;
    bool m_require_colons;
    bool m_managed;
  public:
    Emoji();
    explicit Emoji(rapidjson::Value& data);

    std::string name() const;
    std::vector<Snowflake> roles() const;
    std::string mention() const;
  };
}
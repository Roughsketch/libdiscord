#pragma once

#include "common.h"
#include "user.h"

namespace discord
{
  class member
  {
    std::string m_token;

    user m_user;
    std::string m_nick;
    std::vector<snowflake> m_roles;
    std::string m_joined_at;
    bool m_deaf;
    bool m_mute;
  public:
    member();
    explicit member(const std::string& token, rapidjson::Value& data);

    const user& user() const;

    void set_roles(std::vector<snowflake>& role_ids);
    void set_user(discord::user& user);
    void set_nick(std::string nick);
  };
}
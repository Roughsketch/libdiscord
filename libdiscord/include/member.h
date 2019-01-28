#pragma once

#include "common.h"
#include "user.h"

namespace discord
{
  class Member : public ConnectionObject
  {
    User m_user;
    std::string m_nick;
    std::vector<Snowflake> m_roles;
    std::string m_joined_at;
    bool m_deaf;
    bool m_mute;
  public:
    Member();
    explicit Member(ConnectionState* owner, rapidjson::Value& data);

    const User& user() const;

    void set_roles(std::vector<Snowflake>& role_ids);
    void set_user(discord::User& user);
    void set_nick(std::string nick);
  };
}
#include "member.h"

namespace discord
{
  Member::Member()
  {
    m_deaf = false;
    m_mute = false;
  }

  Member::Member(ConnectionState* owner, rapidjson::Value& data) : ConnectionObject(owner)
  {
    set_from_json(m_nick, "nick", data);
    set_from_json(m_joined_at, "joined_at", data);
    set_from_json(m_deaf, "deaf", data);
    set_from_json(m_mute, "mute", data);

    auto found = data.FindMember("user");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_user = discord::User(owner, found->value);
    }

    found = data.FindMember("roles");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      for (const auto& role_id : found->value.GetArray())
      {
        m_roles.emplace_back(role_id.GetString());
      }
    }

  }

  const User& Member::user() const
  {
    return m_user;
  }

  void Member::set_roles(std::vector<Snowflake>& role_ids)
  {
    m_roles = role_ids;
  }

  void Member::set_user(discord::User& user)
  {
    m_user = user;
  }

  void Member::set_nick(std::string nick)
  {
    m_nick = nick;
  }
}

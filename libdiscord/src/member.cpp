#include "member.h"

namespace discord
{
  member::member()
  {
    m_deaf = false;
    m_mute = false;
  }

  member::member(const std::string& token, rapidjson::Value& data)
  {
    set_from_json(m_nick, "nick", data);
    set_from_json(m_joined_at, "joined_at", data);
    set_from_json(m_deaf, "deaf", data);
    set_from_json(m_mute, "mute", data);

    auto found = data.FindMember("user");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_user = discord::user(token, found->value);
    }

    found = data.FindMember("roles");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      for (auto& role_id : found->value.GetArray())
      {
        m_roles.push_back(snowflake(role_id.GetString()));
      }
    }

  }

  const user& member::user() const
  {
    return m_user;
  }

  void member::set_roles(std::vector<snowflake>& role_ids)
  {
    m_roles = role_ids;
  }

  void member::set_user(discord::user& user)
  {
    m_user = user;
  }

  void member::set_nick(std::string nick)
  {
    m_nick = nick;
  }
}

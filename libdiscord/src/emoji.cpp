#include "emoji.h"

namespace discord
{
  emoji::emoji()
  {
    m_require_colons = false;
    m_managed = false;
  }

  emoji::emoji(rapidjson::Value& data) : identifiable(data["id"])
  {
    set_from_json(m_name, "name", data);
    set_from_json(m_require_colons, "require_colons", data);
    set_from_json(m_managed, "managed", data);

    auto found = data.FindMember("roles");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      for (const auto& emoji_role : data["roles"].GetArray())
      {
        m_roles.emplace_back(emoji_role.GetString());
      }
    }

    //  Make sure roles are sorted by id to make comparisons easier.
    std::sort(std::begin(m_roles), std::end(m_roles));
  }  
  
  std::string emoji::name() const
  {
    return m_name;
  }

  std::vector<snowflake> emoji::roles() const
  {
    return m_roles;
  }

  std::string emoji::mention() const
  {
    return "<:" + name() + ":" + id().to_string() + ">";
  }
}

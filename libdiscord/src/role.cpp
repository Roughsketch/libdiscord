#include "role.h"

namespace discord
{
  Role::Role()
  {
    m_color = 0;
    m_hoist = false;
    m_position = 0;
    m_managed = false;
    m_mentionable = false;
  }

  Role::Role(rapidjson::Value& data)
  {
    set_from_json(m_name, "name", data);
    set_from_json(m_color, "color", data);
    set_from_json(m_hoist, "hoist", data);
    set_from_json(m_position, "position", data);
    set_from_json(m_managed, "managed", data);
    set_from_json(m_mentionable, "mentionable", data);

    auto found = data.FindMember("permissions");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_permissions = Permission(data["permissions"].GetInt());
    }
  }
}

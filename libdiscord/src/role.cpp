#include "role.h"

namespace discord
{
  role::role()
  {
    m_color = 0;
    m_hoist = false;
    m_position = 0;
    m_managed = false;
    m_mentionable = false;
  }

  role::role(rapidjson::Value& data)
  {
    set_from_json(m_name, "name", data);
    set_from_json(m_color, "color", data);
    set_from_json(m_hoist, "hoist", data);
    set_from_json(m_position, "position", data);
    set_from_json(m_managed, "managed", data);
    set_from_json(m_mentionable, "mentionable", data);

    auto found = data.FindMember("permissions");
    if (found != data.MemberEnd())
    {
      m_permissions = permission(data["permissions"].GetInt());
    }
  }
}

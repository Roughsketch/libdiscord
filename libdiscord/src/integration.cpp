#include "integration.h"

namespace discord
{
  integration_account::integration_account()
  {
  }

  integration_account::integration_account(rapidjson::Value& data)
  {
  }

  std::string integration_account::id() const
  {
    return m_id;
  }

  std::string integration_account::name() const
  {
    return m_name;
  }

  integration::integration()
  {
    m_enabled = false;
    m_syncing = false;
    m_expire_behavior = 0;
    m_expire_grace_period = 0;
  }

  integration::integration(connection_state* owner, rapidjson::Value& data) : identifiable(data["id"]), connection_object(owner)
  {
    set_from_json(m_name, "name", data);
    set_from_json(m_type, "type", data);
    set_from_json(m_enabled, "enabled", data);
    set_from_json(m_syncing, "syncing", data);
    set_from_json(m_role_id, "role_id", data);
    set_from_json(m_expire_behavior, "expire_behavior", data);
    set_from_json(m_expire_grace_period, "expire_grace_period", data);
    set_from_json(m_timestamp, "synced_at", data);

    auto found = data.FindMember("user");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_user = user(owner, found->value);
    }
    
    found = data.FindMember("account");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_account = integration_account(found->value);
    }
  }
}

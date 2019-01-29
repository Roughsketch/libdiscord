#include "integration.h"
#include "user.h"

namespace discord
{
  IntegrationAccount::IntegrationAccount()
  {
  }

  IntegrationAccount::IntegrationAccount(rapidjson::Value& data)
  {
  }

  std::string IntegrationAccount::id() const
  {
    return m_id;
  }

  std::string IntegrationAccount::name() const
  {
    return m_name;
  }

  Integration::Integration()
  {
    m_enabled = false;
    m_syncing = false;
    m_expire_behavior = 0;
    m_expire_grace_period = 0;
  }

  Integration::Integration(ConnectionState* owner, rapidjson::Value& data) : Identifiable(data["id"]), ConnectionObject(owner)
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
      m_user = User(owner, found->value);
    }
    
    found = data.FindMember("account");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_account = IntegrationAccount(found->value);
    }
  }
}

#pragma once

#include "common.h"
#include "identifiable.h"
#include "user.h"

namespace discord
{
  class integration_account
  {
    std::string m_id;
    std::string m_name;
  public:
    integration_account();
    explicit integration_account(rapidjson::Value& data);

    /** Get the id of this integration account.

    @return The id of this account.
    */
    std::string id() const;

    /** Get the name of this integration account.

    @return The name of this account.
    */
    std::string name() const;
  };

  class integration : public identifiable, public connection_object
  {
    std::string m_name;
    std::string m_type;
    bool m_enabled;
    bool m_syncing;
    snowflake m_role_id;
    uint32_t m_expire_behavior;
    uint32_t m_expire_grace_period;
    user m_user;
    integration_account m_account;
    std::string m_timestamp;
  public:
    integration();
    explicit integration(connection_state* owner, rapidjson::Value& data);
  };
}
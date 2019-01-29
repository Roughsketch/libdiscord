#pragma once

#include "common.h"
#include "identifiable.h"
#include "user.h"

namespace discord
{
  class IntegrationAccount
  {
    std::string m_id;
    std::string m_name;
  public:
    IntegrationAccount();
    explicit IntegrationAccount(rapidjson::Value& data);

    /** Get the id of this Integration account.

    @return The id of this account.
    */
    std::string id() const;

    /** Get the name of this Integration account.

    @return The name of this account.
    */
    std::string name() const;
  };

  class Integration : public Identifiable, public ConnectionObject
  {
    std::string m_name;
    std::string m_type;
    bool m_enabled;
    bool m_syncing;
    Snowflake m_role_id;
    uint32_t m_expire_behavior;
    uint32_t m_expire_grace_period;
    User m_user;
    IntegrationAccount m_account;
    std::string m_timestamp;
  public:
    Integration();
    explicit Integration(ConnectionState* owner, rapidjson::Value& data);
  };
}
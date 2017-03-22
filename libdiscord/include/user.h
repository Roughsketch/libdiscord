#pragma once

#include "common.h"
#include "connection_object.h"
#include "permission.h"
#include "integration.h"

namespace discord
{
  class user : public identifiable, public connection_object
  {
    std::string m_username;
    std::string m_discriminator;
    std::string m_avatar;
    bool m_bot;
    bool m_mfa_enabled;
    bool m_verified;
    std::string m_email;
  public:
    user();
    explicit user(connection_state* owner, rapidjson::Value& data);

    std::string name() const;
    std::string discriminator() const;
    std::string distinct() const;
  };

  class user_guild : public identifiable
  {
    std::string m_name;
    std::string m_icon;
    bool m_owner;
    permission m_permissions;
  public:
    user_guild();
    explicit user_guild(rapidjson::Value& data);

    /** Get the name of this guild.

    @return The name of this guild object.
    */
    std::string name() const;

    /** Get the icon hash of this guild.

    @return The icon hash of this guild.
    */
    std::string icon() const;

    /** Get owner status of this guild.

    @return True if the current user is the owner of this guild.
    */
    bool owner() const;

    /** Get the permissions set for this user on this guild.

    @return Current user's permissions in this guild.
    */
    permission permissions() const;
  };

  class connection
  {
    std::string m_id;
    std::string m_name;
    std::string m_type;
    bool m_revoked;
    std::vector<integration> m_integrations;
  public:
    connection();
    explicit connection(rapidjson::Value& data);

    /** Gets the id for this connection.

    @return The id of this connection.
    */
    std::string id() const;

    /** Get the name of this connection.

    @return The name of this connection.
    */
    std::string name() const;

    /** Gets the type of this connection (twitch, youtube)

    @return The type of this connection.
    */
    std::string type() const;

    /** Get the revoked status of this connection.

    @return The revoked status of this connection.
    */
    bool revoked() const;

    /** Gets the list of partial integrations associated with this connection.

    @return The list of partial integrations associated with this connection.
    */
    std::vector<integration> integrations() const;
  };
}

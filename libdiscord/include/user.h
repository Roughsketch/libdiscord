#pragma once

#include "common.h"
#include "connection_object.h"
#include "permission.h"

namespace discord
{
  class User : public Identifiable, public ConnectionObject
  {
    std::string m_username;
    std::string m_discriminator;
    std::string m_avatar;
    bool m_bot;
    bool m_mfa_enabled;
    bool m_verified;
    std::string m_email;
  public:
    User();
    explicit User(ConnectionState* owner, rapidjson::Value& data);

    std::string name() const;
    std::string discriminator() const;
    std::string distinct() const;
  };

  class user_guild : public Identifiable
  {
    std::string m_name;
    std::string m_icon;
    bool m_owner;
    Permission m_permissions;
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
    Permission permissions() const;
  };
}

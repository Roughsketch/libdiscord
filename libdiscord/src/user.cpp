#include "user.h"
#include "connection_state.h"

namespace discord
{
  User::User()
  {
    m_bot = false;
    m_mfa_enabled = false;
    m_verified = false;
  }

  User::User(ConnectionState* owner, rapidjson::Value& data) : Identifiable(data["id"]), ConnectionObject(owner)
  {
    set_from_json(m_username, "username", data);
    set_from_json(m_discriminator, "discriminator", data);
    set_from_json(m_avatar, "avatar", data);
    set_from_json(m_bot, "bot", data);
    set_from_json(m_mfa_enabled, "mfa_enabled", data);
    set_from_json(m_verified, "verified", data);
    set_from_json(m_email, "email", data);
  }

  std::string User::name() const
  {
    return m_username;
  }

  std::string User::discriminator() const
  {
    return m_discriminator;
  }

  std::string User::distinct() const
  {
    return m_username + "#" + m_discriminator;
  }
}

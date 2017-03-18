#include "user.h"
#include "connection_state.h"

namespace discord
{
  user::user()
  {
    m_bot = false;
    m_mfa_enabled = false;
    m_verified = false;
  }

  user::user(connection_state* owner, rapidjson::Value& data) : identifiable(data["id"]), connection_object(owner)
  {
    set_from_json(m_username, "username", data);
    set_from_json(m_discriminator, "discriminator", data);
    set_from_json(m_avatar, "avatar", data);
    set_from_json(m_bot, "bot", data);
    set_from_json(m_mfa_enabled, "mfa_enabled", data);
    set_from_json(m_verified, "verified", data);
    set_from_json(m_email, "email", data);
  }

  std::string user::name() const
  {
    return m_username;
  }

  std::string user::discriminator() const
  {
    return m_discriminator;
  }

  std::string user::distinct() const
  {
    return m_username + "#" + m_discriminator;
  }
}

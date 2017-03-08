#include "user.h"
#include "bot.h"

namespace discord
{
  user::user()
  {
    m_bot = false;
    m_mfa_enabled = false;
    m_verified = false;
  }

  user::user(std::string token, rapidjson::Value& data) : identifiable(data["id"]), m_token(token)
  {
    m_username = data["username"].GetString();
    m_discriminator = data["discriminator"].GetString();
    m_avatar = data["avatar"].GetString();
    m_bot = data["bot"].GetBool();
    m_mfa_enabled = data["mfa_enabled"].GetBool();
    m_verified = data["verified"].GetBool();
    m_email = data["email"].GetString();
  }
}

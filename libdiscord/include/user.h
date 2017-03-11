#pragma once

#include "common.h"
#include "bot_ownable.h"

namespace discord
{
  class bot;

  class user : public identifiable, public bot_ownable
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
    explicit user(const bot* owner, rapidjson::Value& data);

    std::string name() const;
    std::string discriminator() const;
    std::string distinct() const;
  };
}

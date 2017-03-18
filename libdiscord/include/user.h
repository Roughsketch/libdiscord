#pragma once

#include "common.h"
#include "connection_object.h"

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
}

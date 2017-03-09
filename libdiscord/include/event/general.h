#pragma once

#include "common.h"
#include "bot.h"

namespace discord
{
  class event
  {
    const std::string m_token;

  public:
    explicit event(const std::string& token);
  };

  inline event::event(const std::string& token) : m_token(token)
  {
  }
}

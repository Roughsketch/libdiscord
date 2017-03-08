#pragma once

#include "common.h"
#include "bot.h"

namespace discord
{
  class event
  {
    std::string m_token;

  public:
    explicit event(std::string token);
  };
}

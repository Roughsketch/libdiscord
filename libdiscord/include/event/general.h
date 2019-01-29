#pragma once

#include "common.h"
#include "bot.h"

namespace discord
{
  class Event
  {
    const std::string m_token;

  public:
    explicit Event(const std::string& token);
  };

  inline Event::Event(const std::string& token) : m_token(token)
  {
  }
}

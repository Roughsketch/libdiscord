#pragma once

#include "common.h"
#include "bot.h"

namespace discord
{
  class event
  {
    bot& m_owner;

  public:
    explicit event(bot& owner);

    bot& get_bot() const;
  };

  inline bot& event::get_bot() const
  {
    return m_owner;
  }
}

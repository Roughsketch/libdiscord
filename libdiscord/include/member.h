#pragma once

#include "common.h"

namespace discord
{
  class bot;

  class member
  {
    bot& m_owner;
  public:
    explicit member(bot& owner, rapidjson::Value& data);
  };
}
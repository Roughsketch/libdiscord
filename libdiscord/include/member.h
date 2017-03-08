#pragma once

#include "common.h"

namespace discord
{
  class bot;

  class member
  {
    std::string m_token;
  public:
    explicit member(std::string token, rapidjson::Value& data);
  };
}
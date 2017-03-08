#pragma once

#include "common.h"

namespace discord
{
  class bot;

  class user : public identifiable
  {
    std::string m_token;
  public:
    explicit user(std::string token, rapidjson::Value& data);

    std::string name() const;
    std::string discriminator() const;
    std::string distinct() const;
  };
}
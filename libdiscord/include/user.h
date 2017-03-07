#pragma once

#include "common.h"

namespace discord
{
  class bot;

  class user : public identifiable
  {
    bot& m_owner;
  public:
    explicit user(bot& owner, rapidjson::Value& data);

    std::string name() const;
    std::string discriminator() const;
    std::string distinct() const;
  };
}
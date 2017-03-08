#pragma once

#include "common.h"

namespace discord
{
  class emoji : public identifiable
  {
  public:
    explicit emoji(rapidjson::Value& data);

    std::string name() const;
    std::vector<snowflake> roles() const;
  };
}
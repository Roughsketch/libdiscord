#pragma once

#include "common.h"

namespace discord
{
  class voice_state
  {
  public:
    explicit voice_state(rapidjson::Value& data);
  };
}
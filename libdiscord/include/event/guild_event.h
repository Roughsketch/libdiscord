#pragma once

#include "event/general.h"

namespace discord
{
  class presence_event : public event
  {
  public:
    explicit presence_event(const std::string& token, rapidjson::Value& data);

  };
}
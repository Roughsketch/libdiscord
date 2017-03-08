#pragma once

#include "event/general.h"

namespace discord
{
  class presence_event : public event
  {
    std::string m_token;
  public:
    explicit presence_event(std::string token, rapidjson::Value& data);

    user& user() const;
  };
}
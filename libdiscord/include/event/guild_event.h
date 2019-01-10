#pragma once

#include "event/general.h"

namespace discord
{
	class guild_created : public event
	{
	public:
		explicit guild_created(const std::string& token, rapidjson::Value& data);
	};

  class presence_event : public event
  {
  public:
    explicit presence_event(const std::string& token, rapidjson::Value& data);

  };
}
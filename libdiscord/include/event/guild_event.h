#pragma once

#include "event/general.h"

namespace discord
{
	class GuildCreated : public Event
	{
	public:
		explicit GuildCreated(const std::string& token, rapidjson::Value& data);
	};

  class PresenceEvent : public Event
  {
  public:
    explicit PresenceEvent(const std::string& token, rapidjson::Value& data);

  };
}
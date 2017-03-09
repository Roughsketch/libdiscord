#pragma once

#include "common.h"

namespace discord
{
  class voice_state
  {
    snowflake m_guild_id;
    snowflake m_channel_id;
    snowflake m_user_id;
    std::string m_session_id;
    bool m_deaf;
    bool m_mute;
    bool m_self_deaf;
    bool m_self_mute;
    bool m_suppress;
  public:
    voice_state();
    explicit voice_state(rapidjson::Value& data);
  };
}
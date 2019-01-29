#pragma once

#include "common.h"

namespace discord
{
  class VoiceState
  {
    Snowflake m_guild_id;
    Snowflake m_channel_id;
    Snowflake m_user_id;
    std::string m_session_id;
    bool m_deaf;
    bool m_mute;
    bool m_self_deaf;
    bool m_self_mute;
    bool m_suppress;
  public:
    VoiceState();
    explicit VoiceState(rapidjson::Value& data);
  };

  class VoiceRegion
  {
    std::string m_id;
    std::string m_name;
    std::string m_sample_hostname;
    uint32_t m_sample_port;
    bool m_vip;
    bool m_optimal;
    bool m_deprecated;
    bool m_custom;
  public:
    VoiceRegion();
    explicit VoiceRegion(rapidjson::Value& data);
  };
}
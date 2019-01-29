#include "voice.h"

namespace discord
{
  VoiceState::VoiceState()
  {
    m_deaf = false;
    m_mute = false;
    m_self_deaf = false;
    m_self_mute = false;
    m_suppress = false;
  }

  VoiceState::VoiceState(rapidjson::Value& data)
  {
    set_from_json(m_guild_id, "guild_id", data);
    set_from_json(m_channel_id, "channel_id", data);
    set_from_json(m_user_id, "user_id", data);
    set_from_json(m_session_id, "session_id", data);
    set_from_json(m_deaf, "deaf", data);
    set_from_json(m_mute, "mute", data);
    set_from_json(m_self_deaf, "self_deaf", data);
    set_from_json(m_self_mute, "self_mute", data);
    set_from_json(m_suppress, "suppress", data);
  }

  VoiceRegion::VoiceRegion()
  {
    m_sample_port = 0;
    m_vip = false;
    m_optimal = false;
    m_deprecated = false;
    m_custom = false;
  }

  VoiceRegion::VoiceRegion(rapidjson::Value& data)
  {
    set_from_json(m_id, "id", data);
    set_from_json(m_name, "name", data);
    set_from_json(m_sample_hostname, "sample_hostname", data);
    set_from_json(m_sample_port, "sample_port", data);
    set_from_json(m_vip, "vip", data);
    set_from_json(m_optimal, "optimal", data);
    set_from_json(m_deprecated, "deprecated", data);
    set_from_json(m_custom, "custom", data);
  }
}

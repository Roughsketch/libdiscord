#include "channel.h"
#include "bot.h"
#include "connection_object.h"
#include "connection_state.h"
#include "guild.h"

#include "api/channel_api.h"

namespace discord
{
  overwrite::overwrite()
  {
  }

  overwrite::overwrite(rapidjson::Value& data) : identifiable(data["id"])
  {
    set_from_json(m_type, "type", data);

    auto found = data.FindMember("allow");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_allow = permission(found->value.GetInt());
    }

    found = data.FindMember("deny");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_deny = permission(found->value.GetInt());
    }
  }

  std::string overwrite::type() const
  {
    return m_type;
  }

  permission overwrite::allow() const
  {
    return m_allow;
  }

  permission overwrite::deny() const
  {
    return m_deny;
  }

  channel::channel()
  {
    m_type = Text;
    m_position = 0;
    m_bitrate = 0;
    m_user_limit = 0;
    m_is_dm = false;
  }

  channel::channel(connection_state* owner, rapidjson::Value& data)
    : identifiable(data["id"]), connection_object(owner)
  {
    set_from_json(m_last_message_id, "last_message_id", data);
    set_from_json(m_name, "name", data);
    set_from_json(m_position, "position", data);
    set_from_json(m_topic, "topic", data);
    set_from_json(m_bitrate, "bitrate", data);
    set_from_json(m_user_limit, "user_limit", data);

    auto found = data.FindMember("type");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_type = static_cast<channel_type>(found->value.GetInt());
    }

    found = data.FindMember("permission_overwrites");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      for (auto& perm_ow : data["permission_overwrites"].GetArray())
      {
        m_permission_overwrites.push_back(overwrite(perm_ow));
      }
    }

    found = data.FindMember("recipient");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_recipient = user(owner, found->value);
      m_is_dm = true;
    }
  }

  std::string channel::name() const
  {
    return m_name;
  }

  channel_type channel::type() const
  {
    return m_type;
  }

  int32_t channel::position() const
  {
    return m_position;
  }

  uint32_t channel::bitrate() const
  {
    return m_bitrate;
  }

  uint32_t channel::user_limit() const
  {
    return m_user_limit;
  }

  std::string channel::topic() const
  {
    return m_topic;
  }

  guild channel::guild() const
  {
    return m_owner->find_guild_from_channel(m_id);
  }

  channel channel::remove() const
  {
    return api::channel::remove(m_owner, m_id);
  }
}

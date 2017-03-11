#include "channel.h"
#include "bot.h"

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

  channel::channel(const bot* owner, snowflake guild_id, rapidjson::Value& data)
    : identifiable(data["id"]), bot_ownable(owner), m_guild_id(guild_id)
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

  channel& channel::operator=(const channel& other)
  {
    bot_ownable::operator=(other);

    m_id = other.m_id;
    m_last_message_id = other.m_last_message_id;
    m_guild_id = other.m_guild_id;
    m_name = other.m_name;
    m_type = other.m_type;
    m_position = other.m_position;
    m_permission_overwrites = other.m_permission_overwrites;
    m_topic = other.m_topic;
    m_bitrate = other.m_bitrate;
    m_user_limit = other.m_user_limit;
    m_recipient = other.m_recipient;
    m_is_dm = other.m_is_dm;

    return *this;
  }

  snowflake channel::guild_id() const
  {
    return m_guild_id;
  }
}

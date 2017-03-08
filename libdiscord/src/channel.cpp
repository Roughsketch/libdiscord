#include "channel.h"
#include "bot.h"

namespace discord
{
  channel::channel()
  {
    m_type = Text;
    m_position = 0;
    m_is_private = false;
    m_bitrate = 0;
    m_user_limit = 0;
    m_is_dm = false;
  }

  channel::channel(std::string token, snowflake guild_id, rapidjson::Value& data)
    : identifiable(data["id"]), m_token(token), m_guild_id(guild_id)
  {
    m_is_private = data["is_private"].GetBool();
    m_last_message_id = snowflake(data["last_message_id"].GetString());
    m_name = data["name"].GetString();
    m_type = static_cast<channel_type>(data["type"].GetInt());
    m_position = data["position"].GetInt();

    for (auto& perm_ow : data["permission_overwrites"].GetArray())
    {
      m_permission_overwrites.push_back(overwrite(perm_ow));
    }

    m_topic = data["topic"].GetString();
    m_bitrate = data["bitrate"].GetInt();
    m_user_limit = data["user_limit"].GetInt();

    auto recipient = data.FindMember("recipient");
    if (recipient != data.MemberEnd())
    {
      m_recipient = user(token, data["recipient"]);
      m_is_dm = true;
    }
  }

  snowflake channel::guild_id() const
  {
    return m_guild_id;
  }
}

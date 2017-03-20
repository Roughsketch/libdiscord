#include "channel.h"
#include "bot.h"
#include "connection_object.h"
#include "connection_state.h"
#include "guild.h"

#include "api/channel_api.h"
#include "discord_exception.h"

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

  std::future<channel> channel::modify(std::string name, int32_t position, std::string topic)
  {
    if (m_type != Text)
    {
      throw discord_exception("Cannot modify text channel attributes on a non-text channel.");
    }

    return std::async(std::launch::async, api::channel::modify_text_channel, m_owner, m_id, name, position, topic);
  }

  std::future<channel> channel::modify(std::string name, int32_t position, uint32_t bitrate, uint32_t user_limit)
  {
    if (m_type != Voice)
    {
      throw discord_exception("Cannot modify voice channel attributes on a non-voice channel.");
    }

    return std::async(std::launch::async, api::channel::modify_voice_channel, m_owner, m_id, name, position, bitrate, user_limit);
  }

  std::future<channel> channel::remove() const
  {
    return std::async(std::launch::async, api::channel::remove, m_owner, m_id);
  }

  std::future<std::vector<message>> channel::get_messages(int32_t limit, search_method method, snowflake pivot)
  {
    return std::async(std::launch::async, api::channel::get_messages, m_owner, m_id, limit, method, pivot);
  }

  std::future<message> channel::get_message(snowflake message_id)
  {
    return std::async(std::launch::async, api::channel::get_message, m_owner, m_id, message_id);
  }

  std::future<bool> channel::create_reaction(snowflake message_id, emoji emoji)
  {
    return std::async(std::launch::async, api::channel::create_reaction, m_owner, m_id, message_id, emoji);
  }

  std::future<bool> channel::remove_reaction(snowflake message_id, emoji emoji, snowflake user_id)
  {
    if (user_id.id() == 0)
    {
      return std::async(std::launch::async, api::channel::remove_own_reaction, m_owner, m_id, message_id, emoji);
    }

    return std::async(std::launch::async, api::channel::remove_user_reaction, m_owner, m_id, message_id, emoji, user_id);
  }

  std::future<std::vector<user>> channel::get_reactions(snowflake message_id, emoji emoji)
  {
    return std::async(std::launch::async, api::channel::get_reactions, m_owner, m_id, message_id, emoji);
  }

  std::future<void> channel::remove_all_reactions(snowflake message_id)
  {
    return std::async(std::launch::async, api::channel::remove_all_reactions, m_owner, m_id, message_id);
  }

  std::future<message> channel::edit_message(snowflake message_id, std::string new_content)
  {
    return std::async(std::launch::async, api::channel::edit_message, m_owner, m_id, message_id, new_content);
  }

  std::future<bool> channel::remove_message(snowflake message_id)
  {
    return std::async(std::launch::async, api::channel::remove_message, m_owner, m_id, message_id);
  }

  std::future<bool> channel::bulk_remove_messages(std::vector<snowflake> message_ids)
  {
    return std::async(std::launch::async, api::channel::bulk_remove_messages, m_owner, m_id, message_ids);
  }

  std::future<bool> channel::edit_permissions(overwrite overwrite, uint32_t allow, uint32_t deny, std::string type)
  {
    return std::async(std::launch::async, api::channel::edit_permissions, m_owner, m_id, overwrite, allow, deny, type);
  }

  std::future<bool> channel::remove_permission(overwrite overwrite)
  {
    return std::async(std::launch::async, api::channel::remove_permission, m_owner, m_id, overwrite);
  }

  std::future<bool> channel::start_typing()
  {
    return std::async(std::launch::async, api::channel::trigger_typing_indicator, m_owner, m_id);
  }

  std::future<std::vector<message>> channel::get_pinned_messages()
  {
    return std::async(std::launch::async, api::channel::get_pinned_messages, m_owner, m_id);
  }

  std::future<bool> channel::pin(snowflake message_id)
  {
    return std::async(std::launch::async, api::channel::add_pinned_message, m_owner, m_id, message_id);
  }

  std::future<bool> channel::unpin(snowflake message_id)
  {
    return std::async(std::launch::async, api::channel::remove_pinned_message, m_owner, m_id, message_id);
  }

  std::future<void> channel::add_recipient(snowflake user_id, std::string access_token, std::string nickname)
  {
    return std::async(std::launch::async, api::channel::group_dm_add_recipient, m_owner, m_id, user_id, access_token, nickname);
  }

  std::future<void> channel::remove_recipient(snowflake user_id)
  {
    return std::async(std::launch::async, api::channel::group_dm_remove_recipient, m_owner, m_id, user_id);
  }
}

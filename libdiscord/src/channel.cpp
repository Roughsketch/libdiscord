#include "channel.h"
#include "bot.h"
#include "connection_object.h"
#include "connection_state.h"
#include "guild.h"

#include "api/channel_api.h"
#include "discord_exception.h"

namespace discord
{
  Overwrite::Overwrite()
  {
  }

  Overwrite::Overwrite(rapidjson::Value& data) : Identifiable(data["id"])
  {
    set_from_json(m_type, "type", data);

    auto found = data.FindMember("allow");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_allow = Permission(found->value.GetInt());
    }

    found = data.FindMember("deny");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_deny = Permission(found->value.GetInt());
    }
  }

  std::string Overwrite::type() const
  {
    return m_type;
  }

  Permission Overwrite::allow() const
  {
    return m_allow;
  }

  Permission Overwrite::deny() const
  {
    return m_deny;
  }

  Channel::Channel()
  {
    m_type = Text;
    m_position = 0;
    m_bitrate = 0;
    m_user_limit = 0;
    m_is_dm = false;
    m_empty = true;
  }

  Channel::Channel(ConnectionState* owner, rapidjson::Value& data)
    : Identifiable(data["id"]), ConnectionObject(owner)
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
      m_type = static_cast<ChannelType>(found->value.GetInt());
    }

    found = data.FindMember("permission_overwrites");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      for (auto& perm_ow : data["permission_overwrites"].GetArray())
      {
        m_permission_overwrites.emplace_back(perm_ow);
      }
    }

    found = data.FindMember("recipient");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_recipient = User(owner, found->value);
      m_is_dm = true;
    }

    m_empty = false;
  }

  std::string Channel::name() const
  {
    return m_name;
  }

  ChannelType Channel::type() const
  {
    return m_type;
  }

  int32_t Channel::position() const
  {
    return m_position;
  }

  uint32_t Channel::bitrate() const
  {
    return m_bitrate;
  }

  uint32_t Channel::user_limit() const
  {
    return m_user_limit;
  }

  std::string Channel::topic() const
  {
    return m_topic;
  }

  std::unique_ptr<Guild> Channel::guild() const
  {
    return m_owner->find_guild_from_channel(m_id);
  }

  bool Channel::empty() const
  {
    return m_empty;
  }

  pplx::task<Channel> Channel::modify(std::string name, std::string topic, int32_t position) const
  {
    if (m_type != Text)
    {
      throw DiscordException("Cannot modify text channel attributes on a non-text channel.");
    }

    return api::channel::modify_text_channel(m_owner, m_id, name, position, topic);
  }

  pplx::task<Channel> Channel::modify(std::string name, int32_t position, uint32_t bitrate, uint32_t user_limit) const
  {
    if (m_type != Voice)
    {
      throw DiscordException("Cannot modify voice channel attributes on a non-voice channel.");
    }

    return api::channel::modify_voice_channel(m_owner, m_id, name, position, bitrate, user_limit);
  }

  pplx::task<Channel> Channel::remove() const
  {
    return api::channel::remove(m_owner, m_id);
  }

  pplx::task<std::vector<Message>> Channel::get_messages(int32_t limit, SearchMethod method, Snowflake pivot) const
  {

    return api::channel::get_messages(m_owner, m_id, limit, method, pivot);
  }

  pplx::task<Message> Channel::get_message(Snowflake message_id) const
  {
    return api::channel::get_message(m_owner, m_id, message_id);
  }

  pplx::task<Message> Channel::send_message(std::string content, bool tts, discord::Embed Embed) const
  {
    return api::channel::create_message(m_owner, m_id, content, tts, Embed);
  }

  pplx::task<Message> Channel::send_embed(std::function<void(Embed&)> modify_callback, std::string content) const
  {
    Embed Embed;
    modify_callback(Embed);

    return api::channel::create_message(m_owner, m_id, content, false, Embed);
  }

  pplx::task<bool> Channel::create_reaction(Snowflake message_id, Emoji emoji) const
  {
    return api::channel::create_reaction(m_owner, m_id, message_id, emoji.name() + ":" + emoji.id().to_string());
  }

  pplx::task<bool> Channel::create_reaction(Snowflake message_id, std::string emoji) const
  {
    return api::channel::create_reaction(m_owner, m_id, message_id, emoji);
  }

  pplx::task<bool> Channel::remove_reaction(Snowflake message_id, Emoji emoji, Snowflake user_id) const
  {
    if (user_id.id() == 0)
    {
      return api::channel::remove_own_reaction(m_owner, m_id, message_id, emoji);
    }

    return api::channel::remove_user_reaction(m_owner, m_id, message_id, emoji, user_id);
  }

  pplx::task<std::vector<User>> Channel::get_reactions(Snowflake message_id, Emoji emoji) const
  {
    return api::channel::get_reactions(m_owner, m_id, message_id, emoji);
  }

  pplx::task<void> Channel::remove_all_reactions(Snowflake message_id) const
  {
    return api::channel::remove_all_reactions(m_owner, m_id, message_id);
  }

  pplx::task<Message> Channel::edit_message(Snowflake message_id, std::string new_content) const
  {
    return api::channel::edit_message(m_owner, m_id, message_id, new_content);
  }

  pplx::task<bool> Channel::remove_message(Snowflake message_id) const
  {
    return api::channel::remove_message(m_owner, m_id, message_id);
  }

  pplx::task<bool> Channel::remove_messages(std::vector<Snowflake> message_ids) const
  {
    return api::channel::bulk_remove_messages(m_owner, m_id, message_ids);
  }

  pplx::task<bool> Channel::remove_messages(int amount) const
  {
    if (amount < 2 || amount > 100)
    {
      throw DiscordException("Bulk message delete amount must be between 2 and 100 inclusive.");
    }

    return api::channel::get_messages(m_owner, m_id, amount)
      .then([owner = m_owner, id = m_id](std::vector<Message> messages) {
        std::vector<Snowflake> msg_ids(messages.size());
        std::transform(std::begin(messages), std::end(messages), std::begin(msg_ids), 
          [](const Message& msg) -> Snowflake {
            return msg.id();
          });
        return api::channel::bulk_remove_messages(owner, id, msg_ids).get();
      });
  }

  pplx::task<bool> Channel::edit_permissions(Overwrite overwrite, uint32_t allow, uint32_t deny, std::string type) const
  {
    return api::channel::edit_permissions(m_owner, m_id, overwrite, allow, deny, type);
  }

  pplx::task<bool> Channel::remove_permission(Overwrite overwrite) const
  {
    return api::channel::remove_permission(m_owner, m_id, overwrite);
  }

  pplx::task<bool> Channel::start_typing() const
  {
    return api::channel::trigger_typing_indicator(m_owner, m_id);
  }

  pplx::task<std::vector<Message>> Channel::get_pinned_messages() const
  {
    return api::channel::get_pinned_messages(m_owner, m_id);
  }

  pplx::task<bool> Channel::pin(Snowflake message_id) const
  {
    return api::channel::add_pinned_message(m_owner, m_id, message_id);
  }

  pplx::task<bool> Channel::unpin(Snowflake message_id) const
  {
    return api::channel::remove_pinned_message(m_owner, m_id, message_id);
  }

  pplx::task<void> Channel::add_recipient(Snowflake user_id, std::string access_token, std::string nickname) const
  {
    return api::channel::group_dm_add_recipient(m_owner, m_id, user_id, access_token, nickname);
  }

  pplx::task<void> Channel::remove_recipient(Snowflake user_id) const
  {
    return api::channel::group_dm_remove_recipient(m_owner, m_id, user_id);
  }
}

#include "guild.h"
#include "channel.h"
#include "connection_state.h"
#include "emoji.h"
#include "member.h"
#include "role.h"
#include "user.h"
#include "voice.h"

namespace discord
{
  game_status::game_status()
  {
    m_type = game_type::Normal;
  }

  game_status::game_status(rapidjson::Value& data)
  {
    set_from_json(m_name, "name", data);
    set_from_json(m_url, "url", data);

    auto found = data.FindMember("type");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_type = static_cast<game_type>(found->value.GetInt());
    }
  }

  presence::presence()
  {
  }

  presence::presence(connection_state* owner, rapidjson::Value& data) : connection_object(owner)
  {
    set_from_json(m_guild_id, "guild_id", data);
    set_from_json(m_status, "status", data);

    auto found = data.FindMember("user");
    if (found != data.MemberEnd())
    {
      m_user = discord::user(owner, data["user"]);
    }

    found = data.FindMember("roles");
    if (found != data.MemberEnd())
    {
      for (auto& role_id : found->value.GetArray())
      {
        m_roles.push_back(snowflake(role_id.GetString()));
      }
    }

    found = data.FindMember("game");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_game = game_status(found->value);
    }
  }

  const user& presence::user() const
  {
    return m_user;
  }

  guild::guild()
  {
    m_afk_timeout = 0;
    m_embed_enabled = false;
    m_verify_level = verify_level::None;
    m_notify_level = notify_level::None;
    m_mfa_level = 0;
    m_large = false;
    m_member_count = 0;
    m_unavailable = false;
    m_empty = true;
  }

  guild::guild(connection_state* owner, rapidjson::Value& data) : identifiable(data["id"]), connection_object(owner)
  {
    set_from_json(m_name, "name", data);
    set_from_json(m_icon, "icon", data);
    set_from_json(m_splash, "splash", data);
    set_from_json(m_owner_id, "owner_id", data);
    set_from_json(m_region, "region", data);
    set_from_json(m_afk_channel_id, "afk_channel_id", data);
    set_from_json(m_afk_timeout, "afk_timeout", data);
    set_from_json(m_embed_enabled, "embed_enabled", data);
    set_from_json(m_embed_channel_id, "embed_channel_id", data);
    set_from_json(m_mfa_level, "mfa_level", data);
    set_from_json(m_joined_at, "joined_at", data);
    set_from_json(m_large, "large", data);
    set_from_json(m_member_count, "member_count", data);
    set_from_json(m_unavailable, "unavailable", data);

    auto found = data.FindMember("verification_level");
    if (found != data.MemberEnd())
    {
      m_verify_level = static_cast<verify_level>(found->value.GetInt());
    }

    found = data.FindMember("default_message_notifications");
    if (found != data.MemberEnd())
    {
      m_notify_level = static_cast<notify_level>(found->value.GetInt());
    }
    

    found = data.FindMember("roles");
    if (found != data.MemberEnd())
    {
      for (auto& role_data : found->value.GetArray())
      {
        role guild_role(role_data);
        m_roles[guild_role.id()] = guild_role;
      }
    }

    found = data.FindMember("emojis");
    if (found != data.MemberEnd())
    {
      for (auto& emoji_data : found->value.GetArray())
      {
        emoji guild_emoji(emoji_data);
        m_emojis[guild_emoji.id()] = guild_emoji;
      }
    }

    found = data.FindMember("features");
    if (found != data.MemberEnd())
    {
      for (auto& guild_feature : found->value.GetArray())
      {
        m_features.push_back(guild_feature.GetString());
      }
    }

    found = data.FindMember("voice_states");
    if (found != data.MemberEnd())
    {
      for (auto& guild_voice_state : found->value.GetArray())
      {
        m_voice_states.push_back(voice_state(guild_voice_state));
      }
    }

    found = data.FindMember("channels");
    if (found != data.MemberEnd())
    {
      for (auto& guild_channel : found->value.GetArray())
      {
        channel chan(owner, m_id, guild_channel);
        m_channels[chan.id()] = chan;
      }
    }

    found = data.FindMember("members");
    if (found != data.MemberEnd())
    {
      for (auto& guild_member : found->value.GetArray())
      {
        member mem(owner, guild_member);
        m_members[mem.user().id()] = mem;
      }
    }

    found = data.FindMember("presences");
    if (found != data.MemberEnd())
    {
      for (auto& guild_presence : found->value.GetArray())
      {
        presence presence(owner, guild_presence);
        m_presences[presence.user().id()] = presence;
      }
    }

    m_empty = false;
  }

  std::string guild::name() const
  {
    return m_name;
  }

  std::vector<emoji> guild::emojis() const
  {
    std::vector<emoji> emojis;

    for (auto& emoji_kv : m_emojis)
    {
      emojis.push_back(emoji_kv.second);
    }

    return emojis;
  }

  uint32_t guild::member_count() const
  {
    return m_member_count;
  }

  const channel& guild::find_channel(snowflake id) const
  {
    if (m_channels.find(id) != std::end(m_channels))
    {
      return m_channels.at(id);
    }

    return channel();
  }

  const member& guild::find_member(snowflake id) const
  {
    if (m_members.find(id) != std::end(m_members))
    {
      return m_members.at(id);
    }

    return member();
  }

  void guild::set_emojis(std::vector<emoji>& emojis)
  {
    for (auto& emoji : emojis)
    {
      m_emojis[emoji.id()] = emoji;
    }
  }

  bool guild::find_emoji(snowflake emoji_id, emoji& dest)
  {
    if (m_emojis.count(emoji_id))
    {
      dest = m_emojis[emoji_id];
      return true;
    }

    return false;
  }

  void guild::set_unavailable(bool value)
  {
    m_unavailable = value;
  }

  void guild::add_channel(channel& chan)
  {
    m_channels[chan.id()] = chan;
  }

  void guild::update_channel(channel& chan)
  {
    m_channels[chan.id()] = chan;
  }

  void guild::remove_channel(channel& chan)
  {
    m_channels.erase(chan.id());
  }

  void guild::add_member(member& mem)
  {
    if (m_members.find(mem.user().id()) == std::end(m_members))
    {
      m_member_count++;
    }

    m_members[mem.user().id()] = mem;
  }

  void guild::update_member(std::vector<snowflake>& role_ids, user& user, std::string nick)
  {
    auto mem_itr = m_members.find(user.id());

    if (mem_itr == std::end(m_members))
    {
      m_member_count++;
      member mem;
      mem.set_roles(role_ids);
      mem.set_user(user);
      mem.set_nick(nick);
      m_members[user.id()] = mem;
    }
    else
    {
      mem_itr->second.set_roles(role_ids);
      mem_itr->second.set_user(user);
      mem_itr->second.set_nick(nick);
    }
  }

  void guild::remove_member(member& mem)
  {
    if (m_members.find(mem.user().id()) != std::end(m_members))
    {
      m_members.erase(mem.user().id());
      m_member_count--;
    }
  }

  void guild::add_role(role& role)
  {
    m_roles[role.id()] = role;
  }

  void guild::update_role(role& role)
  {
    m_roles[role.id()] = role;
  }

  void guild::remove_role(snowflake& role_id)
  {
    m_roles.erase(role_id);
  }

  void guild::update_presence(presence& presence)
  {
    m_presences[presence.user().id()] = presence;
  }

  bool guild::empty()
  {
    return m_empty;
  }
}

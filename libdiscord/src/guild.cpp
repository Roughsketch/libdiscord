#include "guild.h"
#include "channel.h"
#include "connection_state.h"
#include "emoji.h"
#include "member.h"
#include "role.h"
#include "user.h"
#include "voice.h"
#include "api/guild_api.h"

namespace discord
{
  GameStatus::GameStatus()
  {
    m_type = GameType::Normal;
  }

  GameStatus::GameStatus(rapidjson::Value& data)
  {
    set_from_json(m_name, "name", data);
    set_from_json(m_url, "url", data);

    auto found = data.FindMember("type");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_type = static_cast<GameType>(found->value.GetInt());
    }
  }

  Presence::Presence()
  {
  }

  Presence::Presence(ConnectionState* owner, rapidjson::Value& data) : ConnectionObject(owner)
  {
    set_from_json(m_guild_id, "guild_id", data);
    set_from_json(m_status, "status", data);

    auto found = data.FindMember("user");
    if (found != data.MemberEnd())
    {
      m_user = User(owner, data["user"]);
    }

    found = data.FindMember("roles");
    if (found != data.MemberEnd())
    {
      for (const auto& role_id : found->value.GetArray())
      {
        m_roles.emplace_back(role_id.GetString());
      }
    }

    found = data.FindMember("game");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_game = GameStatus(found->value);
    }
  }

  const User& Presence::user() const
  {
    return m_user;
  }

  Guild::Guild()
  {
    m_afk_timeout = 0;
    m_embed_enabled = false;
    m_verify_level = VerificationLevel::None;
    m_notify_level = NotificationLevel::None;
    m_mfa_level = 0;
    m_large = false;
    m_member_count = 0;
    m_unavailable = false;
    m_empty = true;
  }

  Guild::Guild(ConnectionState* owner, rapidjson::Value& data) : Identifiable(data["id"]), ConnectionObject(owner)
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
      m_verify_level = static_cast<discord::VerificationLevel>(found->value.GetInt());
    }

    found = data.FindMember("default_message_notifications");
    if (found != data.MemberEnd())
    {
      m_notify_level = static_cast<discord::NotificationLevel>(found->value.GetInt());
    }
    

    found = data.FindMember("roles");
    if (found != data.MemberEnd())
    {
      for (auto& role_data : found->value.GetArray())
      {
        Role guild_role(role_data);
        m_roles[guild_role.id()] = guild_role;
      }
    }

    found = data.FindMember("emojis");
    if (found != data.MemberEnd())
    {
      for (auto& emoji_data : found->value.GetArray())
      {
        Emoji guild_emoji(emoji_data);
        m_emojis[guild_emoji.id()] = guild_emoji;
      }
    }

    found = data.FindMember("features");
    if (found != data.MemberEnd())
    {
      for (const auto& guild_feature : found->value.GetArray())
      {
        m_features.emplace_back(guild_feature.GetString());
      }
    }

    found = data.FindMember("voice_states");
    if (found != data.MemberEnd())
    {
      for (auto& guild_voice_state : found->value.GetArray())
      {
        m_voice_states.emplace_back(guild_voice_state);
      }
    }

    found = data.FindMember("channels");
    if (found != data.MemberEnd())
    {
      for (auto& guild_channel : found->value.GetArray())
      {
        Channel chan(owner, guild_channel);
        m_channels[chan.id()] = chan;
        m_owner->cache_channel_id(m_id, chan.id());
      }
    }

    found = data.FindMember("members");
    if (found != data.MemberEnd())
    {
      for (auto& guild_member : found->value.GetArray())
      {
        Member mem(owner, guild_member);
        m_members[mem.user().id()] = mem;
      }
    }

    found = data.FindMember("Presences");
    if (found != data.MemberEnd())
    {
      for (auto& guild_Presence : found->value.GetArray())
      {
        Presence Presence(owner, guild_Presence);
        m_presences[Presence.user().id()] = Presence;
      }
    }

    m_empty = false;
  }

  std::string Guild::name() const
  {
    return m_name;
  }

  std::vector<Emoji> Guild::emojis() const
  {
    std::vector<Emoji> emojis;

    for (const auto& emoji_kv : m_emojis)
    {
      emojis.push_back(emoji_kv.second);
    }

    return emojis;
  }

  uint32_t Guild::member_count() const
  {
    return m_member_count;
  }

  std::string Guild::region() const
  {
    return m_region;
  }

  Snowflake Guild::afk_channel_id() const
  {
    return m_afk_channel_id;
  }

  uint32_t Guild::afk_timeout() const
  {
    return m_afk_timeout;
  }

  Snowflake Guild::owner_id() const
  {
    return m_owner_id;
  }

  VerificationLevel Guild::verify_level() const
  {
    return m_verify_level;
  }

  NotificationLevel Guild::notify_level() const
  {
    return m_notify_level;
  }

  std::vector<Channel> Guild::channels() const
  {
    std::vector<Channel> values;

    std::transform(std::begin(m_channels), std::end(m_channels), std::back_inserter(values),
      [](auto& pair) { return pair.second; });

    return values;
  }

  std::vector<uint64_t> Guild::channel_ids() const
  {
    std::vector<uint64_t> keys;

    std::transform(std::begin(m_channels), std::end(m_channels), std::back_inserter(keys),
      [](auto& pair) { return pair.first; });

    return keys;
  }

  std::unique_ptr<Channel> Guild::find_channel(Snowflake id) const
  {
    if (m_channels.find(id) != std::end(m_channels))
    {
      return std::make_unique<Channel>(m_channels.at(id));
    }

    return std::make_unique<Channel>();
  }

  std::unique_ptr<Channel> Guild::find_channel(std::string name) const
  {
    for (const auto& pair : m_channels)
    {
      if (pair.second.name() == name)
      {
        return std::make_unique<Channel>(pair.second);
      }
    }

    return std::make_unique<Channel>();
  }

  std::unique_ptr<Member> Guild::find_member(Snowflake id) const
  {
    if (m_members.find(id) != std::end(m_members))
    {
      return std::make_unique<Member>(m_members.at(id));
    }

    return std::make_unique<Member>();
  }

  void Guild::set_emojis(std::vector<Emoji>& emojis)
  {
    for (const auto& emoji : emojis)
    {
      m_emojis[emoji.id()] = emoji;
    }
  }

  bool Guild::find_emoji(Snowflake emoji_id, Emoji& dest)
  {
    if (m_emojis.count(emoji_id))
    {
      dest = m_emojis[emoji_id];
      return true;
    }

    return false;
  }

  bool Guild::find_emoji(std::string name, Emoji& dest)
  {
    for (const auto& pair : m_emojis)
    {
      if (pair.second.name() == name)
      {
        dest = pair.second;
        return true;
      }
    }

    return false;
  }

  void Guild::set_unavailable(bool unavailable)
  {
    m_unavailable = unavailable;
  }

  void Guild::add_channel(Channel& chan)
  {
    m_channels[chan.id()] = chan;
  }

  void Guild::update_channel(Channel& chan)
  {
    m_channels[chan.id()] = chan;
  }

  void Guild::remove_channel(Channel& chan)
  {
    m_channels.erase(chan.id());
  }

  void Guild::add_member(Member& mem)
  {
    if (m_members.find(mem.user().id()) == std::end(m_members))
    {
      m_member_count++;
    }

    m_members[mem.user().id()] = mem;
  }

  void Guild::update_member(std::vector<Snowflake>& role_ids, User& user, std::string nick)
  {
    auto mem_itr = m_members.find(user.id());

    if (mem_itr == std::end(m_members))
    {
      m_member_count++;
      Member mem;
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

  void Guild::remove_member(Member& mem)
  {
    if (m_members.find(mem.user().id()) != std::end(m_members))
    {
      m_members.erase(mem.user().id());
      m_member_count--;
    }
  }

  void Guild::add_role(Role& role)
  {
    m_roles[role.id()] = role;
  }

  void Guild::update_role(Role& role)
  {
    m_roles[role.id()] = role;
  }

  void Guild::remove_role(Snowflake& role_id)
  {
    m_roles.erase(role_id);
  }

  void Guild::update_presence(Presence& presence)
  {
    m_presences[presence.user().id()] = presence;
  }

  bool Guild::empty() const
  {
    return m_empty;
  }

  pplx::task<Guild> Guild::remove() const
  {
    return api::guild::remove(m_owner, m_id);
  }

  pplx::task<Channel> Guild::create_text_channel(std::string name, std::vector<Overwrite> permission_overwrites) const
  {
    return api::guild::create_text_channel(m_owner, m_id, name, permission_overwrites);
  }

  pplx::task<Channel> Guild::create_voice_channel(std::string name, uint32_t bitrate, uint32_t user_limit, std::vector<Overwrite> permission_overwrites) const
  {
    return api::guild::create_voice_channel(m_owner, m_id, name, bitrate, user_limit, permission_overwrites);
  }

  pplx::task<std::vector<Channel>> Guild::modify_channel_positions(const std::map<Snowflake, uint32_t>& positions) const
  {
    return api::guild::modify_channel_positions(m_owner, m_id, positions);
  }

  pplx::task<Member> Guild::get_member(Snowflake user_id) const
  {
    return api::guild::get_member(m_owner, m_id, user_id);
  }

  pplx::task<std::vector<Member>> Guild::get_members(uint32_t limit, Snowflake after) const
  {
    return api::guild::get_members(m_owner, m_id, limit, after);
  }

  pplx::task<bool> Guild::add_member(Snowflake user_id, std::string access_token, std::string nick, std::vector<Role> roles, bool muted, bool deafened) const
  {
    return api::guild::add_member(m_owner, m_id, user_id, access_token, nick, roles, muted, deafened);
  }

  pplx::task<bool> Guild::modify_member(Snowflake user_id, std::string nick, std::vector<Role> roles, bool muted, bool deafened, Snowflake channel_id) const
  {
    return api::guild::modify_member(m_owner, m_id, user_id, nick, roles, muted, deafened, channel_id);
  }

  pplx::task<bool> Guild::set_nickname(std::string nick) const
  {
    return api::guild::set_nickname(m_owner, m_id, nick);
  }

  pplx::task<bool> Guild::add_member_role(Snowflake user_id, Snowflake role_id) const
  {
    return api::guild::add_member_role(m_owner, m_id, user_id, role_id);
  }

  pplx::task<bool> Guild::remove_member_role(Snowflake user_id, Snowflake role_id) const
  {
    return api::guild::remove_member_role(m_owner, m_id, user_id, role_id);
  }

  pplx::task<bool> Guild::kick(Snowflake user_id) const
  {
    return api::guild::remove_member(m_owner, m_id, user_id);
  }

  pplx::task<std::vector<User>> Guild::get_bans() const
  {
    return api::guild::get_bans(m_owner, m_id);
  }

  pplx::task<bool> Guild::ban(Snowflake user_id, uint32_t delete_x_days) const
  {
    return api::guild::ban(m_owner, m_id, user_id, delete_x_days);
  }

  pplx::task<bool> Guild::unban(Snowflake user_id) const
  {
    return api::guild::unban(m_owner, m_id, user_id);
  }

  pplx::task<std::vector<Role>> Guild::get_roles() const
  {
    return api::guild::get_roles(m_owner, m_id);
  }

  pplx::task<Role> Guild::create_role(std::string name, Permission permissions, uint32_t rgb_color, bool hoist, bool mentionable) const
  {
    return api::guild::create_role(m_owner, m_id, name, permissions, rgb_color, hoist, mentionable);
  }

  pplx::task<std::vector<Role>> Guild::modify_role_positions(const std::map<Snowflake, uint32_t>& positions) const
  {
    return api::guild::modify_role_positions(m_owner, m_id, positions);
  }

  pplx::task<Role> Guild::modify_role(Snowflake role_id, std::string name, Permission permissions, uint32_t rgb_color, bool hoist, bool mentionable) const
  {
    return api::guild::modify_role(m_owner, m_id, role_id, name, permissions, rgb_color, hoist, mentionable);
  }

  pplx::task<bool> Guild::remove_role(Snowflake role_id) const
  {
    return api::guild::remove_role(m_owner, m_id, role_id);
  }

  pplx::task<uint32_t> Guild::get_prune_count(uint32_t days) const
  {
    return api::guild::get_prune_count(m_owner, m_id, days);
  }

  pplx::task<uint32_t> Guild::prune_users(uint32_t days) const
  {
    return api::guild::prune(m_owner, m_id, days);
  }

  pplx::task<std::vector<VoiceRegion>> Guild::get_voice_regions() const
  {
    return api::guild::get_voice_regions(m_owner, m_id);
  }

  pplx::task<std::vector<Integration>> Guild::get_integrations() const
  {
    return api::guild::get_integrations(m_owner, m_id);
  }

  pplx::task<bool> Guild::create_integration(std::string type, Snowflake integration_id) const
  {
    return api::guild::create_integration(m_owner, m_id, type, integration_id);
  }

  pplx::task<bool> Guild::modify_integration(Snowflake integration_id, uint32_t expire_behavior, uint32_t expire_grace_period, bool enable_emoticons) const
  {
    return api::guild::modify_integration(m_owner, m_id, integration_id, expire_behavior, expire_grace_period, enable_emoticons);
  }

  pplx::task<bool> Guild::remove_integration(Snowflake integration_id) const
  {
    return api::guild::remove_integration(m_owner, m_id, integration_id);
  }

  pplx::task<bool> Guild::sync_integration(Snowflake integration_id) const
  {
    return api::guild::sync_integration(m_owner, m_id, integration_id);
  }
}

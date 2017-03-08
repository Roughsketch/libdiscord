#include "guild.h"
#include "bot.h"
#include "channel.h"
#include "emoji.h"
#include "event/guild_event.h"
#include "member.h"
#include "role.h"
#include "user.h"
#include "voice.h"

namespace discord
{
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
  }

  guild::guild(std::string token, rapidjson::Value& data) : identifiable(data["id"]), m_token(token)
  {
    m_name = data["name"].GetString();
    m_icon = data["icon"].GetString();
    m_splash = data["splash"].GetString();
    m_owner_id = snowflake(data["owner_id"].GetString());
    m_region = data["region"].GetString();
    m_afk_channel_id = snowflake(data["afk_channel_id"].GetString());
    m_afk_timeout = data["afk_timeout"].GetInt();
    m_embed_enabled = data["embed_enabled"].GetBool();
    m_embed_channel_id = snowflake(data["embed_channel_id"].GetString());
    m_verify_level = static_cast<verify_level>(data["verification_level"].GetInt());
    m_notify_level = static_cast<notify_level>(data["default_message_notifications"].GetInt());
    m_mfa_level = data["mfa_level"].GetInt();
    m_joined_at = data["joined_at"].GetString();
    m_large = data["large"].GetBool();
    m_member_count = data["member_count"].GetInt();

    for (auto& guild_role : data["roles"].GetArray())
    {
      m_roles.push_back(role(guild_role));
    }

    for (auto& guild_emoji : data["emojis"].GetArray())
    {
      m_emojis.push_back(emoji(guild_emoji));
    }

    for (auto& guild_feature : data["features"].GetArray())
    {
      m_features.push_back(guild_feature.GetString());
    }

    for (auto& guild_voice_state : data["voice_states"].GetArray())
    {
      m_voice_states.push_back(voice_state(guild_voice_state));
    }

    for (auto& guild_channel : data["channels"].GetArray())
    {
      m_channels.push_back(channel(token, m_id, guild_channel));
    }

    for (auto& guild_member : data["members"].GetArray())
    {
      member mem(token, guild_member);

      m_members[mem.user().id()] = mem;
    }

    for (auto& guild_presence : data["presences"].GetArray())
    {
      presence_event presence(token, guild_presence);
      m_presences[presence.user().id()] = presence;
    }

    m_unavailable = data["unavailable"].GetBool();
  }

  std::string guild::name() const
  {
    return m_name;
  }

  std::vector<emoji> guild::emojis() const
  {
    return m_emojis;
  }

  void guild::set_emojis(std::vector<emoji>& emojis)
  {
  }

  void guild::set_unavailable(bool value)
  {
  }

  void guild::add_channel(channel chan)
  {
  }

  void guild::update_channel(channel chan)
  {
  }

  void guild::remove_channel(channel chan)
  {
  }

  void guild::add_member(member mem)
  {
  }

  void guild::update_member(std::vector<snowflake> role_ids, user user, std::string nick)
  {
  }

  void guild::remove_member(member mem)
  {
  }

  void guild::add_role(role role)
  {
  }

  void guild::update_role(role role)
  {
  }

  void guild::remove_role(role role)
  {
  }

  void guild::update_presence(presence_event presence)
  {
  }
}

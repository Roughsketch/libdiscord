#pragma once

#include <unordered_map>

#include "common.h"
#include "user.h"
#include "voice.h"

namespace discord
{
  class user;
  class bot;
  class channel;
  class emoji;
  class member;
  class presence_event;
  class role;

  enum class verify_level
  {
    None,
    Low,
    Medium,
    High
  };

  /** Represents the default notification level of a Guild. */
  enum class notify_level
  {
    None,
    MentionOnly
  };

  /** Enumeration of types that a Game object can be.
  Current only valid options are Stream and Normal. */
  enum game_type : uint8_t
  {
    Normal = 0,
    Stream
  };

  class game_status
  {
    std::string m_name;
    game_type m_type;
    std::string m_url;
  public:
    game_status();
    explicit game_status(rapidjson::Value& data);
    
  };

  class presence
  {
    user m_user;
    std::vector<snowflake> m_roles;
    game_status m_game;
    snowflake m_guild_id;
    std::string m_status;

  public:
    presence();
    explicit presence(const std::string& token, rapidjson::Value& data);

    const user& user() const;
  };

  class guild : public identifiable
  {
    std::string m_token;

    std::string m_name;
    std::string m_icon;
    std::string m_splash;
    snowflake m_owner_id;
    std::string m_region;
    snowflake m_afk_channel_id;
    uint32_t m_afk_timeout;
    bool m_embed_enabled;
    snowflake m_embed_channel_id;
    verify_level m_verify_level;
    notify_level m_notify_level;
    std::unordered_map<uint64_t, role> m_roles;
    std::unordered_map<uint64_t, emoji> m_emojis;
    std::vector<std::string> m_features;
    uint32_t m_mfa_level;
    std::string m_joined_at;
    bool m_large;
    uint32_t m_member_count;
    std::vector<voice_state> m_voice_states;
    std::unordered_map<uint64_t, member> m_members;
    std::unordered_map<uint64_t, channel> m_channels;
    std::unordered_map<uint64_t, presence> m_presences;

    bool m_unavailable;
  public:
    guild();
    explicit guild(const std::string& token, rapidjson::Value& data);

    std::string name() const;
    std::vector<emoji> emojis() const;
    void set_emojis(std::vector<emoji>& emojis);

    void set_unavailable(bool value);

    void add_channel(channel& chan);
    void update_channel(channel& chan);
    void remove_channel(channel& chan);

    void add_member(member& mem);
    void update_member(std::vector<snowflake>& role_ids, user& user, std::string nick);
    void remove_member(member& mem);

    void add_role(role& role);
    void update_role(role& role);
    void remove_role(role& role);

    void update_presence(presence& presence);
  };
}
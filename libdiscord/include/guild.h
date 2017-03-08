#pragma once

#include <unordered_map>

#include "common.h"

namespace discord
{
  class user;
  class bot;
  class channel;
  class emoji;
  class member;
  class presence_event;
  class role;
  class voice_state;

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
    std::vector<role> m_roles;
    std::vector<emoji> m_emojis;
    std::vector<std::string> m_features;
    uint32_t m_mfa_level;
    std::string m_joined_at;
    bool m_large;
    uint32_t m_member_count;
    std::vector<voice_state> m_voice_states;
    std::unordered_map<uint64_t, member> m_members;
    std::vector<channel> m_channels;
    std::unordered_map<uint64_t, presence_event> m_presences;

    bool m_unavailable;
  public:
    guild();
    explicit guild(std::string token, rapidjson::Value& data);

    std::string name() const;
    std::vector<emoji> emojis() const;
    void set_emojis(std::vector<emoji>& emojis);

    void set_unavailable(bool value);

    void add_channel(channel chan);
    void update_channel(channel chan);
    void remove_channel(channel chan);

    void add_member(member mem);
    void update_member(std::vector<snowflake> role_ids, user user, std::string nick);
    void remove_member(member mem);

    void add_role(role role);
    void update_role(role role);
    void remove_role(role role);

    void update_presence(presence_event presence);
  };
}
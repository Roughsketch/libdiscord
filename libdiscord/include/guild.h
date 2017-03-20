#pragma once

#include <unordered_map>

#include "common.h"
#include "connection_object.h"
#include "emoji.h"
#include "member.h"
#include "role.h"
#include "user.h"
#include "voice.h"

namespace discord
{
  class channel;
  class connection_state;
  class presence_event;

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

  class presence : connection_object
  {
    user m_user;
    std::vector<snowflake> m_roles;
    game_status m_game;
    snowflake m_guild_id;
    std::string m_status;

  public:
    presence();
    explicit presence(connection_state* owner, rapidjson::Value& data);

    const user& user() const;
  };

  class guild : public identifiable, public connection_object
  {
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

    bool m_empty;
  public:
    guild();
    explicit guild(connection_state* owner, rapidjson::Value& data);

    std::string name() const;
    std::vector<emoji> emojis() const;
    uint32_t member_count() const;

    std::vector<channel> channels() const;
    std::vector<uint64_t> channel_ids() const;
    channel find_channel(snowflake id) const;
    member find_member(snowflake id) const;

    void set_emojis(std::vector<emoji>& emojis);
    bool find_emoji(snowflake emoji_id, emoji& dest);
    bool find_emoji(std::string name, emoji& dest);

    void set_unavailable(bool value);

    void add_channel(channel& chan);
    void update_channel(channel& chan);
    void remove_channel(channel& chan);

    void add_member(member& mem);
    void update_member(std::vector<snowflake>& role_ids, user& user, std::string nick);
    void remove_member(member& mem);

    void add_role(role& role);
    void update_role(role& role);
    void remove_role(snowflake& role_id);

    void update_presence(presence& presence);

    /** Whether or not this object is considered empty.
     *
     * @return True if this guild has no meaningful data.
     */
    bool empty();
  };
}

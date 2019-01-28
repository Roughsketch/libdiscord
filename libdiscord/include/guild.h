#pragma once

#include <cpprest/http_client.h>
#include <unordered_map>

#include "common.h"
#include "connection_object.h"
#include "emoji.h"
#include "member.h"
#include "role.h"
#include "user.h"
#include "voice.h"
#include "integration.h"
#include "channel.h"

namespace discord
{
  class Channel;
  class ConnectionState;
  class PresenceEvent;

  enum class VerificationLevel
  {
    None,
    Low,
    Medium,
    High
  };

  /** Represents the default notification level of a Guild. */
  enum class NotificationLevel
  {
    None,
    MentionOnly
  };

  /** Enumeration of types that a Game object can be.
  Current only valid options are Stream and Normal. */
  enum GameType : uint8_t
  {
    Normal = 0,
    Stream
  };

  class GameStatus
  {
    std::string m_name;
    GameType m_type;
    std::string m_url;
  public:
    GameStatus();
    explicit GameStatus(rapidjson::Value& data);
    
  };

  class Presence : ConnectionObject
  {
    User m_user;
    std::vector<Snowflake> m_roles;
    GameStatus m_game;
    Snowflake m_guild_id;
    std::string m_status;

  public:
    Presence();
    explicit Presence(ConnectionState* owner, rapidjson::Value& data);

    const User& user() const;
  };

  class Guild : public Identifiable, public ConnectionObject
  {
    std::string m_name;
    std::string m_icon;
    std::string m_splash;
    Snowflake m_owner_id;
    std::string m_region;
    Snowflake m_afk_channel_id;
    uint32_t m_afk_timeout;
    bool m_embed_enabled;
    Snowflake m_embed_channel_id;
    VerificationLevel m_verify_level;
    NotificationLevel m_notify_level;
    std::unordered_map<uint64_t, Role> m_roles;
    std::unordered_map<uint64_t, Emoji> m_emojis;
    std::vector<std::string> m_features;
    uint32_t m_mfa_level;
    std::string m_joined_at;
    bool m_large;
    uint32_t m_member_count;
    std::vector<VoiceState> m_voice_states;
    std::unordered_map<uint64_t, Member> m_members;
    std::unordered_map<uint64_t, Channel> m_channels;
    std::unordered_map<uint64_t, Presence> m_presences;
    bool m_unavailable;

    bool m_empty;
  public:
    Guild();
    explicit Guild(ConnectionState* owner, rapidjson::Value& data);

    /** Gets the name of this guild.
     *
     * @return The name of this guild.
     */
    std::string name() const;

    /** Gets a list of custom emojis in this guild.
    *
    * @return A list of custom emojis in this guild.
    */
    std::vector<Emoji> emojis() const;

    /** Gets amount of members that have been seen in this guild.
    *
    * @return Current guild member count.
    */
    uint32_t member_count() const;

    /** Gets the region this guild is hosted in.
    *
    * @return The region this guild is hosted in.
    */
    std::string region() const;

    /** Gets the id of the channel people are sent to when marked AFK.
    *
    * @return The AFK channel's id.
    */
    Snowflake afk_channel_id() const;

    /** Gets the amount of time to wait until a user is marked AFK.
    *
    * @return AFK timeout in seconds.
    */
    uint32_t afk_timeout() const;

    /** Gets the guild owner's id.
    *
    * @return Guild owner id.
    */
    Snowflake owner_id() const;

    /** Gets the verification level required to join the guild.
    *
    * @return Guild's verification level.
    */
    VerificationLevel verify_level() const;

    /** Gets the notification level for this guild.
    *
    * @return Guild's notification level.
    */
    NotificationLevel notify_level() const;

    /** Gets a list of channels in this guild.
    *
    * @return A list of channels in this guild.
    */
    std::vector<Channel> channels() const;

    /** Gets a list of channel ids in this guild.
    *
    * @return A list of channel ids for this guild.
    */
    std::vector<uint64_t> channel_ids() const;

    /** Finds a channel based on its id.
    *
    * @param id The id of the channel to find.
    * @return The channel that was found, or an empty channel if not found.
    */
    std::unique_ptr<Channel> find_channel(Snowflake id) const;

    /** Finds a channel based on its name.
    *
    * @param name The name of the channel to find.
    * @return The channel that was found, or an empty channel if not found.
    */
    std::unique_ptr<Channel> find_channel(std::string name) const;

    /** Finds a member based on their id.
    *
    * @param id The id of the member to find.
    * @return The member that was found, or an empty member if not found.
    */
    std::unique_ptr<Member> find_member(Snowflake id) const;

    /** Sets this guild's list of emojis. This will overwrite the current list.
    *
    * @param emojis The emojis to set for this guild.
    */
    void set_emojis(std::vector<Emoji>& emojis);

    /** Find an emoji by its id.
    *
    * @param emoji_id The id of the emoji to find.
    * @param dest Destination for the emoji that was found.
    * @return True if the emoji was found.
    */
    bool find_emoji(Snowflake emoji_id, Emoji& dest);

    /** Find an emoji by its name.
    *
    * @param name The name of the emoji to find.
    * @param dest Destination for the emoji that was found.
    * @return True if the emoji was found.
    */
    bool find_emoji(std::string name, Emoji& dest);

    /** Sets this guild as currently unavailable.
    *
    * @param unavailable Whether or not this guild is unavailable
    */
    void set_unavailable(bool unavailable);

    /** Adds a channel to this guild. Does not create the channel given.
    *
    * @param chan The channel to be added.
    */
    void add_channel(Channel& chan);

    /** Updates a channel in this guild. Does not modify the channel given.
    *
    * @param chan The channel that was updated.
    */
    void update_channel(Channel& chan);

    /** Removes a channel from this guild. Does not delete the channel given.
    *
    * @param chan The channel to remove.
    */
    void remove_channel(Channel& chan);

    /** Adds a member to this guild's list of members.
    *
    * @param mem The member to add to the list.
    */
    void add_member(Member& mem);

    /** Updated a member's information in the guild's list of members.
    *
    * @param role_ids The roles this user should have.
    * @param user The user object for this member.
    * @param nick The nickname for this member.
    */
    void update_member(std::vector<Snowflake>& role_ids, User& user, std::string nick);

    /** Removes a member from the guild's list of members. Does not kick the member given.
    *
    * @param mem The member to remove from the guild's member list.
    */
    void remove_member(Member& mem);

    /** Adds a role to the guild.
    *
    * @param role The role to add.
    */
    void add_role(Role& role);

    /** Updates a role in the guild.
    *
    * @param role The role to update.
    */
    void update_role(Role& role);

    /** Removes a role from the guild.
    *
    * @param role_id The role to remove.
    */
    void remove_role(Snowflake& role_id);

    /** Updates the presence of a member.
    *
    * @param presence The presence information.
    */
    void update_presence(Presence& presence);

    /** Whether or not this object is considered empty.
     *
     * @return True if this guild has no meaningful data.
     */
    bool empty() const;

    // ///////////////// //
    // Start API Methods //
    // ///////////////// //

    /** Modify the attributes of a guild.
    *
    * @param guild A guild whose attributes will be sent.
    * @return The modified guild object.
    */
    //  TODO: Change parameters
    pplx::task<Guild> modify(discord::Guild guild);

    /** Removes this guild entirely. Cannot be undone.
    *
    * @return The guild that was removed.
    */
    pplx::task<Guild> remove() const;

    /** Create a new text channel in a guild.
    *
    * @param name The name of the new channel.
    * @param permission_overwrites An array of permission overwrites to assign to this channel.
    * @return The channel that was created.
    */
    pplx::task<Channel> create_text_channel(std::string name, std::vector<Overwrite> permission_overwrites = {}) const;

    /** Create a new voice channel in a guild.
    *
    * @param name The name of the new channel.
    * @param bitrate The bitrate for this voice channel.
    * @param user_limit The user limit for this voice channel.
    * @param permission_overwrites An array of permission overwrites to assign to this channel.
    * @return The channel that was created.
    */
    pplx::task<Channel> create_voice_channel(std::string name, uint32_t bitrate = 96000, uint32_t user_limit = 99, std::vector<Overwrite> permission_overwrites = {}) const;

    /** Sets several channel positions at once.
    *
    * @param positions A mapping of channel ids to positions which will be set.
    * @return The list of channels in the guild.
    */
    pplx::task<std::vector<discord::Channel>> modify_channel_positions(const std::map<Snowflake, uint32_t>& positions) const;

    /** Get a member of a guild.
    *
    * @param user_id The user id of the member to get.
    * @return The member that was found.
    */
    pplx::task<Member> get_member(Snowflake user_id) const;

    /** Gets a list of members in the guild.
    *
    * @param limit The amount of members to list. Can be between 1-1000 inclusive.
    * @param after The id of the member to search after. Used for pagination.
    * @return A list of members from the guild.
    */
    pplx::task<std::vector<Member>> get_members(uint32_t limit = 1, Snowflake after = 0) const;

    /** Adds a member to a guild. Requires an OAuth2 access token.
    *
    * @param user_id The user to add to the guild.
    * @param access_token The access token that gives this user guilds.join permission.
    * @param nick The nickname to give this user.
    * @param roles The roles to give this user.
    * @param muted Whether or not this user should be muted.
    * @param deafened Whether or not this user should be deaf.
    * @return Success status.
    */
    pplx::task<bool> add_member(Snowflake user_id, std::string access_token, std::string nick = "", std::vector<Role> roles = {}, bool muted = false, bool deafened = false) const;

    /** Modify a member in the guild.
    *
    * @param user_id The user to modify.
    * @param nick The nickname to give the user.
    * @param roles The roles to give the user.
    * @param muted Whether or not this user should be muted.
    * @param deafened Whether or not this user should be deaf.
    * @param channel_id Voice channel to move user to.
    * @return Success status.
    */
    pplx::task<bool> modify_member(Snowflake user_id, std::string nick = "", std::vector<Role> roles = {}, bool muted = false, bool deafened = false, Snowflake channel_id = 0) const;

    /** Set the current user's nickname in this guild.
    *
    * @param nick The nickname to set.
    */
    pplx::task<bool> set_nickname(std::string nick) const;

    /** Add a role to a member.
    *
    * @param user_id The member to add the role to.
    * @param role_id The role to add.
    * @return Success status.
    */
    pplx::task<bool> add_member_role(Snowflake user_id, Snowflake role_id) const;

    /** Remove a role from a member.
    *
    * @param user_id The member to remove the role from.
    * @param role_id The role to remove.
    */
    pplx::task<bool> remove_member_role(Snowflake user_id, Snowflake role_id) const;

    /** Remove a member from a guild.
    *
    * @param user_id The member to remove.
    */
    pplx::task<bool> kick(Snowflake user_id) const;

    /** Get a list of bans that the guild has.
    *
    * @return A list of bans that are currently active in the guild.
    */
    pplx::task<std::vector<User>> get_bans() const;

    /** Bans a member from the guild.
    *
    * @param user_id The member to ban from the guild.
    * @param delete_x_days Number of days worth of messages to delete. Can be from 0-7.
    * @return Success status.
    */
    pplx::task<bool> ban(Snowflake user_id, uint32_t delete_x_days) const;

    /** Unbans a member from the guild.
    *
    * @param user_id The member to unban.
    * @return Success status.
    */
    pplx::task<bool> unban(Snowflake user_id) const;

    /** Gets a list of roles that belong to the guild.
    *
    * @return A list of roles that belong to the guild.
    */
    pplx::task<std::vector<Role>> get_roles() const;

    /** Creates a new role in a guild.
    *
    * @param name The name of the new role.
    * @param permissions permissions for the new role.
    * @param rgb_color The color of the new role.
    * @param hoist Whether or not this role should be hoisted into its own group.
    * @param mentionable Whether or not this role is mentionable.
    * @return The role that was created.
    */
    pplx::task<Role> create_role(std::string name, Permission permissions, uint32_t rgb_color = 0, bool hoist = false, bool mentionable = false) const;

    /** Modifies the raw position of a role.
    *
    * @param positions A map of role ids to positions.
    * @return The list of roles from the server.
    */
    pplx::task<std::vector<Role>> modify_role_positions(const std::map<Snowflake, uint32_t>& positions) const;

    /** Modify the attributes of a role.
    *
    * @param role_id The id of the role to modify.
    * @param name The new name of the role.
    * @param permissions The new permissions for this role.
    * @param rgb_color The new color for this role.
    * @param hoist The new hoist value for this role.
    * @param mentionable The new mentionable value for this role.
    * @return The role that was updated.
    */
    pplx::task<Role> modify_role(Snowflake role_id, std::string name, Permission permissions, uint32_t rgb_color = 0, bool hoist = false, bool mentionable = false) const;

    /** Removes a role from a guild.
    *
    * @param role_id The role to remove from the guild.
    * @return Success status.
    */
    pplx::task<bool> remove_role(Snowflake role_id) const;

    /** Get the amount of users that will be pruned if a prune is run on the guild.
    *
    * @param days The number of days to send to the prune command.
    * @return The amount of members that will be pruned if pruned with the given amount of days.
    */
    pplx::task<uint32_t> get_prune_count(uint32_t days) const;

    /** Prunes members from a guild.
    *
    * @param days The number of days to send to the prune command.
    * @return The amount of members that were pruned.
    */
    pplx::task<uint32_t> prune_users(uint32_t days) const;

    /** Lists the voice regions available.
    *
    * @return A list of voice regions.
    */
    pplx::task<std::vector<VoiceRegion>> get_voice_regions() const;

    /** Lists the invites to this guild.
    *
    * @return A list of invites to this guild.
    */
    //  TODO: Implement
    //pplx::task<std::vector<invite>> get_invites();

    /** Lists the integrations this guild owns.
    *
    * @return A list of integrations that this guild owns.
    */
    pplx::task<std::vector<Integration>> get_integrations() const;

    /** Attaches an Integration object to a guild.
    *
    * @param type The type of the Integration.
    * @param integration_id The Integration to attach.
    * @return Success status.
    */
    pplx::task<bool> create_integration(std::string type, Snowflake integration_id) const;

    /** Modifies the behavior of an Integration.
    *
    * @param integration_id The Integration to modify.
    * @param expire_behavior The behavior when an Integration subscription lapses.
    * @param expire_grace_period The period in seconds where the Integration will ignore lapsed subscriptions.
    * @param enable_emoticons Whether emoticons should be synced.
    * @return Success status.
    */
    pplx::task<bool> modify_integration(Snowflake integration_id, uint32_t expire_behavior, uint32_t expire_grace_period, bool enable_emoticons) const;

    /** Removes an Integration from a guild.
    *
    * @param integration_id The Integration to remove.
    */
    pplx::task<bool> remove_integration(Snowflake integration_id) const;

    /** Syncs an Integration.
    *
    * @param integration_id The Integration to sync.
    */
    pplx::task<bool> sync_integration(Snowflake integration_id) const;
  };
}

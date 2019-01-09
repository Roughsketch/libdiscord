#pragma once

#include <unordered_map>

#include "common.h"

namespace discord
{
  enum event_type;

  class channel;
  class connection_state;
  class emoji;
  class gateway;
  class guild;
  class presence;
  class user;

  class message_event;
  class message_deleted_event;
  class typing_event;

  class bot
  {
    std::unique_ptr<connection_state> m_conn_state;

    std::string m_prefix;

    //  Event callbacks
    std::function<void()> m_on_ready;
    std::function<void(message_event&)> m_on_message;
    std::function<void(message_event&)> m_on_message_edited;
    std::function<void(message_deleted_event&)> m_on_message_deleted;
    std::function<void(emoji&)> m_on_emoji_created;
    std::function<void(emoji&)> m_on_emoji_deleted;
    std::function<void(emoji&)> m_on_emoji_updated;
	std::function<void(guild&)> m_on_guild_created;
    std::function<void(presence&)> m_on_presence;
    std::function<void(typing_event&)> m_on_typing;
    std::unordered_map<std::string, std::function<void(message_event)>> m_commands;

    void on_event(event_type type, rapidjson::Value& data);
  public:
    explicit bot(std::string token, std::string prefix = "", int shards = 1);
    ~bot();

    /** Get the bot's current token. *
     *
     * @return The bot's token with the added "Bot" prepended.
     */
    const std::string& token() const;

    /** Get the bot's user profile.
     *
     * @return The bot's user profile.
     */
    const user& profile() const;

    /** Get a list of guilds that this bot is currently in.
     *
     * @return A list of guilds this bot is in.
     */
    std::vector<guild> guilds() const;

    /** Get a list of channels that this bot can currently see.
     *
     * @return A list of channels this bot can see.
     */
    std::vector<channel> channels() const;

    /** Find a guild by its id.
     *
     * @param id The guild's id.
     * @return The guild that was found or an empty guild if not found.
     */
    std::unique_ptr<guild> find_guild(snowflake id) const;

    /** Triggered when the bot is finished receiving the READY packet from the gateway.
     *
     * @param callback The callback to trigger.
     */
    void on_ready(std::function<void()> callback);

    /** Triggered when a message is sent to any channel.
     *
     * @param callback The callback to trigger.
     */
    void on_message(std::function<void(message_event&)> callback);

    /** Triggered when a message is edited.
     *
     * @param callback The callback to trigger.
     */
    void on_message_edited(std::function<void(message_event&)> callback);

    /** Triggered when a message is deleted.
     *
     * @param callback The callback to trigger.
     */
    void on_message_deleted(std::function<void(message_deleted_event&)> callback);

    /** Triggered when an emoji is added to a guild.
     *
     * @param callback The callback to trigger.
     */
    void on_emoji_created(std::function<void(emoji&)> callback);

    /** Triggered when an emoji is removed from a guild.
     *
     * @param callback The callback to trigger.
     */
    void on_emoji_deleted(std::function<void(emoji&)> callback);

    /** Triggered when an emoji is updated in a guild.
     *
     * @param callback The callback to trigger.
     */
    void on_emoji_updated(std::function<void(emoji&)> callback);

	/** Triggered when a guild created event is received.
	 *
	 * @param callback The callback to trigger.
	 */
	void on_guild_created(std::function<void(guild&)> callback);

    /** Triggered when a presence event is received.
     *
     * @param callback The callback to trigger.
     */
    void on_presence(std::function<void(presence&)> callback);

    /** Triggered when a typing event is received.
     *
     * @param callback The callback to trigger.
     */
    void on_typing(std::function<void(typing_event&)> callback);

    /** Adds a command to the bot that will fire when the bot's prefix is followed by the given name.
     *
     * @param name The name of the command to add.
     * @param callback The callback to trigger when the command is received.
     */
    void add_command(std::string name, std::function<void(message_event)> callback);

    /** Runs the bot.
     *
     * @param async Whether to run the bot asynchronously or not. Default will block.
     */
    void run(bool async = false) const;
  };
}

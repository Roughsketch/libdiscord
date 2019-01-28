#pragma once

#include <unordered_map>

#include "common.h"

namespace discord
{
  enum EventType;

  class Channel;
  class ConnectionState;
  class Emoji;
  class Gateway;
  class Guild;
  class Presence;
  class User;

  class MessageEvent;
  class MessageDeletedEvent;
  class TypingEvent;

  class Bot
  {
    std::unique_ptr<ConnectionState> m_conn_state;

    std::string m_prefix;

    //  Event callbacks
    std::function<void()> m_on_ready;
    std::function<void(MessageEvent&)> m_on_message;
    std::function<void(MessageEvent&)> m_on_message_edited;
    std::function<void(MessageDeletedEvent&)> m_on_message_deleted;
    std::function<void(Emoji&)> m_on_emoji_created;
    std::function<void(Emoji&)> m_on_emoji_deleted;
    std::function<void(Emoji&)> m_on_emoji_updated;
	std::function<void(Guild&)> m_on_guild_created;
    std::function<void(Presence&)> m_on_presence;
    std::function<void(TypingEvent&)> m_on_typing;
    std::unordered_map<std::string, std::function<void(MessageEvent)>> m_commands;

    void on_event(EventType type, rapidjson::Value& data);
  public:
    explicit Bot(std::string token, std::string prefix = "", int shards = 1);
    ~Bot();

    /** Get the Bot's current token. *
     *
     * @return The Bot's token with the added "Bot" prepended.
     */
    const std::string& token() const;

    /** Get the Bot's user profile.
     *
     * @return The Bot's user profile.
     */
    const User& profile() const;

    /** Get a list of guilds that this Bot is currently in.
     *
     * @return A list of guilds this Bot is in.
     */
    std::vector<Guild> guilds() const;

    /** Get a list of channels that this Bot can currently see.
     *
     * @return A list of channels this Bot can see.
     */
    std::vector<Channel> channels() const;

    /** Find a guild by its id.
     *
     * @param id The guild's id.
     * @return The guild that was found or an empty guild if not found.
     */
    std::unique_ptr<Guild> find_guild(Snowflake id) const;

    /** Triggered when the Bot is finished receiving the READY packet from the gateway.
     *
     * @param callback The callback to trigger.
     */
    void on_ready(std::function<void()> callback);

    /** Triggered when a message is sent to any channel.
     *
     * @param callback The callback to trigger.
     */
    void on_message(std::function<void(MessageEvent&)> callback);

    /** Triggered when a message is edited.
     *
     * @param callback The callback to trigger.
     */
    void on_message_edited(std::function<void(MessageEvent&)> callback);

    /** Triggered when a message is deleted.
     *
     * @param callback The callback to trigger.
     */
    void on_message_deleted(std::function<void(MessageDeletedEvent&)> callback);

    /** Triggered when an emoji is added to a guild.
     *
     * @param callback The callback to trigger.
     */
    void on_emoji_created(std::function<void(Emoji&)> callback);

    /** Triggered when an emoji is removed from a guild.
     *
     * @param callback The callback to trigger.
     */
    void on_emoji_deleted(std::function<void(Emoji&)> callback);

    /** Triggered when an emoji is updated in a guild.
     *
     * @param callback The callback to trigger.
     */
    void on_emoji_updated(std::function<void(Emoji&)> callback);

	/** Triggered when a guild created event is received.
	 *
	 * @param callback The callback to trigger.
	 */
	void on_guild_created(std::function<void(Guild&)> callback);

    /** Triggered when a presence event is received.
     *
     * @param callback The callback to trigger.
     */
    void on_presence(std::function<void(Presence&)> callback);

    /** Triggered when a typing event is received.
     *
     * @param callback The callback to trigger.
     */
    void on_typing(std::function<void(TypingEvent&)> callback);

    /** Adds a command to the Bot that will fire when the Bot's prefix is followed by the given name.
     *
     * @param name The name of the command to add.
     * @param callback The callback to trigger when the command is received.
     */
    void add_command(std::string name, std::function<void(MessageEvent)> callback);

    /** Runs the Bot.
     *
     * @param async Whether to run the Bot asynchronously or not. Default will block.
     */
    void run(bool async = false) const;
  };
}

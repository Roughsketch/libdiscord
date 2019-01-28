#pragma once

#include <cpprest/http_client.h>

#include "common.h"
#include "embed.h"
#include "permission.h"
#include "user.h"

namespace discord
{
  class Emoji;
  class Message;

  enum class SearchMethod
  {
    None,
    Before,
    After,
    Around
  };

  class Overwrite : public Identifiable
  {
    std::string m_type;
    Permission m_allow;
    Permission m_deny;
  public:
    Overwrite();
    explicit Overwrite(rapidjson::Value& data);

    /** Get the type of this overwrite (either "role" or "member")

    @return The type of this overwrite.
    */
    std::string type() const;

    /** Get the permissions that are allowed.

    @return The permissions that are allowed.
    */
	Permission allow() const;

    /** Get the permissions that are denied.

    @return The permissions that are denied.
    */
	Permission deny() const;
  };

  /** Types of channels that can be sent by the API. */
  enum ChannelType : uint8_t
  {
    Text = 0,
    Private,
    Voice,
    Group
  };

  class Channel : public Identifiable, public ConnectionObject
  {
    //  Constants
    static const uint32_t MinNameSize = 2;
    static const uint32_t MaxNameSize = 100;
    static const uint32_t MaxTopicSize = 1024;
    static const uint32_t MinBitRate = 8000;
    static const uint32_t MaxBitRate = 96000;
    static const uint32_t MaxVIPBitRate = 128000;
    static const uint32_t MaxUserLimit = 99;

    //  All Channels
    Snowflake m_last_message_id;

    //  Guild Channels
    std::string m_name;
    ChannelType m_type;
    int32_t m_position;
    std::vector<Overwrite> m_permission_overwrites;
    std::string m_topic;
    uint32_t m_bitrate;
    uint32_t m_user_limit;

    //  DM Specific
    User m_recipient;

    bool m_is_dm;
    bool m_empty;
  public:
    Channel();
    explicit Channel(ConnectionState* owner, rapidjson::Value& data);

    /** Gets the name of the channel.
     *
     * @return Name of the channel.
     */
    std::string name() const;

    /** Gets the type of the channel.
     *
     *  One of:
     *    Text
     *    Private
     *    Voice
     *    Group
     *    
     * @return The type of channel this is.
     */
    ChannelType type() const;

    /** Gets the position this channel is in the guild list.
     *
     * @return The channel's position.
     */
    int32_t position() const;
    
    /** Gets the bitrate of the current channel.
     *
     * @return The bitrate of this channel, or 0 if not a voice channel.
     */
    uint32_t bitrate() const;

    /** Gets the user limit of the current channel.
     *
     * @return The user limit of this channel, or 0 if not a voice channel.
     */
    uint32_t user_limit() const;

    /** Gets the current set topic for the current channel.
     *
     * @return The channel's topic.
     */
    std::string topic() const;

    /** Gets the guild that owns this channel.
     *
     * @return The guild that owns this channel, or an empty guild if the channel is a DM.
     */
    std::unique_ptr<Guild> guild() const;

    /** Whether or not this object is considered empty.
     *
     * @return True if this channel has no meaningful data.
     */
    bool empty() const;

    // ///////////////// //
    // Start API Methods //
    // ///////////////// //

    /** Modify a text channel's attributes.
    *
    * @param name The new name of the channel. Must be between 2 and 100 characters inclusive.
    * @param position The raw position of the channel. Does not directly correspond to the order.
    * @param topic The new topic of the channel. Must be 1024 characters or less.
    * @return The channel that was modified.
    */
    pplx::task<Channel> modify(std::string name = "", std::string topic = "", int32_t position = 0) const;

    /** Modify a voice channel's attributes.
    *
    * @param name The new name of the channel. Must be between 2 and 100 characters inclusive.
    * @param position The raw position of the channel. Does not directly correspond to the order.
    * @param bitrate The new bitrate of the channel. Must be between 8000 and 96000 inclusive.
    * @param user_limit The new user limit for the channel. Must be between 1 and 99 inclusive.
    * @return The channel that was modified.
    */
    pplx::task<Channel> modify(std::string name = "", int32_t position = 0, uint32_t bitrate = 0, uint32_t user_limit = 0) const;


    /** Removes this channel. Cannot be undone. */
    pplx::task<Channel> remove() const;

    /** Gets a list of messages from this channel.
     *
     * @param limit The amount of messages to get.
     * @param method The search method to use. Defaults to none.
     * @param pivot The message id to pivot the search around.
     * @return A list of messages that were retrieved.
     */
    pplx::task<std::vector<Message>> get_messages(int32_t limit = 50, SearchMethod method = SearchMethod::None, Snowflake pivot = 0) const;

    /** Gets a message given its id.
     *
     * @param message_id The id of the message to get.
     * @return The message that was found, or an empty message if not found.
     */
    pplx::task<Message> get_message(Snowflake message_id) const;

    /** Creates a message and sends it to the channel.
     *
     * @param content The content of the message.
     * @param tts Whether or not this message should be text-to-speech.
     * @return The message that was sent.
     */
    pplx::task<Message> send_message(std::string content, bool tts = false, Embed Embed = Embed()) const;

    /** Creates a message with the given content and Embed that is modified through the callback.
     *
     * @code
     * auto embed_task = chan.send_embed([](Embed& e)
     * {
     *     e.set_title("Example Embed");
     *     e.set_description("This is how to use the send_embed callback.");
     *     e.add_field("Field1", "You can add any Embed object by using the Embed mutator methods.");
     *     e.add_field("Field2", "When this lambda is finished, the Embed will be sent.");
     * });
     * @endcode
     *  
     * @param modify_callback The callback that will be used to modify an Embed.
     * @param content Text that will appear above the Embed as a normal message.
     */
    pplx::task<Message> send_embed(std::function<void(Embed&)> modify_callback, std::string content = "") const;

    /** Creates a reaction on a message.
     *
     * @param message_id The message to react to.
     * @param emoji The emoji to react with.
     * @return Success status.
     */
    pplx::task<bool> create_reaction(Snowflake message_id, Emoji emoji) const;

    /** Creates a reaction on a message.
     *
     * @param message_id The message to react to.
     * @param emoji The emoji to react with.
     * @return Success status.
     */
    pplx::task<bool> create_reaction(Snowflake message_id, std::string emoji) const;

    /** Deletes a reaction that a user or Bot has made.
    *
    * @param message_id The message to remove the reaction from.
    * @param emoji The emoji to remove from the reaction list.
    * @param user_id The id of the user whose reaction will be removed. Defaults to the Bot's id.
    * @return Success status.
    */
    pplx::task<bool> remove_reaction(Snowflake message_id, Emoji emoji, Snowflake user_id = 0) const;

    /** Get a list of users who reacted with a particular emoji.
    *
    * @param message_id The message to get the list of reaction users from.
    * @param emoji The emoji that we should get the user list for.
    * @return A list of users who reacted with the given emoji.
    */
    pplx::task<std::vector<User>> get_reactions(Snowflake message_id, Emoji emoji) const;

    /** Deletes all reactions on a message.
    *
    * @param message_id The message to delete all reactions on.
    */
    pplx::task<void> remove_all_reactions(Snowflake message_id) const;

    /** Edits a message with new information.
    *
    * @param message_id The message to edit.
    * @param new_content The new content of the message.
    * @return The message that was edited.
    */
    pplx::task<Message> edit_message(Snowflake message_id, std::string new_content) const;

    /** Delete a message from a channel.
    *
    * @param message_id The message to delete.
    * @return Success status.
    */
    pplx::task<bool> remove_message(Snowflake message_id) const;

    /** Delete a list of messages all at once.
    *
    * @param message_ids A list of message ids to delete. Must be 100 or fewer.
    * @return Success status.
    */
    pplx::task<bool> remove_messages(std::vector<Snowflake> message_ids) const;

    /** Delete a set amount of messages from a channel all at once.
    * @param amount The amount of messages to delete. Must be between 2 and 100 inclusive.
    * @throw DiscordException on invalid amount.
    */
    pplx::task<bool> remove_messages(int amount = 2) const;

    /** Edits the permissions of either a member or a role.
    *
    * @param overwrite The overwrite to update.
    * @param allow The permissions to allow.
    * @param deny The permissions to deny.
    * @param type The type of permission to edit.
    * @return Success status.
    */
    pplx::task<bool> edit_permissions(Overwrite overwrite, uint32_t allow, uint32_t deny, std::string type) const;

    /** Delete permissions from a channel.
    *
    * @param overwrite The permissions to delete.
    */
    pplx::task<bool> remove_permission(Overwrite overwrite) const;

    /** Triggers a typing indicator for the current user.
    *
    * @return Success status.
    */
    pplx::task<bool> start_typing() const;

    /** Get a list of pinned messages in the given channel.
    *
    * @return A list of messages that were pinned.
    */
    pplx::task<std::vector<Message>> get_pinned_messages() const;

    /** Pins a message in a channel.
    *
    * @param message_id The message to pin.
    * @return Success status.
    */
    pplx::task<bool> pin(Snowflake message_id) const;

    /** Remove a message from the list of pinned messages.
    *
    * @param message_id The message to unpin.
    * @return Success status.
    */
    pplx::task<bool> unpin(Snowflake message_id) const;

    /** Add a user to a group DM.
    *
    * @param user_id The id of the user to add.
    * @param access_token The access token that allows the Bot to add a user to a DM.
    * @param nickname The nickname of the added user.
    */
    pplx::task<void> add_recipient(Snowflake user_id, std::string access_token, std::string nickname) const;

    /** Remove a user from a group DM.
    *
    * @param user_id The id of the user to remove.
    */
    pplx::task<void> remove_recipient(Snowflake user_id) const;
  };
}

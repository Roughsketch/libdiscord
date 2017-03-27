#pragma once

#include <cpprest/http_client.h>

#include "common.h"
#include "embed.h"
#include "permission.h"
#include "user.h"

namespace discord
{
  class emoji;
  class message;

  enum class search_method
  {
    None,
    Before,
    After,
    Around
  };

  class overwrite : public identifiable
  {
    std::string m_type;
    permission m_allow;
    permission m_deny;
  public:
    overwrite();
    explicit overwrite(rapidjson::Value& data);

    /** Get the type of this overwrite (either "role" or "member")

    @return The type of this overwrite.
    */
    std::string type() const;

    /** Get the permissions that are allowed.

    @return The permissions that are allowed.
    */
    permission allow() const;

    /** Get the permissions that are denied.

    @return The permissions that are denied.
    */
    permission deny() const;
  };

  /** Types of channels that can be sent by the API. */
  enum channel_type : uint8_t
  {
    Text = 0,
    Private,
    Voice,
    Group
  };

  class channel : public identifiable, public connection_object
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
    snowflake m_last_message_id;

    //  Guild Channels
    std::string m_name;
    channel_type m_type;
    int32_t m_position;
    std::vector<overwrite> m_permission_overwrites;
    std::string m_topic;
    uint32_t m_bitrate;
    uint32_t m_user_limit;

    //  DM Specific
    user m_recipient;

    bool m_is_dm;
    bool m_empty;
  public:
    channel();
    explicit channel(connection_state* owner, rapidjson::Value& data);

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
    channel_type type() const;

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
    guild guild() const;

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
    pplx::task<channel> modify(std::string name = "", std::string topic = "", int32_t position = 0) const;

    /** Modify a voice channel's attributes.
    *
    * @param name The new name of the channel. Must be between 2 and 100 characters inclusive.
    * @param position The raw position of the channel. Does not directly correspond to the order.
    * @param bitrate The new bitrate of the channel. Must be between 8000 and 96000 inclusive.
    * @param user_limit The new user limit for the channel. Must be between 1 and 99 inclusive.
    * @return The channel that was modified.
    */
    pplx::task<channel> modify(std::string name = "", int32_t position = 0, uint32_t bitrate = 0, uint32_t user_limit = 0) const;


    /** Removes this channel. Cannot be undone. */
    pplx::task<channel> remove() const;

    /** Gets a list of messages from this channel.
     *
     * @param limit The amount of messages to get.
     * @param method The search method to use. Defaults to none.
     * @param pivot The message id to pivot the search around.
     * @return A list of messages that were retrieved.
     */
    pplx::task<std::vector<message>> get_messages(int32_t limit = 50, search_method method = search_method::None, snowflake pivot = 0) const;

    /** Gets a message given its id.
     *
     * @param message_id The id of the message to get.
     * @return The message that was found, or an empty message if not found.
     */
    pplx::task<message> get_message(snowflake message_id) const;

    /** Creates a message and sends it to the channel.
     *
     * @param content The content of the message.
     * @param tts Whether or not this message should be text-to-speech.
     * @return The message that was sent.
     */
    pplx::task<message> send_message(std::string content, bool tts = false, discord::embed embed = discord::embed()) const;

    /** Creates a message with the given content and embed that is modified through the callback.
     *
     * @code
     * auto embed_task = chan.send_embed([](embed& e)
     * {
     *     e.set_title("Example embed");
     *     e.set_description("This is how to use the send_embed callback.");
     *     e.add_field("Field1", "You can add any embed object by using the embed mutator methods.");
     *     e.add_field("Field2", "When this lambda is finished, the embed will be sent.");
     * });
     * @endcode
     *  
     * @param modify_callback The callback that will be used to modify an embed.
     * @param content Text that will appear above the embed as a normal message.
     */
    pplx::task<message> send_embed(std::function<void(embed&)> modify_callback, std::string content = "") const;

    /** Creates a reaction on a message.
     *
     * @param message_id The message to react to.
     * @param emoji The emoji to react with.
     * @return Success status.
     */
    pplx::task<bool> create_reaction(snowflake message_id, emoji emoji) const;

    /** Creates a reaction on a message.
     *
     * @param message_id The message to react to.
     * @param emoji The emoji to react with.
     * @return Success status.
     */
    pplx::task<bool> create_reaction(snowflake message_id, std::string emoji) const;

    /** Deletes a reaction that a user or bot has made.
    *
    * @param message_id The message to remove the reaction from.
    * @param emoji The emoji to remove from the reaction list.
    * @param user_id The id of the user whose reaction will be removed. Defaults to the bot's id.
    * @return Success status.
    */
    pplx::task<bool> remove_reaction(snowflake message_id, emoji emoji, snowflake user_id = 0) const;

    /** Get a list of users who reacted with a particular emoji.
    *
    * @param message_id The message to get the list of reaction users from.
    * @param emoji The emoji that we should get the user list for.
    * @return A list of users who reacted with the given emoji.
    */
    pplx::task<std::vector<user>> get_reactions(snowflake message_id, emoji emoji) const;

    /** Deletes all reactions on a message.
    *
    * @param message_id The message to delete all reactions on.
    */
    pplx::task<void> remove_all_reactions(snowflake message_id) const;

    /** Edits a message with new information.
    *
    * @param message_id The message to edit.
    * @param new_content The new content of the message.
    * @return The message that was edited.
    */
    pplx::task<message> edit_message(snowflake message_id, std::string new_content) const;

    /** Delete a message from a channel.
    *
    * @param message_id The message to delete.
    * @return Success status.
    */
    pplx::task<bool> remove_message(snowflake message_id) const;

    /** Delete a list of messages all at once.
    *
    * @param message_ids A list of message ids to delete. Must be 100 or fewer.
    * @return Success status.
    */
    pplx::task<bool> remove_messages(std::vector<snowflake> message_ids) const;

    /** Edits the permissions of either a member or a role.
    *
    * @param overwrite The overwrite to update.
    * @param allow The permissions to allow.
    * @param deny The permissions to deny.
    * @param type The type of permission to edit.
    * @return Success status.
    */
    pplx::task<bool> edit_permissions(overwrite overwrite, uint32_t allow, uint32_t deny, std::string type) const;

    /** Delete permissions from a channel.
    *
    * @param overwrite The permissions to delete.
    */
    pplx::task<bool> remove_permission(overwrite overwrite) const;

    /** Triggers a typing indicator for the current user.
    *
    * @return Success status.
    */
    pplx::task<bool> start_typing() const;

    /** Get a list of pinned messages in the given channel.
    *
    * @return A list of messages that were pinned.
    */
    pplx::task<std::vector<message>> get_pinned_messages() const;

    /** Pins a message in a channel.
    *
    * @param message_id The message to pin.
    * @return Success status.
    */
    pplx::task<bool> pin(snowflake message_id) const;

    /** Remove a message from the list of pinned messages.
    *
    * @param message_id The message to unpin.
    * @return Success status.
    */
    pplx::task<bool> unpin(snowflake message_id) const;

    /** Add a user to a group DM.
    *
    * @param user_id The id of the user to add.
    * @param access_token The access token that allows the bot to add a user to a DM.
    * @param nickname The nickname of the added user.
    */
    pplx::task<void> add_recipient(snowflake user_id, std::string access_token, std::string nickname) const;

    /** Remove a user from a group DM.
    *
    * @param user_id The id of the user to remove.
    */
    pplx::task<void> remove_recipient(snowflake user_id) const;
  };
}

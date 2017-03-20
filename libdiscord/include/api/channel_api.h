#pragma once
#include "channel.h"
#include "embed.h"
#include "message.h"
#include "api.h"

namespace discord
{
  class emoji;

  namespace api
  {
    namespace channel
    {
      /** Modify a channel's attributes.
       *
       * @param channel_id The id of the channel to modify.
       * @param name The new name of the channel. Must be between 2 and 100 characters inclusive.
       * @param position The raw position of the channel. Does not directly correspond to the order.
       * @param topic The new topic of the channel. Must be 1024 characters or less.
       * @return The channel that was modified.
       */
      pplx::task<discord::channel> modify_text_channel(connection_state* conn, snowflake channel_id, std::string name = "", int32_t position = 0, std::string topic = "");

      /** Modify a channel's attributes.
       *
       * @param channel_id The id of the channel to modify.
       * @param name The new name of the channel. Must be between 2 and 100 characters inclusive.
       * @param position The raw position of the channel. Does not directly correspond to the order.
       * @param bitrate The new bitrate of the channel. Must be between 8000 and 96000 inclusive.
       * @param user_limit The new user limit for the channel. Must be between 1 and 99 inclusive.
       * @return The channel that was modified.
       */
      pplx::task<discord::channel> modify_voice_channel(connection_state* conn, snowflake channel_id, std::string name = "", int32_t position = 0, uint32_t bitrate = 0, uint32_t user_limit = 0);

      /** Deletes a channel.
       *
       * @param channel_id The channel to delete
       * @return The channel object that was deleted.
       */
      pplx::task<discord::channel> remove(connection_state* conn, snowflake channel_id);

      /** Gets a list of messages from a channel. Optionally accepts a search criteria and message id to search around.
       *
       * @param channel_id The channel to get messages from.
       * @param limit The amount of messages to get. Defaults to 50 and must be between 1 and 100 inclusive.
       * @param method The search method to use when getting methods. If used, must pass in the pivot snowflake.
       * @param pivot The message to base the search around.
       * @return A vector of messages that were retrieved.
       */
      pplx::task<std::vector<message>> get_messages(connection_state* conn, snowflake channel_id, int32_t limit = 50, search_method method = search_method::None, snowflake pivot = 0);

      /** Get a single message from a channel.
       *
       * @param channel_id The channel to get the message from.
       * @param message_id The message to get.
       * @return The message that was retrieved.
       */
      pplx::task<message> get_message(connection_state* conn, snowflake channel_id, snowflake message_id);

      /** Creates a message and sends it to the channel.
       *
       * @param channel_id The channel to send the message to.
       * @param content The content of the message.
       * @param tts Whether or not this message should be text-to-speech.
       * @return The message that was sent.
       */
      pplx::task<message> create_message(connection_state* conn, snowflake channel_id, std::string content, bool tts = false, discord::embed embed = discord::embed());

      /** Creates a reaction on a message.
       *
       * @param channel_id The channel that holds the message to react to.
       * @param message_id The message to react to.
       * @param emoji The emoji to react with.
       * @return Success status.
       */
      pplx::task<bool> create_reaction(connection_state* conn, snowflake channel_id, snowflake message_id, std::string emoji);

      /** Deletes a reaction that the bot has made.
       *
       * @param channel_id The channel where the message was reacted to.
       * @param message_id The message to remove the reaction from.
       * @param emoji The emoji to remove from the reaction list.
       * @return Success status.
       */
      pplx::task<bool> remove_own_reaction(connection_state* conn, snowflake channel_id, snowflake message_id, emoji emoji);

      /** Deletes a reaction that a user has made.
       *
       * @param channel_id The channel where the message was reacted to.
       * @param message_id The message to remove the reaction from.
       * @param emoji The emoji to remove from the reaction list.
       * @param user_id The id of the user whose reaction will be removed.
       * @return Success status.
       */
      pplx::task<bool> remove_user_reaction(connection_state* conn, snowflake channel_id, snowflake message_id, emoji emoji, snowflake user_id);

      /** Get a list of users who reacted with a particular emoji.
       *
       * @param channel_id The channel where the message was reacted to.
       * @param message_id The message to get the list of reaction users from.
       * @param emoji The emoji that we should get the user list for.
       * @return A list of users who reacted with the given emoji.
       */
      pplx::task<std::vector<discord::user>> get_reactions(connection_state* conn, snowflake channel_id, snowflake message_id, emoji emoji);

      /** Deletes all reactions on a message.
       *
       * @param channel_id The channel where the message was reacted to.
       * @param message_id The message to delete all reactions on.
       */
      pplx::task<void> remove_all_reactions(connection_state* conn, snowflake channel_id, snowflake message_id);

      /** Edits a message with new information.
       *
       * @param channel_id The channel where the message is located.
       * @param message_id The message to edit.
       * @param new_content The new content of the message.
       * @return The message that was edited.
       */
      pplx::task<message> edit_message(connection_state* conn, snowflake channel_id, snowflake message_id, std::string new_content);

      /** Delete a message from a channel.
       *
       * @param channel_id The channel where the message is located.
       * @param message_id The message to delete.
       * @return Success status.
       */
      pplx::task<bool> remove_message(connection_state* conn, snowflake channel_id, snowflake message_id);

      /** Delete a list of messages all at once.
       *
       * @param channel_id The channel where the messages are located.
       * @param message_ids A list of message ids to delete. Must be 100 or fewer.
       * @return Success status.
       */
      pplx::task<bool> bulk_remove_messages(connection_state* conn, snowflake channel_id, std::vector<snowflake> message_ids);

      /** Edits the permissions of either a member or a role.
       *
       * @param channel_id The channel whose permissions will be updated.
       * @param overwrite The overwrite to update.
       * @param allow The permissions to allow.
       * @param deny The permissions to deny.
       * @param type The type of permission to edit.
       * @return Success status.
       */
      pplx::task<bool> edit_permissions(connection_state* conn, snowflake channel_id, overwrite overwrite, uint32_t allow, uint32_t deny, std::string type);

      /** Get a list of invites for this channel.
       *
       * @param channel_id The channel to get invites to.
       * @return A list of invites to the given channel.
       */
        // TODO: Implement
      //std::vector<invite> get_invites(connection_state* conn, snowflake channel_id);

      /** Create a channel invite.
       *
       * @param channel_id The id of the channel to create an invite for.
       * @param max_age The maximum lifetime for this invite.
       * @param max_uses The maximum amount of time this invite can be used.
       * @param temporary Whether or not this invite should be temporary. Defaults to false.
       * @param unique Whether or not this invite is unique. Defaults to false.
       */
        // TODO: Implement
      //invite create_invite(connection_state* conn, snowflake channel_id, uint32_t max_age, uint32_t max_uses, bool temporary = false, bool unique = false);

      /** Delete permissions from a channel.
       *
       * @param channel_id The channel whose permissions to delete
       * @param overwrite The permissions to delete.
       */
      pplx::task<bool> remove_permission(connection_state* conn, snowflake channel_id, overwrite overwrite);

      /** Triggers a typing indicator for the current user.
       *
       * @param channel_id The channel to indicate typing in.
       * @return Success status.
       */
      pplx::task<bool> trigger_typing_indicator(connection_state* conn, snowflake channel_id);

      /** Get a list of pinned messages in the given channel.
       *
       * @param channel_id The channel whose pinned messages to get.
       * @return A list of messages that were pinned.
       */
      pplx::task<std::vector<message>> get_pinned_messages(connection_state* conn, snowflake channel_id);

      /** Pins a message in a channel.
       *
       * @param channel_id The channel that owns the message to pin.
       * @param message_id The message to pin.
       * @return Success status.
       */
      pplx::task<bool> add_pinned_message(connection_state* conn, snowflake channel_id, snowflake message_id);

      /** Remove a message from the list of pinned messages.
       *
       * @param channel_id The channel that owns the message to unpin.
       * @param message_id The message to unpin.
       * @return Success status.
       */
      pplx::task<bool> remove_pinned_message(connection_state* conn, snowflake channel_id, snowflake message_id);

      /** Add a user to a group DM.
       *
       * @param channel_id The channel to add the user to.
       * @param user_id The id of the user to add.
       * @param access_token The access token that allows the bot to add a user to a DM.
       * @param nickname The nickname of the added user.
       */
      pplx::task<void> group_dm_add_recipient(connection_state* conn, snowflake channel_id, snowflake user_id, std::string access_token, std::string nickname);

      /** Remove a user from a group DM.
       *
       * @param channel_id The channel to remove the user from.
       * @param user_id The id of the user to remove.
       */
      pplx::task<void> group_dm_remove_recipient(connection_state* conn, snowflake channel_id, snowflake user_id);
    }
  }
}

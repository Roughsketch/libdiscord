#pragma once

#include "common.h"
#include "channel.h"
#include "connection.h"
#include "user.h"

namespace discord
{
  namespace api
  {
    namespace user
    {
      /** Get information on the current user.
       *
       * @return The information on the current user.
       */
      pplx::task<User> get_current_user(ConnectionState* conn);

      /** Get information on a specific user.
       *
       * @param user_id The id of the user to get information on.
       * @return The information on the user.
       */
      pplx::task<User> get_user(ConnectionState* conn, Snowflake user_id);

      /** Modifies the current user's username and optionally avatar.
       *
       * @param username The new username for this user.
       * @param avatar The new avatar data for this user.
       * @return The updated user.
       */
      pplx::task<User> modify(ConnectionState* conn, std::string username, std::string avatar = "");

      /** Get the guilds this user is currently in.
       *
       * @return A list of userGuild objects that identify the guilds this user is in.
       */
      pplx::task<std::vector<user_guild>> guilds(ConnectionState* conn, uint32_t limit = 100, SearchMethod method = SearchMethod::None, Snowflake guild_id = 0);

      /** Leaves a guild.
       *
       * @param guild_id The guild to leave.
       * @return Success status.
       */
      pplx::task<bool> leave_guild(ConnectionState* conn, Snowflake guild_id);

      /** Get a list of DMs that the current user is in.
       *
       * @return A list of DMs that the current user is in.
       */
      pplx::task<std::vector<Channel>> get_dms(ConnectionState* conn);

      /** Creates a DM with another user.
       *
       * @param recipient_id The recipient for this DM.
       * @return The channel that was created for this DM.
       */
      pplx::task<Channel> create_dm(ConnectionState* conn, Snowflake recipient_id);

      /** Creates a group DM with a list of users.
       *
       * @param access_tokens Access tokens of users that have granted this user the gdm.join scope.
       * @param user_nicknames A map of user ids to nicknames that will be assigned.
       * @return The channel that was created for this group DM.
       */
      pplx::task<Channel> create_group_dm(ConnectionState* conn, std::vector<std::string> access_tokens, std::map<Snowflake, std::string> user_nicknames);

      /** Get a list of connections that this user has.
       *
       * @return A list of connections for this user.
       */
      pplx::task<std::vector<Connection>> connections(ConnectionState* conn);
    }
  }
}

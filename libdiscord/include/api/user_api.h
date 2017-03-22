#pragma once

#include "common.h"
#include "user.h"
#include "channel.h"

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
      pplx::task<discord::user> get_current_user(connection_state* conn);

      /** Get information on a specific user.
       *
       * @param user_id The id of the user to get information on.
       * @return The information on the user.
       */
      pplx::task<discord::user> get_user(connection_state* conn, snowflake user_id);

      /** Modifies the current user's username and optionally avatar.
       *
       * @param username The new username for this user.
       * @param avatar The new avatar data for this user.
       * @return The updated user.
       */
      pplx::task<discord::user> modify(connection_state* conn, std::string username, std::string avatar = "");

      /** Get the guilds this user is currently in.
       *
       * @return A list of userGuild objects that identify the guilds this user is in.
       */
      pplx::task<std::vector<user_guild>> guilds(connection_state* conn, uint32_t limit = 100, search_method method = search_method::None, snowflake guild_id = 0);

      /** Leaves a guild.
       *
       * @param guild_id The guild to leave.
       * @return Success status.
       */
      pplx::task<bool> leave_guild(connection_state* conn, snowflake guild_id);

      /** Get a list of DMs that the current user is in.
       *
       * @return A list of DMs that the current user is in.
       */
      pplx::task<std::vector<discord::channel>> get_dms(connection_state* conn);

      /** Creates a DM with another user.
       *
       * @param recipient_id The recipient for this DM.
       * @return The channel that was created for this DM.
       */
      pplx::task<discord::channel> create_dm(connection_state* conn, snowflake recipient_id);

      /** Creates a group DM with a list of users.
       *
       * @param access_tokens Access tokens of users that have granted this user the gdm.join scope.
       * @param user_nicknames A map of user ids to nicknames that will be assigned.
       * @return The channel that was created for this group DM.
       */
      pplx::task<discord::channel> create_group_dm(connection_state* conn, std::vector<std::string> access_tokens, std::map<snowflake, std::string> user_nicknames);

      /** Get a list of connections that this user has.
       *
       * @return A list of connections for this user.
       */
      pplx::task<std::vector<connection>> connections(connection_state* conn);
    }
  }
}

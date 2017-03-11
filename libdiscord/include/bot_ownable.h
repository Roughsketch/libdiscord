#pragma once

#include "bot.h"
#include "common.h"

namespace discord
{
  class channel;

  /** A class used to pass a const bot reference around to other classes 
   *  that need to access bot methods. For example, a message object needs
   *  to be able to access the channel cache, but can't do so if it doesn't
   *  have a bot object to get the cache from. */
  class bot_ownable
  {
    const bot* m_owner;
  public:
    bot_ownable();
    explicit bot_ownable(const bot* owner);

    /** Get the const bot that this class holds.
     *
     *  @return A const pointer to the bot.
     */
    const bot* owner() const;

    /** A convinience method to get find a channel from the bot without first calling owner.
    *
    *  @param id The id of the channel to find.
    *  @return The channel that was found, or an empty channel if not found.
    */
    const channel& find_channel(snowflake id);

    /** A convinience method to get find a guild from the bot without first calling owner.
    *
    *  @param id The id of the guild to find.
    *  @return The guild that was found, or an empty guild if not found.
    */
    const channel& find_guild(snowflake id);

    /** A convinience method to get find a user from the bot without first calling owner.
    *
    *  @param id The id of the user to find.
    *  @return The user that was found, or an empty user if not found.
    */
    const channel& find_user(snowflake id);
  };
}
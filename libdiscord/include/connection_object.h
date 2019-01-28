#pragma once

#include "common.h"

namespace discord
{
  class Guild;
  class ConnectionState;

  /** A class used to pass a const connection_state reference around to other classes 
   *  that need to access api call and cache methods. For example, a message object needs
   *  to be able to access the channel cache, but can't do so if it doesn't
   *  have a connection_state object to get the cache from. */
  class ConnectionObject
  {
  protected:
    ConnectionState* m_owner;
  public:
    ConnectionObject();
    explicit ConnectionObject(ConnectionState* owner);

    /** Get the const connection_state that this class holds.
     *
     *  @return A const pointer to the connection_state.
     */
    ConnectionState* owner() const;

    /** A convinience method to get find a guild from the connection_state without first calling owner.
    *
    *  @param id The id of the guild to find.
    *  @return The guild that was found, or an empty guild if not found.
    */
    std::unique_ptr<Guild> find_guild(Snowflake id) const;
  };
}
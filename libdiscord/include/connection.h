#pragma once

#include "common.h"
#include "integration.h"

namespace discord
{
  class Connection
  {
    std::string m_id;
    std::string m_name;
    std::string m_type;
    bool m_revoked;
    std::vector<Integration> m_integrations;
  public:
    Connection();
    explicit Connection(rapidjson::Value& data);

    /** Gets the id for this connection.

    @return The id of this connection.
    */
    std::string id() const;

    /** Get the name of this connection.

    @return The name of this connection.
    */
    std::string name() const;

    /** Gets the type of this connection (twitch, youtube)

    @return The type of this connection.
    */
    std::string type() const;

    /** Get the revoked status of this connection.

    @return The revoked status of this connection.
    */
    bool revoked() const;

    /** Gets the list of partial integrations associated with this connection.

    @return The list of partial integrations associated with this connection.
    */
    std::vector<Integration> integrations() const;
  };
}
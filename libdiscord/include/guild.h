#pragma once

#include "common.h"

namespace discord
{
  class user;
  class bot;
  class channel;
  class member;
  class presence_event;
  class role;

  class guild : public identifiable
  {
    bot& m_owner;
  public:
    explicit guild(bot& owner, rapidjson::Value& data);
    guild(const guild& other);

    std::string name() const;

    void set_unavailable(bool value);

    void add_channel(channel chan);
    void update_channel(channel chan);
    void remove_channel(channel chan);

    void add_member(member mem);
    void update_member(std::vector<snowflake> role_ids, user user, std::string nick);
    void remove_member(member mem);

    void add_role(role role);
    void update_role(role role);
    void remove_role(role role);

    void update_presence(presence_event presence);
  };
}
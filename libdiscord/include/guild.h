#pragma once

#include "common.h"

namespace discord
{
  class user;
  class bot;
  class channel;
  class emoji;
  class member;
  class presence_event;
  class role;

  class guild : public identifiable
  {
    std::string m_token;
  public:
    guild();
    explicit guild(std::string token, rapidjson::Value& data);
    guild(const guild& other);

    std::string name() const;
    std::vector<emoji> emojis() const;
    void set_emojis(std::vector<emoji>& emojis);

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
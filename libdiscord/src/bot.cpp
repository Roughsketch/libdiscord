#include "api.h"
#include "bot.h"
#include "channel.h"
#include "easylogging++.h"
#include "emoji.h"
#include "event/message_event.h"
#include "gateway.h"
#include "guild.h"
#include "member.h"
#include "role.h"
#include "user.h"

namespace discord
{
  bot::bot(std::string token, std::string prefix) : m_token("Bot " + token), m_prefix(prefix)
  {
    m_gateway = std::make_unique<gateway>(m_token);
    m_gateway->on_dispatch(std::bind(&bot::on_dispatch, this, std::placeholders::_1, std::placeholders::_2));
  }

  bot::~bot()
  {
  }

  void bot::update_emojis(rapidjson::Value& data)
  {
    snowflake guild_id(data["guild_id"].GetString());
    auto owner = m_guilds[guild_id];

    std::vector<emoji> new_emojis;
    
    for (auto& emoji_data : data["emojis"].GetArray())
    {
      new_emojis.push_back(emoji(emoji_data));
    }

    auto old_emojis = owner.emojis();

    //  Must sort or set differences won't work properly.
    std::sort(std::begin(new_emojis), std::end(new_emojis));
    std::sort(std::begin(old_emojis), std::end(old_emojis));

    owner.set_emojis(new_emojis);

    std::vector<emoji> added_emoji;
    std::vector<emoji> deleted_emoji;
    std::vector<emoji> updated_emoji;

    std::set_difference(std::begin(new_emojis), std::end(new_emojis), std::begin(old_emojis), std::end(old_emojis), std::back_inserter(added_emoji));
    std::set_difference(std::begin(old_emojis), std::end(old_emojis), std::begin(new_emojis), std::end(new_emojis), std::back_inserter(deleted_emoji));
    std::set_intersection(std::begin(old_emojis), std::end(old_emojis), std::begin(new_emojis), std::end(new_emojis), std::back_inserter(updated_emoji));

    updated_emoji.erase(std::remove_if(std::begin(updated_emoji), std::end(updated_emoji),
      [old_emojis](emoji& e) {
      auto other = std::find_if(std::begin(old_emojis), std::end(old_emojis),
        [e](const emoji& a) {
          return a.id() == e.id();
        });

      if (other == std::end(old_emojis))
      {
        LOG(ERROR) << "Somehow a new emoji got into the updated list.";
        return false;
      }

      //  Although data seems to be sent in sorted order, we need to be sure since vector compare requires sorted order to work.
      auto e_roles = e.roles();
      auto other_roles = other->roles();

      std::sort(std::begin(e_roles), std::end(e_roles));
      std::sort(std::begin(other_roles), std::end(other_roles));

      return (e.name() == other->name() && e_roles == other_roles);
    })
    );

    for (auto& emoji : added_emoji)
    {
      LOG(TRACE) << "Sending Emoji Created event with emoji " << emoji.name();
      m_on_emoji_created(emoji);
    }

    for (auto& emoji : deleted_emoji)
    {
      LOG(TRACE) << "Sending Emoji Deleted event with emoji " << emoji.name();
      m_on_emoji_deleted(emoji);
    }

    for (auto& emoji : updated_emoji)
    {
      LOG(TRACE) << "Sending Emoji Updated event with emoji " << emoji.name();
      m_on_emoji_updated(emoji);
    }
  }

  void bot::run(bool async) const
  {
    m_gateway->start();

    if (!async)
    {
      for (;;)
      {
        std::this_thread::sleep_for(std::chrono::seconds(100000));
      }
    }
  }

  const std::string& bot::token() const
  {
    return m_token;
  }

  const user& bot::profile() const
  {
    return *m_profile.get();
  }

  std::vector<guild> bot::guilds() const
  {
    std::vector<guild> guild_vec;

    for (auto& pair : m_guilds)
    {
      guild_vec.push_back(pair.second);
    }

    return guild_vec;
  }

  void bot::on_message(std::function<void(message_event&)> callback)
  {
    m_on_message = callback;
  }

  void bot::on_message_edited(std::function<void(message_event&)> callback)
  {
    m_on_message_edited = callback;
  }

  void bot::on_message_deleted(std::function<void(message_deleted_event&)> callback)
  {
    m_on_message_deleted = callback;
  }

  void bot::on_emoji_created(std::function<void(emoji&)> callback)
  {
    m_on_emoji_created = callback;
  }

  void bot::on_emoji_deleted(std::function<void(emoji&)> callback)
  {
    m_on_emoji_deleted = callback;
  }

  void bot::on_emoji_updated(std::function<void(emoji&)> callback)
  {
    m_on_emoji_updated = callback;
  }

  void bot::on_presence(std::function<void(presence&)> callback)
  {
    m_on_presence = callback;
  }

  void bot::on_typing(std::function<void(typing_event&)> callback)
  {
    m_on_typing = callback;
  }

  void bot::add_command(std::string name, std::function<void(message_event&)> callback)
  {
    m_commands[name] = callback;
  }

  void bot::on_dispatch(std::string event_name, rapidjson::Value& data)
  {
    //LOG(INFO) << "Bot.handle_dispatch entered with " << event_name.c_str() << ".";

    if (event_name == "READY")
    {
      m_profile = std::make_unique<user>(this, data["user"]);

      for (auto& channel_data : data["private_channels"].GetArray())
      {
        snowflake id(channel_data["id"].GetString());
        m_private_channels[id] = channel(this, 0, channel_data);
      }
    }
    else if (event_name == "CHANNEL_CREATE")
    {
      auto guild_id = snowflake(data["guild_id"].GetString());
      channel chan(this, guild_id, data);

      auto owner = m_guilds.find(guild_id);

      if (owner == std::end(m_guilds))
      {
        LOG(ERROR) << "Tried to add a channel from a non-existent guild.";
      }
      else
      {
        owner->second.add_channel(chan);
      }
    }
    else if (event_name == "CHANNEL_UPDATE")
    {
      auto guild_id = snowflake(data["guild_id"].GetString());
      channel chan(this, guild_id, data);

      auto owner = m_guilds.find(guild_id);

      if (owner == std::end(m_guilds))
      {
        LOG(ERROR) << "Tried to add a channel from a non-existent guild.";
      }
      else
      {
        owner->second.update_channel(chan);
      }
    }
    else if (event_name == "CHANNEL_DELETE")
    {
      auto guild_id = snowflake(data["guild_id"].GetString());
      channel chan(this, guild_id, data);

      auto owner = m_guilds.find(guild_id);

      if (owner == std::end(m_guilds))
      {
        LOG(ERROR) << "Tried to remove a channel from a non-existent guild.";
      }
      else
      {
        owner->second.remove_channel(chan);
      }
    }
    else if (event_name == "GUILD_CREATE")
    {
      guild new_guild(this, data);
      m_guilds[new_guild.id()] = new_guild;
    }
    else if (event_name == "GUILD_UPDATE")
    {
      guild updated(this, data);
      m_guilds[updated.id()] = updated;
    }
    else if (event_name == "GUILD_DELETE")
    {
      snowflake id(data["id"].GetString());

      if (data.FindMember("unavailable") != data.MemberEnd())
      {
        m_guilds[id].set_unavailable(true);
      }
      else
      {
        m_guilds.erase(id);
      }
    }
    else if (event_name == "GUILD_BAN_ADD")
    {
      user banned(this, data["user"]);
      snowflake guild_id(data["guild_id"].GetString());

      LOG(DEBUG) << "User " << banned.distinct()
        << " has been banned from "
        << m_guilds[guild_id].name() << ".";
    }
    else if (event_name == "GUILD_BAN_REMOVE")
    {
      user unbanned(this, data["user"]);
      snowflake guild_id(data["guild_id"].GetString());
      LOG(DEBUG) << "User " << unbanned.distinct()
        << " has been unbanned from "
        << m_guilds[guild_id].name();
    }
    else if (event_name == "GUILD_EMOJIS_UPDATE")
    {
      update_emojis(data);
    }
    else if (event_name == "GUILD_INTEGRATIONS_UPDATE")
    {
      LOG(DEBUG) << "Got a Guild Integrations Update, but left it unhandled.";
    }
    else if (event_name == "GUILD_MEMBER_ADD")
    {
      snowflake guild_id(data["guild_id"].GetString());
      member guild_member(this, data);
      m_guilds[guild_id].add_member(guild_member);
    }
    else if (event_name == "GUILD_MEMBER_REMOVE")
    {
      snowflake guild_id(data["guild_id"].GetString()); 
      member guild_member(this, data);
      m_guilds[guild_id].remove_member(guild_member);
    }
    else if (event_name == "GUILD_MEMBER_UPDATE")
    {
      snowflake guild_id(data["guild_id"].GetString());

      std::vector<snowflake> roles;
      std::string nick;
      user updated_user;

      set_from_json(nick, "nick", data);
      
      auto found = data.FindMember("user");
      if (found != data.MemberEnd() && !found->value.IsNull())
      {
        updated_user = user(this, data["user"]);
      }

      found = data.FindMember("roles");
      if (found != data.MemberEnd() && !found->value.IsNull())
      {
        for (auto& role_id : found->value.GetArray())
        {
          roles.push_back(snowflake(role_id.GetString()));
        }
      }

      m_guilds[guild_id].update_member(roles, updated_user, nick);
    }
    else if (event_name == "GUILD_MEMBERS_CHUNK")
    {
      snowflake guild_id(data["guild_id"].GetString());
      auto owner = m_guilds[guild_id];

      for (auto& member_data : data["members"].GetArray())
      {
        member guild_member(this, member_data);
        owner.add_member(guild_member);
      }
    }
    else if (event_name == "GUILD_ROLE_CREATE")
    {
      snowflake guild_id(data["guild_id"].GetString());
      role guild_role(data["role"]);
      m_guilds[guild_id].add_role(guild_role);
    }
    else if (event_name == "GUILD_ROLE_UPDATE")
    {
      snowflake guild_id(data["guild_id"].GetString());
      role guild_role(data["role"]);
      m_guilds[guild_id].update_role(guild_role);
    }
    else if (event_name == "GUILD_ROLE_DELETE")
    {
      snowflake guild_id(data["guild_id"].GetString());
      role guild_role(data["role"]);
      m_guilds[guild_id].remove_role(guild_role);
    }
    else if (event_name == "MESSAGE_CREATE")
    {
      message_event event(this, data);
      auto word = event.content().substr(0, event.content().find_first_of(" \n"));

      //  If we have a prefix and it's the start of this message and it's a command
      if (!m_prefix.empty() &&
        word.size() > m_prefix.size() &&
        (word.compare(0, m_prefix.size(), m_prefix) == 0) &&
        m_commands.count(word.substr(m_prefix.size()))
        )
      {
        //  Call the command
        m_commands[word.substr(m_prefix.size())](event);
      }
      else if (m_on_message)
      {
        //  Not a command, but if we have an OnMessage handler call that instead.
        m_on_message(event);
      }
    }
    else if (event_name == "MESSAGE_UPDATE")
    {
      if (m_on_message_edited)
      {
        message_event event(this, data);
        m_on_message_edited(event);
      }
    }
    else if (event_name == "MESSAGE_DELETE")
    {
      if (m_on_message_deleted)
      {
        message_deleted_event event(m_token, data);
        m_on_message_deleted(event);
      }
    }
    else if (event_name == "MESSAGE_DELETE_BULK")
    {
      std::vector<snowflake> ids;
      snowflake chan_id(data["channel_id"].GetString());

      for (auto& id : data["ids"].GetArray())
      {
        ids.push_back(snowflake(id.GetString()));
      }

      LOG(DEBUG) << "Sending out " << ids.size() << " MessageDeletedEvents";

      if (m_on_message_deleted)
      {
        for (auto& id : ids)
        {
          message_deleted_event event(m_token, id, chan_id);
          m_on_message_deleted(event);
        }
      }
    }
    else if (event_name == "PRESENCE_UPDATE")
    {
      presence presence(this, data);
      snowflake guild_id(data["guild_id"].GetString());

      m_guilds[guild_id].update_presence(presence);

      if (m_on_presence)
      {
        m_on_presence(presence);
      }
    }
    else if (event_name == "TYPING_START")
    {
      if (m_on_typing)
      {
        typing_event event(m_token, data);
        m_on_typing(event);
      }
    }
    else if (event_name == "VOICE_STATE_UPDATE")
    {

    }
    else if (event_name == "VOICE_SERVER_UPDATE")
    {

    }
  }
}

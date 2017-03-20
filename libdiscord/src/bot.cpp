#include "api.h"
#include "bot.h"
#include "channel.h"
#include "connection_state.h"
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
  void bot::on_event(event_type type, rapidjson::Value& data)
  {
    switch (type)
    {
    case Ready:
      {
        if (m_on_ready)
        {
          m_on_ready();
        }
        break;
      }
    case MessageCreated:
      {
        message_event event(m_conn_state.get(), data);
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
        break;
      }
    case MessageDeleted:
      {
        if (m_on_message_deleted)
        {
          message_deleted_event event(m_conn_state.get(), data);
          m_on_message_deleted(event);
        }
        break;
      }
    case MessageEdited:
      {
        if (m_on_message_edited)
        {
          message_event event(m_conn_state.get(), data);
          m_on_message_edited(event);
        }
        break;
      }
    case MessagesBulkDeleted:
      {
        if (m_on_message_deleted)
        {
          std::vector<snowflake> ids;
          snowflake chan_id(data["channel_id"].GetString());

          for (auto& id : data["ids"].GetArray())
          {
            ids.push_back(snowflake(id.GetString()));
          }

          LOG(DEBUG) << "Sending out " << ids.size() << " MessageDeletedEvents";

          for (auto& id : ids)
          {
            message_deleted_event event(m_conn_state.get(), id, chan_id);
            m_on_message_deleted(event);
          }
        }
      }
    case Presence:
      {
        if (m_on_presence)
        {
          presence presence(m_conn_state.get(), data);
          m_on_presence(presence);
        }
        break;
      }
    case Typing:
      {
        if (m_on_typing)
        {
          typing_event event(m_conn_state.get(), data);
          m_on_typing(event);
        }
      }
    case EmojiCreated: break;
    case EmojiDeleted: break;
    case EmojiEdited: break;
    default: ;
    }
  }

  bot::bot(std::string token, std::string prefix, int shards) : m_prefix(prefix)
  {
    m_conn_state = std::make_unique<connection_state>("Bot " + token, shards);
    m_conn_state->on_event(std::bind(&bot::on_event, this, std::placeholders::_1, std::placeholders::_2));
  }

  bot::~bot()
  {
  }

  void bot::run(bool async) const
  {
    m_conn_state->connect();

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
    return m_conn_state->token();
  }

  const user& bot::profile() const
  {
    return m_conn_state->profile();
  }

  std::vector<guild> bot::guilds() const
  {
    return m_conn_state->guilds();
  }

  std::vector<channel> bot::channels() const
  {
    std::vector<channel> channels;

    for (auto& guild : guilds())
    {
      for (auto& chan : guild.channels())
      {
        channels.push_back(chan);
      }
    }

    return channels;
  }

  guild bot::find_guild(snowflake id) const
  {
    return m_conn_state->find_guild(id);
  }

  void bot::on_ready(std::function<void()> callback)
  {
    m_on_ready = callback;
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
}

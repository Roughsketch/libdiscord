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
  void Bot::on_event(EventType type, rapidjson::Value& data)
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
        MessageEvent event(m_conn_state.get(), data);
        auto word = event.content().substr(0, event.content().find_first_of(" \n"));

        //  If we have a prefix and it's the start of this message and it's a command
        if (!m_prefix.empty() &&
          word.size() > m_prefix.size() &&
          (word.compare(0, m_prefix.size(), m_prefix) == 0) &&
          m_commands.count(word.substr(m_prefix.size()))
          )
        {
          //  Call the command
          pplx::create_task([this, word, event] 
          {
            m_commands[word.substr(m_prefix.size())](event); 
          }).then([](){});
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
          MessageDeletedEvent event(m_conn_state.get(), data);
          m_on_message_deleted(event);
        }
        break;
      }
    case MessageEdited:
      {
        if (m_on_message_edited)
        {
          MessageEvent event(m_conn_state.get(), data);
          m_on_message_edited(event);
        }
        break;
      }
    case MessagesBulkDeleted:
      {
        if (m_on_message_deleted)
        {
          std::vector<Snowflake> ids;
          Snowflake chan_id(data["channel_id"].GetString());

          for (const auto& id : data["ids"].GetArray())
          {
            ids.emplace_back(id.GetString());
          }

          LOG(DEBUG) << "Sending out " << ids.size() << " MessageDeletedEvents";

          for (const auto& id : ids)
          {
            MessageDeletedEvent event(m_conn_state.get(), id, chan_id);
            m_on_message_deleted(event);
          }
        }
      }
	case GuildCreated:
	  {
		if(m_on_guild_created)
		{
			Guild guild(m_conn_state.get(), data);
			m_on_guild_created(guild);
		}
		break;
	  }
    case PresenceUpdate:
      {
        if (m_on_presence)
        {
          Presence presence(m_conn_state.get(), data);
          m_on_presence(presence);
        }
        break;
      }
    case Typing:
      {
        if (m_on_typing)
        {
          TypingEvent event(m_conn_state.get(), data);
          m_on_typing(event);
        }
      }
    case EmojiCreated: break;
    case EmojiDeleted: break;
    case EmojiEdited: break;
    default: ;
    }
  }

  Bot::Bot(std::string token, std::string prefix, int shards) : m_prefix(prefix)
  {
    m_conn_state = std::make_unique<ConnectionState>("Bot " + token, shards);
    m_conn_state->on_event(std::bind(&Bot::on_event, this, std::placeholders::_1, std::placeholders::_2));
  }

  Bot::~Bot()
  {
  }

  void Bot::run(bool async) const
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

  const std::string& Bot::token() const
  {
    return m_conn_state->token();
  }

  const User& Bot::profile() const
  {
    return m_conn_state->profile();
  }

  std::vector<Guild> Bot::guilds() const
  {
    return m_conn_state->guilds();
  }

  std::vector<Channel> Bot::channels() const
  {
    std::vector<Channel> channels;

    for (const auto& guild : guilds())
    {
      for (const auto& chan : guild.channels())
      {
        channels.push_back(chan);
      }
    }

    return channels;
  }

  std::unique_ptr<Guild> Bot::find_guild(Snowflake id) const
  {
    return m_conn_state->find_guild(id);
  }

  void Bot::on_ready(std::function<void()> callback)
  {
    m_on_ready = callback;
  }

  void Bot::on_message(std::function<void(MessageEvent&)> callback)
  {
    m_on_message = callback;
  }

  void Bot::on_message_edited(std::function<void(MessageEvent&)> callback)
  {
    m_on_message_edited = callback;
  }

  void Bot::on_message_deleted(std::function<void(MessageDeletedEvent&)> callback)
  {
    m_on_message_deleted = callback;
  }

  void Bot::on_emoji_created(std::function<void(Emoji&)> callback)
  {
    m_on_emoji_created = callback;
  }

  void Bot::on_emoji_deleted(std::function<void(Emoji&)> callback)
  {
    m_on_emoji_deleted = callback;
  }

  void Bot::on_emoji_updated(std::function<void(Emoji&)> callback)
  {
    m_on_emoji_updated = callback;
  }

  void Bot::on_guild_created(std::function<void(Guild&)> callback) 
  {
	  m_on_guild_created = callback;
  }

  void Bot::on_presence(std::function<void(Presence&)> callback)
  {
    m_on_presence = callback;
  }

  void Bot::on_typing(std::function<void(TypingEvent&)> callback)
  {
    m_on_typing = callback;
  }

  void Bot::add_command(std::string name, std::function<void(MessageEvent)> callback)
  {
    m_commands[name] = callback;
  }
}

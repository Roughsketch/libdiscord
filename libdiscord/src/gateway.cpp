#include "gateway.h"

#include "api.h"
#include "bot.h"
#include "channel.h"
#include "easylogging++.h"
#include "event/general.h"
#include "event/guild_event.h"
#include "event/message_event.h"
#include "guild.h"
#include "member.h"
#include "role.h"
#include "user.h"

#include <cpprest/http_msg.h>
#include <zlib.h>

namespace discord
{
  const uint8_t gateway::LARGE_SERVER = 100;
  const utility::string_t gateway::VERSION = utility::string_t(U("6"));
  const utility::string_t gateway::ENCODING = utility::string_t(U("json"));

  void gateway::connect()
  {
    m_client.connect(m_wss_url);
  }

  void gateway::on_message(web::websockets::client::websocket_incoming_message msg)
  {
    std::string str;

    //  If the message is binary data, then we need to decompress it using ZLib.
    if (msg.message_type() == web::websockets::client::websocket_message_type::binary_message)
    {
      Concurrency::streams::container_buffer<std::string> strbuf;
      std::string compressed;

      //  Read the entire binary payload and put into a string container
      compressed = msg.body().read_to_end(strbuf).then([strbuf](size_t bytesRead)
      {
        return strbuf.collection();
      }).get();

      z_stream zs;
      memset(&zs, 0, sizeof(zs));

      if (inflateInit(&zs) != Z_OK)
      {
        LOG(ERROR) << "Could not initialize zlib Inflate";
      }

      zs.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(compressed.data()));
      zs.avail_in = static_cast<uInt>(compressed.size());

      int ret;
      char buffer[32768];

      do
      {
        zs.next_out = reinterpret_cast<Bytef *>(buffer);
        zs.avail_out = sizeof(buffer);

        ret = inflate(&zs, 0);

        if (str.size() < zs.total_out)
        {
          str.append(buffer, zs.total_out - str.size());
        }
      } while (ret == Z_OK);

      inflateEnd(&zs);

      if (ret != Z_STREAM_END)
      {
        LOG(ERROR) << "Error during zlib decompression: (" << ret << ")";
      }
    }
    else
    {
      //  If not compressed, just get the string.
      str = msg.extract_string().get();
    }

    //  Parse our payload as JSON.
    rapidjson::Document payload;
    payload.Parse(str.c_str());

    if (str.size() > 1000)
    {
      LOG(DEBUG) << "Got WS Payload: " << str.substr(0, 1000);
    }
    else
    {
      LOG(DEBUG) << "Got WS Payload: " << str;
    }

    rapidjson::Value& data = payload["d"]; //  Get the data for the event

    switch (payload["op"].GetInt())
    {
    case Dispatch:
      m_last_seq = payload["s"].GetInt();
      handle_dispatch_event(payload["t"].GetString(), data);
      break;
    case Reconnect:
      send_resume();
      break;
    case Hello:
      m_connected = true;
      m_heartbeat_interval = data["heartbeat_interval"].GetInt();
      LOG(DEBUG) << "Set heartbeat interval to " << m_heartbeat_interval;

      if (m_use_resume)
      {
        LOG(INFO) << "Connected again, sending Resume packet.";
        send_resume();
      }
      else
      {
        m_heartbeat_thread = std::thread([&]() {
          while (connected())
          {
            send_heartbeat();
            std::this_thread::sleep_for(std::chrono::milliseconds(m_heartbeat_interval));
          }

          LOG(DEBUG) << "Disconnected, stopping heartbeats.";
        });

        LOG(DEBUG) << "Connected, sending Identify packet.";
        send_identify();
        m_use_resume = true;  //  Next time use Resume
      }
      break;
    case Heartbeat_ACK:
      LOG(TRACE) << "Recieved Heartbeat ACK.";
      m_recieved_ack = true;
      break;
    default:
      LOG(WARNING) << "Unhandled WS Opcode (" << static_cast<int>(payload["op"].GetInt()) << ")";
    }
  }

  void gateway::handle_dispatch_event(std::string event_name, rapidjson::Value& data)
  {
    if (event_name == "READY")
    {
      LOG(DEBUG) << "Using gateway version " << data["v"];

      //  Save session id so we can restart a session
      m_session_id = data["session_id"].GetString();

      handle_dispatch(event_name, data);
    }
    else if (event_name == "RESUMED")
    {
      LOG(DEBUG) << "Successfully resumed.";
    }
    else
    {
      handle_dispatch(event_name, data);
    }
  }

  void gateway::send(Opcode op, rapidjson::Value& packet)
  {
    rapidjson::Document payload;

    payload["op"].SetInt(op);
    payload.AddMember("d", packet, payload.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    payload.Accept(writer);

    auto payload_str = buffer.GetString();

    web::websockets::client::websocket_outgoing_message msg;
    msg.set_utf8_message(payload_str);

    LOG(DEBUG) << "Sending packet: " << payload_str;

    try
    {
      m_client.send(msg);
    }
    catch (web::websockets::client::websocket_exception& e)
    {
      LOG(ERROR) << "WS Exception: " << e.what();
    }
  }

  void gateway::send_heartbeat()
  {
    if (!m_recieved_ack)
    {
      LOG(WARNING) << "Did not recieve a heartbeat ACK packet before this heartbeat.";
    }

    LOG(DEBUG) << "Sending heartbeat packet.";

    send(Heartbeat, rapidjson::Value(m_last_seq).Move());
    m_recieved_ack = false;
  }

  void gateway::send_identify()
  {
    LOG(DEBUG) << "Sending identify packet.";

    //  TODO: Convert this to RapidJSON.
    /*
    send(Identify,
    {
      { "token", m_bot->token() },
      {
        "properties",
        {
          { "$os", "windows" },
          { "$browser", "Discord" },
          { "$device", "Discord" },
          { "$referrer", "" },
          { "$refferring_domain", "" }
        }
      },
      { "compress", true },
      { "large_threshold", LARGE_SERVER },
      { "shard", nlohmann::json::array({ 0, 1 }) }
    });
    */
  }

  void gateway::send_resume()
  {
    LOG(DEBUG) << "Sending resume packet.";

    rapidjson::Value payload;

    payload["token"].SetString(m_bot.token.c_str(), m_bot.token.size());
    payload["sesson_id"].SetString(m_session_id.c_str(), m_session_id.size());
    payload["seq"].SetInt(m_last_seq);
    send(Resume, payload);
  }

  gateway::gateway(bot& owner, bot_data& bot) : m_owner(owner), m_bot(bot)
  {
    m_heartbeat_interval = 0;
    m_recieved_ack = true; // Set true to start because first hearbeat sent doesn't require an ACK.
    m_last_seq = 0;
    m_connected = false;
    m_use_resume = false;
  }

  void gateway::start()
  {
    web::uri_builder builder(U(""));
    builder.append_query(U("v"), VERSION);
    builder.append_query(U("encoding"), ENCODING);

    if (m_wss_url.empty())
    {
      do
      {
        try
        {
          //  Attempt to get the Gateway URL.
          m_wss_url = utility::conversions::to_string_t(api::get_wss_url());
        }
        catch (const web::http::http_exception& e)
        {
          LOG(ERROR) << "Exception getting gateway URL: " << e.what();
          LOG(ERROR) << "Sleeping for 5 seconds and trying again.";
          std::this_thread::sleep_for(std::chrono::seconds(5));
        }
      } while (m_wss_url.empty());  //  Keep trying until we get it.

      m_wss_url += builder.to_string();
    }

    m_client.set_message_handler([&](web::websockets::client::websocket_incoming_message msg)
    {
      try
      {
        on_message(msg);
      }
      catch (const std::exception& e)
      {
        LOG(ERROR) << "WebSocket Exception: " << e.what();
      }
    });

    m_client.set_close_handler([&](web::websockets::client::websocket_close_status status, const utility::string_t& reason, const std::error_code& code)
    {
      if (m_connected)
      {
        LOG(ERROR) << "WebSocket connection has closed with reason "
          << utility::conversions::to_utf8string(reason) << " - "
          << code.message() << " (" << code.value() << ")";
        m_connected = false;
        connect();
      }
    });

    connect();
  }

  bool gateway::connected() const
  {
    return m_connected;
  }

  void gateway::handle_dispatch(std::string event_name, rapidjson::Value& data)
  {
    LOG(INFO) << "Bot.handle_dispatch entered with " << event_name.c_str() << ".";

    if (event_name == "READY")
    {
      m_bot.profile = std::make_unique<user>(m_owner, data["user"]);

      for (auto& channel_data : data["private_channels"].GetArray())
      {
        snowflake id(channel_data["id"].GetString());
        m_bot.private_channels[id] = channel(m_owner, channel_data);
      }
    }
    else if (event_name == "CHANNEL_CREATE")
    {
      channel chan(m_owner, data);
      auto guild_id = chan.guild_id();

      auto owner = m_bot.guilds.find(guild_id);

      if (owner == std::end(m_bot.guilds))
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
      channel chan(m_owner, data);
      auto guild_id = chan.guild_id();

      auto owner = m_bot.guilds.find(guild_id);

      if (owner == std::end(m_bot.guilds))
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
      channel chan(m_owner, data);
      auto guild_id = chan.guild_id();

      auto owner = m_bot.guilds.find(guild_id);

      if (owner == std::end(m_bot.guilds))
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
      guild new_guild(m_owner, data);
      m_bot.guilds[new_guild.id()] = new_guild;
    }
    else if (event_name == "GUILD_UPDATE")
    {
      guild updated(m_owner, data);
      m_bot.guilds[updated.id()] = updated;
    }
    else if (event_name == "GUILD_DELETE")
    {
      snowflake id(data["id"].GetString());

      if (data.FindMember("unavailable") != data.MemberEnd())
      {
        m_bot.guilds[id].set_unavailable(true);
      }
      else
      {
        m_bot.guilds.erase(id);
      }
    }
    else if (event_name == "GUILD_BAN_ADD")
    {
      user banned(m_owner, data);
      snowflake guild_id(data["guild_id"].GetString());
      
      LOG(DEBUG)  << "User " << banned.distinct() 
                  << " has been banned from " 
                  << m_bot.guilds[guild_id].name() << ".";
    }
    else if (event_name == "GUILD_BAN_REMOVE")
    {
      user unbanned(m_owner, data);
      snowflake guild_id(data["guild_id"].GetString());
      LOG(DEBUG)  << "User " << unbanned.distinct() 
                  << " has been unbanned from " 
                  << m_bot.guilds[guild_id].name();
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
      m_bot.guilds[guild_id].add_member(member(m_owner, data));
    }
    else if (event_name == "GUILD_MEMBER_REMOVE")
    {
      snowflake guild_id(data["guild_id"].GetString());
      m_bot.guilds[guild_id].remove_member(member(m_owner, data));
    }
    else if (event_name == "GUILD_MEMBER_UPDATE")
    {
      snowflake guild_id(data["guild_id"].GetString());

      std::vector<snowflake> roles;
      user user(m_owner, data["user"]);
      std::string nick = data["nick"].GetString();

      for (auto& role_id : data["roles"].GetArray())
      {
        roles.push_back(snowflake(role_id.GetString()));
      }

      m_bot.guilds[guild_id].update_member(roles, user, nick);
    }
    else if (event_name == "GUILD_MEMBERS_CHUNK")
    {
      snowflake guild_id(data["guild_id"].GetString());
      auto owner = m_bot.guilds[guild_id];

      for (auto& member_data : data["members"].GetArray())
      {
        owner.add_member(member(m_owner, member_data));
      }
    }
    else if (event_name == "GUILD_ROLE_CREATE")
    {
      snowflake guild_id(data["guild_id"].GetString());
      m_bot.guilds[guild_id].add_role(role(data["role"]));
    }
    else if (event_name == "GUILD_ROLE_UPDATE")
    {
      snowflake guild_id(data["guild_id"].GetString());
      m_bot.guilds[guild_id].update_role(role(data["role"]));
    }
    else if (event_name == "GUILD_ROLE_DELETE")
    {
      snowflake guild_id(data["guild_id"].GetString());
      m_bot.guilds[guild_id].remove_role(role(data["role"]));
    }
    else if (event_name == "MESSAGE_CREATE")
    {
      auto event = message_event(m_owner, data);
      auto word = event.content().substr(0, event.content().find_first_of(" \n"));

      //  If we have a prefix and it's the start of this message and it's a command
      if (!m_bot.prefix.empty() &&
        word.size() > m_bot.prefix.size() &&
        (word.compare(0, m_bot.prefix.size(), m_bot.prefix) == 0) &&
        m_bot.commands.count(word.substr(m_bot.prefix.size()))
        )
      {
        //  Call the command
        m_bot.commands[word.substr(m_bot.prefix.size())](event);
      }
      else if (m_bot.on_message)
      {
        //  Not a command, but if we have an OnMessage handler call that instead.
        m_bot.on_message(event);
      }
    }
    else if (event_name == "MESSAGE_UPDATE")
    {
      if (m_bot.on_message_edited)
      {
        m_bot.on_message_edited(message_event(m_owner, data));
      }
    }
    else if (event_name == "MESSAGE_DELETE")
    {
      if (m_bot.on_message_deleted)
      {
        m_bot.on_message_deleted(message_deleted_event(m_owner, data));
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

      if (m_bot.on_message_deleted)
      {
        for (auto& id : ids)
        {
          m_bot.on_message_deleted(message_deleted_event(m_owner, id, chan_id));
        }
      }
    }
    else if (event_name == "PRESENCE_UPDATE")
    {
      presence_event presence(m_owner, data);
      snowflake guild_id(data["guild_id"].GetString());

      m_bot.guilds[guild_id].update_presence(presence);

      if (m_bot.on_presence)
      {
        m_bot.on_presence(presence);
      }
    }
    else if (event_name == "TYPING_START")
    {
      if (m_bot.on_typing)
      {
        m_bot.on_typing(typing_event(m_owner, data));
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

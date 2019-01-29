#include "api/user_api.h"
#include "connection_state.h"
#include "discord_exception.h"

namespace discord
{
  namespace api
  {
    namespace user
    {
      pplx::task<User> get_current_user(ConnectionState* conn)
      {
        return conn->request(User_Me, 0, Method::GET, "users/@me")
        .then([conn](APIResponse response)
        {
          return User(conn, response.data);
        });
      }

      pplx::task<User> get_user(ConnectionState* conn, Snowflake user_id)
      {
        return conn->request(User_UID, 0, Method::GET, "users/" + user_id.to_string())
        .then([conn](APIResponse response)
        {
          return User(conn, response.data);
        });
      }

      pplx::task<User> modify(ConnectionState* conn, std::string username, std::string avatar)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("username");
        writer.String(username);

        writer.EndObject();

        return conn->request(User_Me, 0, Method::PATCH, "users/@me", sb.GetString())
        .then([conn](APIResponse response)
        {
          return User(conn, response.data);
        });
      }

      pplx::task<std::vector<user_guild>> guilds(ConnectionState* conn, uint32_t limit, SearchMethod method, Snowflake guild_id)
      {
        if (limit == 0)
        {
          throw DiscordException("Invalid limit of 0 passed to guilds endpoint.");
        }

        if (limit > 100)
        {
          LOG(WARNING) << "Reducing guild limit of " << limit << " to 100 to fit request.";
          limit = 100;
        }

        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("limit");
        writer.Int(limit);

        if (guild_id.id() != 0)
        {
          switch (method)
          {
          case SearchMethod::After:
            writer.String("after");
            writer.String(guild_id.to_string());
            break;
          case SearchMethod::Before:
            writer.String("before");
            writer.String(guild_id.to_string());
            break;
          default:
            break;
          }
        }

        writer.EndObject();

        return conn->request(User_Me_Guild, 0, Method::GET, "users/@me/guilds", sb.GetString())
        .then([](APIResponse response)
        {
          std::vector<user_guild> user_guilds;

          for (auto& ug_data : response.data.GetArray())
          {
            user_guilds.emplace_back(ug_data);
          }

          return user_guilds;
        });
      }

      pplx::task<bool> leave_guild(ConnectionState* conn, Snowflake guild_id)
      {
        return conn->request(User_Me_Guild_GID, 0, Method::DEL, "users/@me/guilds/" + guild_id.to_string())
        .then([](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<std::vector<Channel>> get_dms(ConnectionState* conn)
      {
        return conn->request(User_Me_Channel, 0, Method::GET, "users/@me/channels")
        .then([conn](APIResponse response)
        {
          std::vector<Channel> channels;

          for (auto& chan_data : response.data.GetArray())
          {
            channels.emplace_back(conn, chan_data);
          }

          return channels;
        });
      }

      pplx::task<Channel> create_dm(ConnectionState* conn, Snowflake recipient_id)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("recipient_id");
        writer.String(recipient_id.to_string());

        writer.EndObject();

        return conn->request(User_Me_Channel, 0, Method::POST, "users/@me/channels", sb.GetString())
        .then([conn](APIResponse response)
        {
          return Channel(conn, response.data);
        });
      }

      pplx::task<Channel> create_group_dm(ConnectionState* conn, std::vector<std::string> access_tokens, std::map<Snowflake, std::string> user_nicknames)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        for (const auto& pair : user_nicknames)
        {
          writer.String(pair.first.to_string());
          writer.String(pair.second);
        }

        writer.EndObject();

        return conn->request(User_Me_Channel, 0, Method::POST,
          "users/@me/channels", sb.GetString())
        .then([conn](APIResponse response)
        {
          return Channel(conn, response.data);
        });
      }

      pplx::task<std::vector<Connection>> connections(ConnectionState* conn)
      {
        return conn->request(User_Me_Connections, 0, Method::GET, "users/@me/connections")
        .then([](APIResponse response)
        {
          std::vector<Connection> connections;

          for (auto& conn_data : response.data.GetArray())
          {
            connections.emplace_back(conn_data);
          }

          return connections;
        });
      }
    }
  }
}

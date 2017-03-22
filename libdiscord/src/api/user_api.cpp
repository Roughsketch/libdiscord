#include "api/user_api.h"
#include "connection_state.h"
#include "discord_exception.h"

namespace discord
{
  namespace api
  {
    namespace user
    {
      pplx::task<discord::user> get_current_user(connection_state* conn)
      {
        return conn->request(User_Me, 0, method::GET, "users/@me")
        .then([conn](api_response response)
        {
          return discord::user(conn, response.data);
        });
      }

      pplx::task<discord::user> get_user(connection_state* conn, snowflake user_id)
      {
        return conn->request(User_UID, 0, method::GET, "users/" + user_id.to_string())
        .then([conn](api_response response)
        {
          return discord::user(conn, response.data);
        });
      }

      pplx::task<discord::user> modify(connection_state* conn, std::string username, std::string avatar)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("username");
        writer.String(username);

        writer.EndObject();

        return conn->request(User_Me, 0, method::PATCH, "users/@me", sb.GetString())
        .then([conn](api_response response)
        {
          return discord::user(conn, response.data);
        });
      }

      pplx::task<std::vector<user_guild>> guilds(connection_state* conn, uint32_t limit, search_method method, snowflake guild_id)
      {
        if (limit == 0)
        {
          throw discord_exception("Invalid limit of 0 passed to guilds endpoint.");
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
          case search_method::After:
            writer.String("after");
            writer.String(guild_id.to_string());
            break;
          case search_method::Before:
            writer.String("before");
            writer.String(guild_id.to_string());
            break;
          default:
            break;
          }
        }

        writer.EndObject();

        return conn->request(User_Me_Guild, 0, method::GET, "users/@me/guilds", sb.GetString())
        .then([](api_response response)
        {
          std::vector<user_guild> user_guilds;

          for (auto& ug_data : response.data.GetArray())
          {
            user_guilds.emplace_back(user_guild(ug_data));
          }

          return user_guilds;
        });
      }

      pplx::task<bool> leave_guild(connection_state* conn, snowflake guild_id)
      {
        return conn->request(User_Me_Guild_GID, 0, method::DEL, "users/@me/guilds/" + guild_id.to_string())
        .then([](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<std::vector<channel>> get_dms(connection_state* conn)
      {
        return conn->request(User_Me_Channel, 0, method::GET, "users/@me/channels")
        .then([conn](api_response response)
        {
          std::vector<channel> channels;

          for (auto& chan_data : response.data.GetArray())
          {
            channels.emplace_back(channel(conn, chan_data));
          }

          return channels;
        });
      }

      pplx::task<channel> create_dm(connection_state* conn, snowflake recipient_id)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("recipient_id");
        writer.String(recipient_id.to_string());

        writer.EndObject();

        return conn->request(User_Me_Channel, 0, method::POST, "users/@me/channels", sb.GetString())
        .then([conn](api_response response)
        {
          return channel(conn, response.data);
        });
      }

      pplx::task<channel> create_group_dm(connection_state* conn, std::vector<std::string> access_tokens, std::map<snowflake, std::string> user_nicknames)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        for (auto& pair : user_nicknames)
        {
          writer.String(pair.first.to_string());
          writer.String(pair.second);
        }

        writer.EndObject();

        return conn->request(User_Me_Channel, 0, method::POST,
          "users/@me/channels", sb.GetString())
        .then([conn](api_response response)
        {
          return channel(conn, response.data);
        });
      }

      pplx::task<std::vector<connection>> connections(connection_state* conn)
      {
        return conn->request(User_Me_Connections, 0, method::GET, "users/@me/connections")
        .then([](api_response response)
        {
          std::vector<connection> connections;

          for (auto& conn_data : response.data.GetArray())
          {
            connections.emplace_back(connection(conn_data));
          }

          return connections;
        });
      }
    }
  }
}

#include "api/guild_api.h"
#include "api.h"
#include "connection_state.h"
#include "integration.h"

namespace discord
{
  namespace api
  {
    namespace guild
    {
      pplx::task<discord::guild> modify(connection_state* conn, snowflake guild_id, discord::guild guild)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("name");
        writer.String(guild.name());
        writer.String("region");
        writer.String(guild.region());
        writer.String("verification_level");
        writer.Int(static_cast<int>(guild.verify_level()));
        writer.String("default_message_notifications");
        writer.Int(static_cast<int>(guild.notify_level()));
        writer.String("afk_channel_id");
        writer.String(guild.afk_channel_id().to_string());
        writer.String("afk_timeout");
        writer.Int(guild.afk_timeout());
        writer.String("owner_id");
        writer.String(guild.owner_id().to_string());

        writer.EndObject();

        return conn->request(Guild_GID, guild_id, method::PATCH, "guilds/" + guild_id.to_string(), sb.GetString())
        .then([conn](api_response response)
        {
          return discord::guild(conn, response.data);
        });
      }

      pplx::task<discord::guild> remove(connection_state* conn, snowflake guild_id)
      {
        return conn->request(Guild_GID, guild_id, method::DEL, "guilds/" + guild_id.to_string())
        .then([conn](api_response response)
        {
          return discord::guild(conn, response.data);
        });
      }

      pplx::task<std::vector<discord::channel>> get_channels(connection_state* conn, snowflake guild_id)
      {
        return conn->request(Guild_GID_Chan, guild_id, method::GET, "guilds/" + guild_id.to_string() + "/channels")
        .then([conn](api_response response)
        {
          std::vector<channel> channels;

          for (auto& chan_data : response.data.GetArray())
          {
            channels.emplace_back(conn, chan_data);
          }

          return channels;
        });
      }

      pplx::task<discord::channel> create_text_channel(connection_state* conn, snowflake guild_id, std::string name, std::vector<overwrite> permission_overwrites)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("name");
        writer.String(name);

        writer.String("type");
        writer.String("text");

        writer.String("permission_overwrites");
        writer.StartArray();

        for (const auto& ow : permission_overwrites)
        {
          writer.String(ow.id().to_string());
        }

        writer.EndArray();
        writer.EndObject();

        return conn->request(Guild_GID_Chan, guild_id, method::POST, 
          "guilds/" + guild_id.to_string() + "/channels", sb.GetString())
        .then([conn](api_response response)
        {
          return channel(conn, response.data);
        });
      }

      pplx::task<discord::channel> create_voice_channel(connection_state* conn, snowflake guild_id, std::string name, uint32_t bitrate, uint32_t user_limit, std::vector<overwrite> permission_overwrites)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("name");
        writer.String(name);

        writer.String("type");
        writer.String("voice");

        writer.String("bitrate");
        writer.Int(bitrate);

        writer.String("user_limit");
        writer.Int(user_limit);

        writer.EndObject();

        return conn->request(Guild_GID_Chan, guild_id, method::POST,
          "guilds/" + guild_id.to_string() + "/channels", sb.GetString())
        .then([conn](api_response response)
        {
          return channel(conn, response.data);
        });
      }

      pplx::task<std::vector<discord::channel>> modify_channel_positions(connection_state* conn, snowflake guild_id, const std::map<snowflake, uint32_t>& positions)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();
        writer.StartArray();

        for(auto& pair : positions)
        {
          writer.String(pair.first.to_string());
          writer.Int(pair.second);
        }

        writer.EndArray();
        writer.EndObject();

        return conn->request(Guild_GID_Chan, guild_id, method::PATCH,
          "guilds/" + guild_id.to_string() + "/channels", sb.GetString())
        .then([conn](api_response response)
        {
          std::vector<channel> channels;

          for (auto& chan_data : response.data.GetArray())
          {
            channels.emplace_back(conn, chan_data);
          }
          
          return channels;
        });
      }

      pplx::task<member> get_member(connection_state* conn, snowflake guild_id, snowflake user_id)
      {
        return conn->request(Guild_GID_Mem_UID, guild_id, method::GET,
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string())
        .then([conn](api_response response)
        {
          return member(conn, response.data);
        });
      }

      pplx::task<std::vector<member>> get_members(connection_state* conn, snowflake guild_id, uint32_t limit, snowflake after)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("limit");
        writer.Int(limit);

        writer.String("after");
        writer.String(after.to_string());

        writer.EndObject();

        return conn->request(Guild_GID_Mem, guild_id, method::GET,
          "guilds/" + guild_id.to_string() + "/members", sb.GetString())
        .then([conn](api_response response)
        {
          std::vector<member> members;

          for (auto& member_data : response.data.GetArray())
          {
            members.emplace_back(conn, member_data);
          }

          return members;
        });
      }

      pplx::task<bool> add_member(connection_state* conn, snowflake guild_id, snowflake user_id, std::string access_token, std::string nick, std::vector<role> roles, bool muted, bool deafened)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("access_token");
        writer.String(access_token);

        writer.String("nick");
        writer.String(nick);

        writer.String("roles");
        writer.StartArray();

        for (const auto& role : roles)
        {
          writer.String(role.id().to_string());
        }

        writer.EndArray();

        writer.String("mute");
        writer.Bool(muted);

        writer.String("deaf");
        writer.Bool(deafened);

        writer.EndObject();

        return conn->request(Guild_GID_Mem_UID, guild_id, method::PUT, 
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string(), sb.GetString())
        .then([](api_response response)
        {
          return response.status_code == 201;
        });
      }

      pplx::task<bool> modify_member(connection_state* conn, snowflake guild_id, snowflake user_id, std::string nick, std::vector<role> roles, bool muted, bool deafened, snowflake channel_id)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("nick");
        writer.String(nick);

        writer.String("roles");
        writer.StartArray();

        for (const auto& role : roles)
        {
          writer.String(role.id().to_string());
        }

        writer.EndArray();

        writer.String("mute");
        writer.Bool(muted);

        writer.String("deaf");
        writer.Bool(deafened);

        if (channel_id.id() != 0)
        {
          writer.String("channel_id");
          writer.String(channel_id.to_string());
        }

        writer.EndObject();

        return conn->request(Guild_GID_Mem_UID, guild_id, method::PATCH,
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string(), sb.GetString())
        .then([](api_response response)
        {
          return response.status_code == 201;
        });
      }

      pplx::task<bool> set_nickname(connection_state* conn, snowflake guild_id, std::string nick)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("nick");
        writer.String(nick);

        writer.EndObject();

        return conn->request(Guild_GID_Mem_Me_Nick, guild_id, method::PATCH, 
          "guilds/" + guild_id.to_string() + "/members/@me/nick", sb.GetString())
        .then([](api_response response)
        {
          return response.status_code == 200;
        });
      }

      pplx::task<bool> add_member_role(connection_state* conn, snowflake guild_id, snowflake user_id, snowflake role_id)
      {
        return conn->request(Guild_GID_Mem_UID_Role_RID, guild_id, method::PUT,
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string() + "/roles/" + role_id.to_string())
        .then([](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_member_role(connection_state* conn, snowflake guild_id, snowflake user_id, snowflake role_id)
      {
        return conn->request(Guild_GID_Mem_UID_Role_RID, guild_id, method::DEL,
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string() + "/roles/" + role_id.to_string())
        .then([](api_response response)
        {
          return response.status_code == 204;
        });;
      }

      pplx::task<bool> remove_member(connection_state* conn, snowflake guild_id, snowflake user_id)
      {
        return conn->request(Guild_GID_Mem_UID, guild_id, method::DEL,
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string())
        .then([](api_response response)
        {
          return response.status_code == 204;
        });;
      }

      pplx::task<std::vector<discord::user>> get_bans(connection_state* conn, snowflake guild_id)
      {
        return conn->request(Guild_GID_Bans, guild_id, method::GET,
          "guilds/" + guild_id.to_string() + "/bans")
        .then([conn](api_response response)
        {
          std::vector<user> users;

          for (auto& user_data : response.data.GetArray())
          {
            users.emplace_back(conn, user_data);
          }

          return users;
        });
      }

      pplx::task<bool> ban(connection_state* conn, snowflake guild_id, snowflake user_id, uint32_t delete_x_days)
      {
        if (delete_x_days > 7)
        {
          delete_x_days = 7;
        }

        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("delete-message-days");
        writer.Int(delete_x_days);

        writer.EndObject();

        return conn->request(Guild_GID_Bans_UID, guild_id, method::PUT, 
          "guilds/" + guild_id.to_string() + "/bans/" + user_id.to_string(), sb.GetString())
        .then([](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> unban(connection_state* conn, snowflake guild_id, snowflake user_id)
      {
        return conn->request(Guild_GID_Bans_UID, guild_id, method::DEL,
          "guilds/" + guild_id.to_string() + "/bans/" + user_id.to_string())
        .then([](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<std::vector<role>> get_roles(connection_state* conn, snowflake guild_id)
      {
        return conn->request(Guild_GID_Roles, guild_id, method::GET,
          "guilds/" + guild_id.to_string() + "/roles")
        .then([](api_response response)
        {
          std::vector<role> roles;

          for (auto& role_data : response.data.GetArray())
          {
            roles.emplace_back(role_data);
          }

          return roles;
        });
      }

      pplx::task<role> create_role(connection_state* conn, snowflake guild_id, std::string name, permission permissions, uint32_t rgb_color, bool hoist, bool mentionable)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("name");
        writer.String(name);

        writer.String("permissions");
        writer.Int(permissions.get());

        writer.String("color");
        writer.Int(rgb_color);

        writer.String("hoist");
        writer.Bool(hoist);

        writer.String("mentionable");
        writer.Bool(mentionable);

        writer.EndObject();

        return conn->request(Guild_GID_Roles, guild_id, method::POST, 
          "guilds/" + guild_id.to_string() + "/roles", sb.GetString())
        .then([](api_response response)
        {
          return role(response.data);
        });
      }

      pplx::task<std::vector<role>> modify_role_positions(connection_state* conn, snowflake guild_id, const std::map<snowflake, uint32_t>& positions)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();
        writer.StartArray();

        for (const auto& pair : positions)
        {
          writer.String(pair.first.to_string());
          writer.Int(pair.second);
        }

        writer.EndArray();
        writer.EndObject();

        return conn->request(Guild_GID_Roles, guild_id, method::PATCH,
          "guilds/" + guild_id.to_string() + "/roles", sb.GetString())
        .then([](api_response response)
        {
          std::vector<role> roles;

          for (auto& role_data : response.data.GetArray())
          {
            roles.emplace_back(role_data);
          }

          return roles;
        });
      }

      pplx::task<role> modify_role(connection_state* conn, snowflake guild_id, snowflake role_id, std::string name, permission permissions, uint32_t rgb_color, bool hoist, bool mentionable)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("name");
        writer.String(name);

        writer.String("permissions");
        writer.Int(permissions.get());

        writer.String("color");
        writer.Int(rgb_color);

        writer.String("hoist");
        writer.Bool(hoist);

        writer.String("mentionable");
        writer.Bool(mentionable);

        writer.EndObject();

        return conn->request(Guild_GID_Roles_RID, guild_id, method::PATCH, 
          "guilds/" + guild_id.to_string() + "/roles/" + role_id.to_string(), sb.GetString())
        .then([](api_response response)
        {
          return role(response.data);
        });
      }

      pplx::task<bool> remove_role(connection_state* conn, snowflake guild_id, snowflake role_id)
      {
        return conn->request(Guild_GID_Roles_RID, guild_id, method::DEL,
          "guilds/" + guild_id.to_string() + "/roles/" + role_id.to_string())
        .then([](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<uint32_t> get_prune_count(connection_state* conn, snowflake guild_id, uint32_t days)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("days");
        writer.Int(days);

        writer.EndObject();

        return conn->request(Guild_GID_Prune, guild_id, method::GET, 
          "guilds/" + guild_id.to_string() + "/prune", sb.GetString())
        .then([](api_response response)
        {
          return response.data.GetUint();
        });
      }

      pplx::task<uint32_t> prune(connection_state* conn, snowflake guild_id, uint32_t days)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("days");
        writer.Int(days);

        writer.EndObject();

        return conn->request(Guild_GID_Prune, guild_id, method::POST, 
          "guilds/" + guild_id.to_string() + "/prune", sb.GetString())
        .then([](api_response response)
        {
          return response.data["pruned"].GetUint();
        });
      }

      pplx::task<std::vector<voice_region>> get_voice_regions(connection_state* conn, snowflake guild_id)
      {
        return conn->request(Guild_GID_Regions, guild_id, method::GET,
          "guilds/" + guild_id.to_string() + "/regions")
        .then([](api_response response)
        {
          std::vector<voice_region> regions;

          for (auto& region_data : response.data.GetArray())
          {
            regions.emplace_back(region_data);
          }

          return regions;
        });
      }

      pplx::task<std::vector<integration>> get_integrations(connection_state* conn, snowflake guild_id)
      {
        return conn->request(Guild_GID_Int, guild_id, method::GET,
          "guilds/" + guild_id.to_string() + "/integrations")
        .then([conn](api_response response)
        {
          std::vector<integration> integrations;

          for (auto& integration_data : response.data.GetArray())
          {
            integrations.emplace_back(conn, response.data);
          }

          return integrations;
        });
      }

      pplx::task<bool> create_integration(connection_state* conn, snowflake guild_id, std::string type, snowflake integration_id)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("type");
        writer.String(type);

        writer.String("id");
        writer.String(integration_id.to_string());

        writer.EndObject();

        return conn->request(Guild_GID_Int, guild_id, method::POST,
          "guilds/" + guild_id.to_string() + "/integrations", sb.GetString())
        .then([](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> modify_integration(connection_state* conn, snowflake guild_id, snowflake integration_id, uint32_t expire_behavior, uint32_t expire_grace_period, bool enable_emoticons)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("expire_behavior");
        writer.Int(expire_behavior);

        writer.String("expire_grace_period");
        writer.Int(expire_grace_period);

        writer.String("enable_emoticons");
        writer.Bool(enable_emoticons);

        writer.EndObject();

        return conn->request(Guild_GID_Int_IID, guild_id, method::PATCH,
          "guilds/" + guild_id.to_string() + "/integrations/" + integration_id.to_string(), sb.GetString())
        .then([](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_integration(connection_state* conn, snowflake guild_id, snowflake integration_id)
      {
        return conn->request(Guild_GID_Int_IID, guild_id, method::DEL,
          "guilds/" + guild_id.to_string() + "/integrations/" + integration_id.to_string())
        .then([](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> sync_integration(connection_state* conn, snowflake guild_id, snowflake integration_id)
      {
        return conn->request(Guild_GID_Int_IID_Sync, guild_id, method::POST,
          "guilds/" + guild_id.to_string() + "/integrations/" + integration_id.to_string() + "/sync")
        .then([](api_response response)
        {
          return response.status_code == 204;
        });
      }
    }
  }
}

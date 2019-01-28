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
      pplx::task<discord::Guild> modify(ConnectionState* conn, Snowflake guild_id, discord::Guild guild)
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

        return conn->request(Guild_GID, guild_id, Method::PATCH, "guilds/" + guild_id.to_string(), sb.GetString())
        .then([conn](APIResponse response)
        {
          return discord::Guild(conn, response.data);
        });
      }

      pplx::task<discord::Guild> remove(ConnectionState* conn, Snowflake guild_id)
      {
        return conn->request(Guild_GID, guild_id, Method::DEL, "guilds/" + guild_id.to_string())
        .then([conn](APIResponse response)
        {
          return discord::Guild(conn, response.data);
        });
      }

      pplx::task<std::vector<discord::Channel>> get_channels(ConnectionState* conn, Snowflake guild_id)
      {
        return conn->request(Guild_GID_Chan, guild_id, Method::GET, "guilds/" + guild_id.to_string() + "/channels")
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

      pplx::task<discord::Channel> create_text_channel(ConnectionState* conn, Snowflake guild_id, std::string name, std::vector<Overwrite> permission_overwrites)
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

        return conn->request(Guild_GID_Chan, guild_id, Method::POST, 
          "guilds/" + guild_id.to_string() + "/channels", sb.GetString())
        .then([conn](APIResponse response)
        {
          return Channel(conn, response.data);
        });
      }

      pplx::task<discord::Channel> create_voice_channel(ConnectionState* conn, Snowflake guild_id, std::string name, uint32_t bitrate, uint32_t user_limit, std::vector<Overwrite> permission_overwrites)
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

        return conn->request(Guild_GID_Chan, guild_id, Method::POST,
          "guilds/" + guild_id.to_string() + "/channels", sb.GetString())
        .then([conn](APIResponse response)
        {
          return Channel(conn, response.data);
        });
      }

      pplx::task<std::vector<discord::Channel>> modify_channel_positions(ConnectionState* conn, Snowflake guild_id, const std::map<Snowflake, uint32_t>& positions)
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

        return conn->request(Guild_GID_Chan, guild_id, Method::PATCH,
          "guilds/" + guild_id.to_string() + "/channels", sb.GetString())
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

      pplx::task<Member> get_member(ConnectionState* conn, Snowflake guild_id, Snowflake user_id)
      {
        return conn->request(Guild_GID_Mem_UID, guild_id, Method::GET,
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string())
        .then([conn](APIResponse response)
        {
          return Member(conn, response.data);
        });
      }

      pplx::task<std::vector<Member>> get_members(ConnectionState* conn, Snowflake guild_id, uint32_t limit, Snowflake after)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("limit");
        writer.Int(limit);

        writer.String("after");
        writer.String(after.to_string());

        writer.EndObject();

        return conn->request(Guild_GID_Mem, guild_id, Method::GET,
          "guilds/" + guild_id.to_string() + "/members", sb.GetString())
        .then([conn](APIResponse response)
        {
          std::vector<Member> members;

          for (auto& member_data : response.data.GetArray())
          {
            members.emplace_back(conn, member_data);
          }

          return members;
        });
      }

      pplx::task<bool> add_member(ConnectionState* conn, Snowflake guild_id, Snowflake user_id, std::string access_token, std::string nick, std::vector<Role> roles, bool muted, bool deafened)
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

        return conn->request(Guild_GID_Mem_UID, guild_id, Method::PUT, 
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string(), sb.GetString())
        .then([](APIResponse response)
        {
          return response.status_code == 201;
        });
      }

      pplx::task<bool> modify_member(ConnectionState* conn, Snowflake guild_id, Snowflake user_id, std::string nick, std::vector<Role> roles, bool muted, bool deafened, Snowflake channel_id)
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

        return conn->request(Guild_GID_Mem_UID, guild_id, Method::PATCH,
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string(), sb.GetString())
        .then([](APIResponse response)
        {
          return response.status_code == 201;
        });
      }

      pplx::task<bool> set_nickname(ConnectionState* conn, Snowflake guild_id, std::string nick)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("nick");
        writer.String(nick);

        writer.EndObject();

        return conn->request(Guild_GID_Mem_Me_Nick, guild_id, Method::PATCH, 
          "guilds/" + guild_id.to_string() + "/members/@me/nick", sb.GetString())
        .then([](APIResponse response)
        {
          return response.status_code == 200;
        });
      }

      pplx::task<bool> add_member_role(ConnectionState* conn, Snowflake guild_id, Snowflake user_id, Snowflake role_id)
      {
        return conn->request(Guild_GID_Mem_UID_Role_RID, guild_id, Method::PUT,
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string() + "/roles/" + role_id.to_string())
        .then([](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_member_role(ConnectionState* conn, Snowflake guild_id, Snowflake user_id, Snowflake role_id)
      {
        return conn->request(Guild_GID_Mem_UID_Role_RID, guild_id, Method::DEL,
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string() + "/roles/" + role_id.to_string())
        .then([](APIResponse response)
        {
          return response.status_code == 204;
        });;
      }

      pplx::task<bool> remove_member(ConnectionState* conn, Snowflake guild_id, Snowflake user_id)
      {
        return conn->request(Guild_GID_Mem_UID, guild_id, Method::DEL,
          "guilds/" + guild_id.to_string() + "/members/" + user_id.to_string())
        .then([](APIResponse response)
        {
          return response.status_code == 204;
        });;
      }

      pplx::task<std::vector<discord::User>> get_bans(ConnectionState* conn, Snowflake guild_id)
      {
        return conn->request(Guild_GID_Bans, guild_id, Method::GET,
          "guilds/" + guild_id.to_string() + "/bans")
        .then([conn](APIResponse response)
        {
          std::vector<User> users;

          for (auto& user_data : response.data.GetArray())
          {
            users.emplace_back(conn, user_data);
          }

          return users;
        });
      }

      pplx::task<bool> ban(ConnectionState* conn, Snowflake guild_id, Snowflake user_id, uint32_t delete_x_days)
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

        return conn->request(Guild_GID_Bans_UID, guild_id, Method::PUT, 
          "guilds/" + guild_id.to_string() + "/bans/" + user_id.to_string(), sb.GetString())
        .then([](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> unban(ConnectionState* conn, Snowflake guild_id, Snowflake user_id)
      {
        return conn->request(Guild_GID_Bans_UID, guild_id, Method::DEL,
          "guilds/" + guild_id.to_string() + "/bans/" + user_id.to_string())
        .then([](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<std::vector<Role>> get_roles(ConnectionState* conn, Snowflake guild_id)
      {
        return conn->request(Guild_GID_Roles, guild_id, Method::GET,
          "guilds/" + guild_id.to_string() + "/roles")
        .then([](APIResponse response)
        {
          std::vector<Role> roles;

          for (auto& role_data : response.data.GetArray())
          {
            roles.emplace_back(role_data);
          }

          return roles;
        });
      }

      pplx::task<Role> create_role(ConnectionState* conn, Snowflake guild_id, std::string name, Permission permissions, uint32_t rgb_color, bool hoist, bool mentionable)
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

        return conn->request(Guild_GID_Roles, guild_id, Method::POST, 
          "guilds/" + guild_id.to_string() + "/roles", sb.GetString())
        .then([](APIResponse response)
        {
          return Role(response.data);
        });
      }

      pplx::task<std::vector<Role>> modify_role_positions(ConnectionState* conn, Snowflake guild_id, const std::map<Snowflake, uint32_t>& positions)
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

        return conn->request(Guild_GID_Roles, guild_id, Method::PATCH,
          "guilds/" + guild_id.to_string() + "/roles", sb.GetString())
        .then([](APIResponse response)
        {
          std::vector<Role> roles;

          for (auto& role_data : response.data.GetArray())
          {
            roles.emplace_back(role_data);
          }

          return roles;
        });
      }

      pplx::task<Role> modify_role(ConnectionState* conn, Snowflake guild_id, Snowflake role_id, std::string name, Permission permissions, uint32_t rgb_color, bool hoist, bool mentionable)
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

        return conn->request(Guild_GID_Roles_RID, guild_id, Method::PATCH, 
          "guilds/" + guild_id.to_string() + "/roles/" + role_id.to_string(), sb.GetString())
        .then([](APIResponse response)
        {
          return Role(response.data);
        });
      }

      pplx::task<bool> remove_role(ConnectionState* conn, Snowflake guild_id, Snowflake role_id)
      {
        return conn->request(Guild_GID_Roles_RID, guild_id, Method::DEL,
          "guilds/" + guild_id.to_string() + "/roles/" + role_id.to_string())
        .then([](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<uint32_t> get_prune_count(ConnectionState* conn, Snowflake guild_id, uint32_t days)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("days");
        writer.Int(days);

        writer.EndObject();

        return conn->request(Guild_GID_Prune, guild_id, Method::GET, 
          "guilds/" + guild_id.to_string() + "/prune", sb.GetString())
        .then([](APIResponse response)
        {
          return response.data.GetUint();
        });
      }

      pplx::task<uint32_t> prune(ConnectionState* conn, Snowflake guild_id, uint32_t days)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("days");
        writer.Int(days);

        writer.EndObject();

        return conn->request(Guild_GID_Prune, guild_id, Method::POST, 
          "guilds/" + guild_id.to_string() + "/prune", sb.GetString())
        .then([](APIResponse response)
        {
          return response.data["pruned"].GetUint();
        });
      }

      pplx::task<std::vector<VoiceRegion>> get_voice_regions(ConnectionState* conn, Snowflake guild_id)
      {
        return conn->request(Guild_GID_Regions, guild_id, Method::GET,
          "guilds/" + guild_id.to_string() + "/regions")
        .then([](APIResponse response)
        {
          std::vector<VoiceRegion> regions;

          for (auto& region_data : response.data.GetArray())
          {
            regions.emplace_back(region_data);
          }

          return regions;
        });
      }

      pplx::task<std::vector<Integration>> get_integrations(ConnectionState* conn, Snowflake guild_id)
      {
        return conn->request(Guild_GID_Int, guild_id, Method::GET,
          "guilds/" + guild_id.to_string() + "/integrations")
        .then([conn](APIResponse response)
        {
          std::vector<Integration> integrations;

          for (auto& integration_data : response.data.GetArray())
          {
            integrations.emplace_back(conn, response.data);
          }

          return integrations;
        });
      }

      pplx::task<bool> create_integration(ConnectionState* conn, Snowflake guild_id, std::string type, Snowflake integration_id)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("type");
        writer.String(type);

        writer.String("id");
        writer.String(integration_id.to_string());

        writer.EndObject();

        return conn->request(Guild_GID_Int, guild_id, Method::POST,
          "guilds/" + guild_id.to_string() + "/integrations", sb.GetString())
        .then([](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> modify_integration(ConnectionState* conn, Snowflake guild_id, Snowflake integration_id, uint32_t expire_behavior, uint32_t expire_grace_period, bool enable_emoticons)
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

        return conn->request(Guild_GID_Int_IID, guild_id, Method::PATCH,
          "guilds/" + guild_id.to_string() + "/integrations/" + integration_id.to_string(), sb.GetString())
        .then([](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_integration(ConnectionState* conn, Snowflake guild_id, Snowflake integration_id)
      {
        return conn->request(Guild_GID_Int_IID, guild_id, Method::DEL,
          "guilds/" + guild_id.to_string() + "/integrations/" + integration_id.to_string())
        .then([](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> sync_integration(ConnectionState* conn, Snowflake guild_id, Snowflake integration_id)
      {
        return conn->request(Guild_GID_Int_IID_Sync, guild_id, Method::POST,
          "guilds/" + guild_id.to_string() + "/integrations/" + integration_id.to_string() + "/sync")
        .then([](APIResponse response)
        {
          return response.status_code == 204;
        });
      }
    }
  }
}

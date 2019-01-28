#include <cpprest/http_client.h>
#include "api/channel_api.h"
#include "connection_state.h"
#include "emoji.h"
#include "discord_exception.h"

namespace discord
{
  namespace api
  {
    namespace channel
    {
      pplx::task<Channel> modify_text_channel(ConnectionState* conn, Snowflake channel_id, std::string name, int32_t position, std::string topic)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("name");
        writer.String(name);

        writer.String("topic");
        writer.String(topic);

        writer.String("position");
        writer.Int(position);

        writer.EndObject();

        return conn->request(Chan_CID, channel_id, Method::PATCH, "channels/" + channel_id.to_string(), sb.GetString())
        .then([conn](APIResponse response)
        {
          return Channel(conn, response.data);
        });
      }

      pplx::task<Channel> modify_voice_channel(ConnectionState* conn, Snowflake channel_id, std::string name, int32_t position, uint32_t bitrate, uint32_t user_limit)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("name");
        writer.String(name);

        writer.String("bitrate");
        writer.Uint(bitrate);

        writer.String("position");
        writer.Int(position);

        writer.String("user_limit");
        writer.Uint(user_limit);

        writer.EndObject();

        return conn->request(Chan_CID, channel_id, Method::PATCH, "channels/" + channel_id.to_string(), sb.GetString())
        .then([conn](APIResponse response)
        {
          return Channel(conn, response.data);
        });
      }

      pplx::task<Channel> remove(ConnectionState* conn, Snowflake channel_id)
      {
        return conn->request(Chan_CID, channel_id, Method::DEL, "channels/" + channel_id.to_string())
        .then([conn](APIResponse response)
        {
          return Channel(conn, response.data);
        });
      }

      pplx::task<std::vector<Message>> get_messages(ConnectionState* conn, Snowflake channel_id, int32_t limit, SearchMethod method, Snowflake pivot)
      {
        if (method != SearchMethod::None && pivot.id() == 0)
        {
          throw DiscordException("Search method passed to get_messages, but pivot id is zero.");
        }
        std::string params = "limit=" + std::to_string(limit);

        switch (method)
        {
        case SearchMethod::None:
          break;
        case SearchMethod::Before:
          params += "&before=" + pivot.to_string();
          break;
        case SearchMethod::After:
          params += "&after=" + pivot.to_string();
          break;
        case SearchMethod::Around:
          params += "&around=" + pivot.to_string();
          break;
        default:
          throw DiscordException("Invalid search method passed to get_messages.");
        }

        return conn->request(Chan_CID_Messages, channel_id, Method::GET, "channels/" + channel_id.to_string() + "/messages", std::move(params))
        .then([conn](APIResponse response)
        {
          std::vector<Message> messages;
          for (auto& message_data : response.data.GetArray())
          {
            messages.emplace_back(conn, message_data);
          }

          return messages;
        });
      }

      pplx::task<Message> get_message(ConnectionState* conn, Snowflake channel_id, Snowflake message_id)
      {
        return conn->request(Chan_CID_Messages_MID, channel_id, Method::GET, "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string())
        .then([conn](APIResponse response)
        {
          return Message(conn, response.data);
        });
      }

      pplx::task<Message> create_message(ConnectionState* conn, Snowflake channel_id, std::string content, bool tts, Embed Embed)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("content");
        writer.String(content);

        writer.String("tts");
        writer.Bool(tts);

        if (!Embed.empty())
        {
          writer.String("embed");
          Embed.Serialize(writer);
        }

        writer.EndObject();

        return conn->request(Chan_CID_Messages, channel_id, Method::POST, "channels/" + channel_id.to_string() + "/messages", sb.GetString())
        .then([conn](APIResponse response)
        {
          return Message(conn, response.data);
        });
      }

      pplx::task<bool> create_reaction(ConnectionState* conn, Snowflake channel_id, Snowflake message_id, std::string emoji)
      {
        return conn->request(Chan_CID_Messages_MID_Reactions_Emoji_Me, channel_id, Method::PUT, 
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji + "/@me")
        .then([conn](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_own_reaction(ConnectionState* conn, Snowflake channel_id, Snowflake message_id, Emoji emoji)
      {
        return conn->request(Chan_CID_Messages_MID_Reactions_Emoji_Me, channel_id, Method::DEL,
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji.name() + "/@me")
        .then([conn](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_user_reaction(ConnectionState* conn, Snowflake channel_id, Snowflake message_id, Emoji emoji, Snowflake user_id)
      {
        return conn->request(Chan_CID_Messages_MID_Reactions_Emoji_UID, channel_id, Method::DEL,
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji.name() + "/" + user_id.to_string())
        .then([conn](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<std::vector<User>> get_reactions(ConnectionState* conn, Snowflake channel_id, Snowflake message_id, Emoji emoji)
      {
        return conn->request(Chan_CID_Messages_MID_Reactions_Emoji, channel_id, Method::GET,
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji.name())
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

      pplx::task<void> remove_all_reactions(ConnectionState* conn, Snowflake channel_id, Snowflake message_id)
      {
        return conn->request(Chan_CID_Messages_MID_Reactions, channel_id, Method::DEL,
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions")
        .then([](APIResponse response)
        {
          return;
        });
      }

      pplx::task<Message> edit_message(ConnectionState* conn, Snowflake channel_id, Snowflake message_id, std::string new_content)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();
        writer.String("content");
        writer.String(new_content);
        writer.EndObject();

        return conn->request(Chan_CID_Messages_MID, channel_id, Method::PATCH, 
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string(), sb.GetString())
        .then([conn](APIResponse response)
        {
          return Message(conn, response.data);
        });
      }

      pplx::task<bool> remove_message(ConnectionState* conn, Snowflake channel_id, Snowflake message_id)
      {
        return conn->request(Chan_CID_Messages_MID, channel_id, Method::DEL, 
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string())
        .then([conn](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> bulk_remove_messages(ConnectionState* conn, Snowflake channel_id, std::vector<Snowflake> message_ids)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();
        writer.String("messages");

        writer.StartArray();
        for (const auto& id : message_ids)
        {
          writer.String(id.to_string());
        }
        writer.EndArray();

        writer.EndObject();

        return conn->request(Chan_CID_Messages_BulkDelete, channel_id, Method::POST,
          "channels/" + channel_id.to_string() + "/messages/bulk-delete", sb.GetString())
        .then([conn](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> edit_permissions(ConnectionState* conn, Snowflake channel_id, Overwrite overwrite, uint32_t allow, uint32_t deny, std::string type)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("allow");
        writer.Uint(allow);
        writer.String("deny");
        writer.Uint(deny);
        writer.String("type");
        writer.String(type);

        writer.EndObject();

        return conn->request(Chan_CID_Perms_OID, channel_id, Method::PUT,
          "channels/" + channel_id.to_string() + "/permissions/" + overwrite.id().to_string(), sb.GetString())
        .then([conn](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_permission(ConnectionState* conn, Snowflake channel_id, Overwrite overwrite)
      {
        return conn->request(Chan_CID_Perms_OID, channel_id, Method::DEL,
          "channels/" + channel_id.to_string() + "/permissions/" + overwrite.id().to_string())
        .then([conn](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> trigger_typing_indicator(ConnectionState* conn, Snowflake channel_id)
      {
        return conn->request(Chan_CID_Typing, channel_id, Method::POST,
          "channels/" + channel_id.to_string() + "/typing")
        .then([conn](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<std::vector<Message>> get_pinned_messages(ConnectionState* conn, Snowflake channel_id)
      {
        return conn->request(Chan_CID_Pins, channel_id, Method::GET, "channels/" + channel_id.to_string() + "/pins")
        .then([conn](APIResponse response)
        {
          std::vector<Message> messages;

          for (auto& message_data : response.data.GetArray())
          {
            messages.emplace_back(conn, message_data);
          }

          return messages;
        });
      }

      pplx::task<bool> add_pinned_message(ConnectionState* conn, Snowflake channel_id, Snowflake message_id)
      {
        return conn->request(Chan_CID_Pins_MID, channel_id, Method::POST,
          "channels/" + channel_id.to_string() + "/pins/" + message_id.to_string())
        .then([conn](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_pinned_message(ConnectionState* conn, Snowflake channel_id, Snowflake message_id)
      {
        return conn->request(Chan_CID_Pins_MID, channel_id, Method::DEL,
          "channels/" + channel_id.to_string() + "/pins/" + message_id.to_string())
        .then([conn](APIResponse response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<void> group_dm_add_recipient(ConnectionState* conn, Snowflake channel_id, Snowflake user_id, std::string access_token, std::string nickname)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("access_token");
        writer.String(access_token);
        writer.String("nick");
        writer.String(nickname);

        writer.EndObject();

        return conn->request(Chan_CID_Recip_UID, channel_id, Method::PUT,
          "channels/" + channel_id.to_string() + "/recipients/" + user_id.to_string(), sb.GetString())
        .then([](APIResponse response)
        {
          return;
        });;
      }

      pplx::task<void> group_dm_remove_recipient(ConnectionState* conn, Snowflake channel_id, Snowflake user_id)
      {
        return conn->request(Chan_CID_Recip_UID, channel_id, Method::DEL,
          "channels/" + channel_id.to_string() + "/recipients/" + user_id.to_string())
        .then([](APIResponse response)
        {
          return;
        });;
      }
    }
  }
}

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
      pplx::task<discord::channel> modify_text_channel(connection_state* conn, snowflake channel_id, std::string name, int32_t position, std::string topic)
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

        return conn->request(Chan_CID, channel_id, method::PATCH, "channels/" + channel_id.to_string(), sb.GetString())
        .then([conn](api_response response)
        {
          return discord::channel(conn, response.data);
        });
      }

      pplx::task<discord::channel> modify_voice_channel(connection_state* conn, snowflake channel_id, std::string name, int32_t position, uint32_t bitrate, uint32_t user_limit)
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

        return conn->request(Chan_CID, channel_id, method::PATCH, "channels/" + channel_id.to_string(), sb.GetString())
        .then([conn](api_response response)
        {
          return discord::channel(conn, response.data);
        });
      }

      pplx::task<discord::channel> remove(connection_state* conn, snowflake channel_id)
      {
        return conn->request(Chan_CID, channel_id, method::DEL, "channels/" + channel_id.to_string())
        .then([conn](api_response response)
        {
          return discord::channel(conn, response.data);
        });
      }

      pplx::task<std::vector<message>> get_messages(connection_state* conn, snowflake channel_id, int32_t limit, search_method method, snowflake pivot)
      {
        if (method != search_method::None && pivot.id() == 0)
        {
          throw discord_exception("Search method passed to get_messages, but pivot id is zero.");
        }

        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("limit");
        writer.Int(limit);

        switch (method)
        {
        case search_method::None:
          break;
        case search_method::Before:
          writer.String("before");
          writer.String(pivot.to_string());
          break;
        case search_method::After:
          writer.String("after");
          writer.String(pivot.to_string());
          break;
        case search_method::Around:
          writer.String("around");
          writer.String(pivot.to_string());
          break;
        default:
          throw discord_exception("Invalid search method passed to get_messages.");
        }

        writer.EndObject();


        return conn->request(Chan_CID_Messages, channel_id, method::GET, "channels/" + channel_id.to_string() + "/messages", sb.GetString())
        .then([conn](api_response response)
        {
          std::vector<message> messages;
          for (auto& message_data : response.data.GetArray())
          {
            messages.push_back(message(conn, message_data));
          }

          return messages;
        });
      }

      pplx::task<message> get_message(connection_state* conn, snowflake channel_id, snowflake message_id)
      {
        return conn->request(Chan_CID_Messages_MID, channel_id, method::GET, "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string())
        .then([conn](api_response response)
        {
          return message(conn, response.data);
        });
      }

      pplx::task<message> create_message(connection_state* conn, snowflake channel_id, std::string content, bool tts, discord::embed embed)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("content");
        writer.String(content);

        writer.String("tts");
        writer.Bool(tts);

        if (!embed.empty())
        {
          writer.String("embed");
          embed.Serialize(writer);
        }

        writer.EndObject();

        return conn->request(Chan_CID_Messages, channel_id, method::POST, "channels/" + channel_id.to_string() + "/messages", sb.GetString())
        .then([conn](api_response response)
        {
          return message(conn, response.data);
        });
      }

      pplx::task<bool> create_reaction(connection_state* conn, snowflake channel_id, snowflake message_id, std::string emoji)
      {
        return conn->request(Chan_CID_Messages_MID_Reactions_Emoji_Me, channel_id, method::PUT, 
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji + "/@me")
        .then([conn](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_own_reaction(connection_state* conn, snowflake channel_id, snowflake message_id, emoji emoji)
      {
        return conn->request(Chan_CID_Messages_MID_Reactions_Emoji_Me, channel_id, method::DEL,
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji.name() + "/@me")
        .then([conn](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_user_reaction(connection_state* conn, snowflake channel_id, snowflake message_id, emoji emoji, snowflake user_id)
      {
        return conn->request(Chan_CID_Messages_MID_Reactions_Emoji_UID, channel_id, method::DEL,
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji.name() + "/" + user_id.to_string())
        .then([conn](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<std::vector<discord::user>> get_reactions(connection_state* conn, snowflake channel_id, snowflake message_id, emoji emoji)
      {
        return conn->request(Chan_CID_Messages_MID_Reactions_Emoji, channel_id, method::GET,
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji.name())
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

      pplx::task<void> remove_all_reactions(connection_state* conn, snowflake channel_id, snowflake message_id)
      {
        return conn->request(Chan_CID_Messages_MID_Reactions, channel_id, method::DEL,
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions")
        .then([](api_response response)
        {
          return;
        });
      }

      pplx::task<message> edit_message(connection_state* conn, snowflake channel_id, snowflake message_id, std::string new_content)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();
        writer.String("content");
        writer.String(new_content);
        writer.EndObject();

        return conn->request(Chan_CID_Messages_MID, channel_id, method::PATCH, 
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string(), sb.GetString())
        .then([conn](api_response response)
        {
          return message(conn, response.data);
        });
      }

      pplx::task<bool> remove_message(connection_state* conn, snowflake channel_id, snowflake message_id)
      {
        return conn->request(Chan_CID_Messages_MID, channel_id, method::DEL, 
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string())
        .then([conn](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> bulk_remove_messages(connection_state* conn, snowflake channel_id, std::vector<snowflake> message_ids)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();
        writer.String("messages");

        writer.StartArray();
        for (auto& id : message_ids)
        {
          writer.String(id.to_string());
        }
        writer.EndArray();

        writer.EndObject();

        return conn->request(Chan_CID_Messages_BulkDelete, channel_id, method::POST,
          "channels/" + channel_id.to_string() + "/messages/bulk-delete", sb.GetString())
        .then([conn](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> edit_permissions(connection_state* conn, snowflake channel_id, overwrite overwrite, uint32_t allow, uint32_t deny, std::string type)
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

        return conn->request(Chan_CID_Perms_OID, channel_id, method::PUT,
          "channels/" + channel_id.to_string() + "/permissions/" + overwrite.id().to_string(), sb.GetString())
        .then([conn](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_permission(connection_state* conn, snowflake channel_id, overwrite overwrite)
      {
        return conn->request(Chan_CID_Perms_OID, channel_id, method::DEL,
          "channels/" + channel_id.to_string() + "/permissions/" + overwrite.id().to_string())
        .then([conn](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> trigger_typing_indicator(connection_state* conn, snowflake channel_id)
      {
        return conn->request(Chan_CID_Typing, channel_id, method::POST,
          "channels/" + channel_id.to_string() + "/typing")
        .then([conn](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<std::vector<message>> get_pinned_messages(connection_state* conn, snowflake channel_id)
      {
        return conn->request(Chan_CID_Pins, channel_id, method::GET, "channels/" + channel_id.to_string() + "/pins")
        .then([conn](api_response response)
        {
          std::vector<message> messages;

          for (auto& message_data : response.data.GetArray())
          {
            messages.emplace_back(conn, message_data);
          }

          return messages;
        });
      }

      pplx::task<bool> add_pinned_message(connection_state* conn, snowflake channel_id, snowflake message_id)
      {
        return conn->request(Chan_CID_Pins_MID, channel_id, method::POST,
          "channels/" + channel_id.to_string() + "/pins/" + message_id.to_string())
        .then([conn](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<bool> remove_pinned_message(connection_state* conn, snowflake channel_id, snowflake message_id)
      {
        return conn->request(Chan_CID_Pins_MID, channel_id, method::DEL,
          "channels/" + channel_id.to_string() + "/pins/" + message_id.to_string())
        .then([conn](api_response response)
        {
          return response.status_code == 204;
        });
      }

      pplx::task<void> group_dm_add_recipient(connection_state* conn, snowflake channel_id, snowflake user_id, std::string access_token, std::string nickname)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("access_token");
        writer.String(access_token);
        writer.String("nick");
        writer.String(nickname);

        writer.EndObject();

        return conn->request(Chan_CID_Recip_UID, channel_id, method::PUT,
          "channels/" + channel_id.to_string() + "/recipients/" + user_id.to_string(), sb.GetString())
        .then([](api_response response)
        {
          return;
        });;
      }

      pplx::task<void> group_dm_remove_recipient(connection_state* conn, snowflake channel_id, snowflake user_id)
      {
        return conn->request(Chan_CID_Recip_UID, channel_id, method::DEL,
          "channels/" + channel_id.to_string() + "/recipients/" + user_id.to_string())
        .then([](api_response response)
        {
          return;
        });;
      }
    }
  }
}

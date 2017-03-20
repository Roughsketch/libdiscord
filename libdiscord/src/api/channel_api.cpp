#include <cpprest/http_client.h>
#include "api/channel_api.h"
#include "connection_state.h"
#include "emoji.h"

namespace discord
{
  namespace api
  {
    namespace channel
    {
      discord::channel modify_text_channel(connection_state* conn, snowflake channel_id, std::string name, int32_t position, std::string topic)
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

        auto response = conn->request("channels/" + channel_id.to_string(), channel_id, method::PATCH, sb.GetString()).get();
        
        return discord::channel(conn, response.data);
      }

      discord::channel modify_voice_channel(connection_state* conn, snowflake channel_id, std::string name, int32_t position, uint32_t bitrate, uint32_t user_limit)
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

        auto response = conn->request("channels/" + channel_id.to_string(), channel_id, method::PATCH, sb.GetString()).get();

        return discord::channel(conn, response.data);
      }

      discord::channel remove(connection_state* conn, snowflake channel_id)
      {
        auto response = conn->request("channels/" + channel_id.to_string(), channel_id, method::DEL).get();
        return discord::channel(conn, response.data);
      }

      std::vector<message> get_messages(connection_state* conn, snowflake channel_id, int32_t limit, search_method method, snowflake pivot)
      {
        if (method != search_method::None && pivot.id() == 0)
        {
          LOG(ERROR) << "Search method passed to get_messages, but pivot id is zero.";
          return std::vector<message>();
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
          LOG(ERROR) << "Invalid search method passed to get_messages.";
          return std::vector<message>();
        }

        writer.EndObject();


        auto response = conn->request("channels/" + channel_id.to_string() + "/messages", channel_id, method::GET, sb.GetString()).get();

        std::vector<message> messages;
        for (auto& message_data : response.data.GetArray())
        {
          messages.push_back(message(conn, message_data));
        }

        return messages;
      }

      message get_message(connection_state* conn, snowflake channel_id, snowflake message_id)
      {
        auto response = conn->request("channels/" + channel_id.to_string() + "/messages/" + message_id.to_string(), channel_id, method::GET).get();

        return message(conn, response.data);
      }

      message create_message(connection_state* conn, snowflake channel_id, std::string content, bool tts, discord::embed embed)
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

        auto response = conn->request("channels/" + channel_id.to_string() + "/messages", channel_id, method::POST, sb.GetString()).get();
        return message(conn, response.data);
      }

      bool create_reaction(connection_state* conn, snowflake channel_id, snowflake message_id, emoji emoji)
      {
        auto response = conn->request(
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji.name(),
          channel_id, method::PUT).get();

        return response.status_code == 204;
      }

      bool remove_own_reaction(connection_state* conn, snowflake channel_id, snowflake message_id, emoji emoji)
      {
        auto response = conn->request(
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji.name() + "/@me",
          channel_id, method::DEL).get();

        return response.status_code == 204;
      }

      bool remove_user_reaction(connection_state* conn, snowflake channel_id, snowflake message_id, emoji emoji, snowflake user_id)
      {
        auto response = conn->request(
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji.name() + "/" + user_id.to_string(),
          channel_id, method::DEL).get();

        return response.status_code == 204;
      }

      std::vector<user> get_reactions(connection_state* conn, snowflake channel_id, snowflake message_id, emoji emoji)
      {
        auto response = conn->request(
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions/" + emoji.name(),
          channel_id, method::GET).get();

        std::vector<user> users;

        for (auto& user_data : response.data.GetArray())
        {
          users.emplace_back(user(conn, user_data));
        }

        return users;
      }

      void remove_all_reactions(connection_state* conn, snowflake channel_id, snowflake message_id)
      {
        conn->request(
          "channels/" + channel_id.to_string() + "/messages/" + message_id.to_string() + "/reactions",
          channel_id, method::DEL);
      }

      message edit_message(connection_state* conn, snowflake channel_id, snowflake message_id, std::string new_content)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();
        writer.String("content");
        writer.String(new_content);
        writer.EndObject();

        auto response = conn->request("channels/" + channel_id.to_string() + "/messages/" + message_id.to_string(),
          channel_id, method::PATCH, sb.GetString()).get();

        return message(conn, response.data);
      }

      bool remove_message(connection_state* conn, snowflake channel_id, snowflake message_id)
      {
        auto response = conn->request("channels/" + channel_id.to_string() + "/messages/" + message_id.to_string(),
          channel_id, method::DEL).get();
        return response.status_code == 204;
      }

      bool bulk_remove_messages(connection_state* conn, snowflake channel_id, std::vector<snowflake> message_ids)
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

        auto response = conn->request("channels/" + channel_id.to_string() + "/messages/bulk-delete",
          channel_id, method::POST, sb.GetString()).get();

        return response.status_code == 204;
      }

      bool edit_permissions(connection_state* conn, snowflake channel_id, overwrite overwrite, uint32_t allow, uint32_t deny, std::string type)
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

        auto response = conn->request("channels/" + channel_id.to_string() + "/permissions/" + overwrite.id().to_string(),
          channel_id, method::PUT, sb.GetString()).get();
        return response.status_code == 204;
      }

      bool remove_permission(connection_state* conn, snowflake channel_id, overwrite overwrite)
      {
        auto response = conn->request("channels/" + channel_id.to_string() + "/permissions/" + overwrite.id().to_string(),
          channel_id, method::DEL).get();

        return response.status_code == 204;
      }

      bool trigger_typing_indicator(connection_state* conn, snowflake channel_id)
      {
        auto response = conn->request("channels/" + channel_id.to_string() + "/typing", channel_id, method::POST).get();

        return response.status_code == 204;
      }

      std::vector<message> get_pinned_messages(connection_state* conn, snowflake channel_id)
      {
        auto response = conn->request("channels/" + channel_id.to_string() + "/pins",
          channel_id, method::GET).get();

        std::vector<message> messages;

        for (auto& message_data : response.data.GetArray())
        {
          messages.emplace_back(message(conn, message_data));
        }

        return messages;
      }

      bool add_pinned_message(connection_state* conn, snowflake channel_id, snowflake message_id)
      {
        auto response = conn->request("channels/" + channel_id.to_string() + "/pins/" + message_id.to_string(),
          channel_id, method::POST).get();

        return response.status_code == 204;
      }

      bool remove_pinned_message(connection_state* conn, snowflake channel_id, snowflake message_id)
      {
        auto response = conn->request("channels/" + channel_id.to_string() + "/pins/" + message_id.to_string(),
          channel_id, method::DEL).get();

        return response.status_code == 204;
      }

      void group_dm_add_recipient(connection_state* conn, snowflake channel_id, snowflake user_id, std::string access_token, std::string nickname)
      {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

        writer.StartObject();

        writer.String("access_token");
        writer.String(access_token);
        writer.String("nick");
        writer.String(nickname);

        writer.EndObject();

        conn->request("channels/" + channel_id.to_string() + "/recipients/" + user_id.to_string(),
          channel_id, method::PUT, sb.GetString());
      }

      void group_dm_remove_recipient(connection_state* conn, snowflake channel_id, snowflake user_id)
      {
        conn->request("channels/" + channel_id.to_string() + "/recipients/" + user_id.to_string(),
          channel_id, method::DEL);
      }
    }
  }
}
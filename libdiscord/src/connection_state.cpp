#include <cpprest/http_client.h>

#include "connection_state.h"
#include "discord_exception.h"

#include "channel.h"
#include "guild.h"
#include "member.h"
#include "role.h"
#include "user.h"
#include <iomanip>

namespace discord
{
  void connection_state::raise_event(event_type type, rapidjson::Value& data) const
  {
    if (m_event_handler)
    {
      m_event_handler(type, data);
    }
  }

  connection_state::connection_state() : m_shards(0)
  {
    m_client = new web::http::client::http_client(U("https://discordapp.com/api/v6"));
  }

  connection_state::connection_state(std::string token, int shards) : connection_state()
  {
    m_token = token;
    m_shards = shards;
  }

  connection_state::~connection_state()
  {
    delete m_client;
  }

  void connection_state::connect()
  {
    int recommended_shards;
    utility::string_t wss_url;

    web::uri_builder builder(U(""));
    builder.append_query(U("v"), gateway::VERSION);
    builder.append_query(U("encoding"), gateway::ENCODING);

    if (wss_url.empty())
    {
      do
      {
        try
        {
          //  Attempt to get the Gateway URL.
          wss_url = utility::conversions::to_string_t(api::get_wss_url(*this, &recommended_shards));
        }
        catch (const web::http::http_exception& e)
        {
          LOG(ERROR) << "Exception getting gateway URL: " << e.what();
          LOG(ERROR) << "Sleeping for 5 seconds and trying again.";
          std::this_thread::sleep_for(std::chrono::seconds(5));
        }
      } while (wss_url.empty());  //  Keep trying until we get it.

      wss_url += builder.to_string();
    }

    //  For each shard, create a gateway.
    for (auto shard = 0; shard < m_shards; ++shard)
    {
      m_gateways.push_back(std::make_unique<gateway>(wss_url, m_token, shard, m_shards));

      //  Bind this object's on_dispatch method to the gateway callback.
      m_gateways.back()->on_dispatch(std::bind(&connection_state::on_dispatch, this, std::placeholders::_1, std::placeholders::_2));
    }

    //  Start all the gateways.
    for (auto& gateway : m_gateways)
    {
      gateway->start();
    }
  }

  pplx::task<api_response> connection_state::request(api_key key, snowflake major, method type, std::string endpoint, const std::string&& data)
  {
    LOG(DEBUG) << "Request: " << endpoint << " - " << major.to_string() << " - " << data;
    auto map_key = std::hash<std::string>()(std::to_string(key) + major.to_string());
    auto mutex_it = m_api_mutex.find(map_key);

    //  If the cached mutex does not exist, create it.
    if (mutex_it == std::end(m_api_mutex))
    {
      m_api_mutex[map_key] = std::make_unique<std::mutex>();
    }

    auto mutex = m_api_mutex[map_key].get();
    
    //  Try to lock this mutex until it works.
    while (!mutex->try_lock())
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    if (m_global_mutex.try_lock())
    {
      m_global_mutex.unlock();
    }
    else
    {
      //  If we can't lock the global mutex, then we might be rate limited.
      //  Wait for the global mutex to become available again before continuing.
      LOG(DEBUG) << "Could not lock global mutex. Waiting for it to unlock.";
      std::lock_guard<std::mutex> global_lock(m_global_mutex);
      LOG(DEBUG) << "Global mutex unlocked.";
    }

    web::http::method method;

    switch (type)
    {
    case method::GET:
      method = web::http::methods::GET;
      break;
    case method::POST:
      method = web::http::methods::POST;
      break;
    case method::PUT:
      method = web::http::methods::PUT;
      break;
    case method::PATCH:
      method = web::http::methods::PATCH;
      break;
    case method::DEL:
      method = web::http::methods::DEL;
      break;
    }

    web::http::http_request request(method);
    request.set_request_uri(utility::conversions::to_string_t(endpoint));
    request.headers().add(U("Authorization"), utility::conversions::to_string_t(m_token));
    request.headers().add(U("Content-Type"), U("application/json"));

    if (!data.empty())
    {
      //  If there's data and the method is GET, then add data as query parameters.
      if (method == web::http::methods::GET)
      {
        LOG(DEBUG) << "Setting query parameters: " << (endpoint + data);
        request.set_request_uri(utility::conversions::to_string_t(endpoint + data));
      }
      else
      {
        LOG(DEBUG) << "Setting request data.";
        request.set_body(data);
      }
    }

    return m_client->request(request).then([=](web::http::http_response res) -> api_response
    {
      api_response response;
      auto headers = res.headers();

      //  Various rate-limit related headers
      auto remaining = headers.find(U("X-RateLimit-Remaining"));
      auto reset = headers.find(U("X-RateLimit-Reset"));
      auto retry = headers.find(U("Retry-After"));
      auto global = headers.find(U("X-RateLimit-Global"));
      auto date_str = headers.find(U("Date"));
      auto server_date = std::chrono::system_clock::now();

      if (date_str != std::end(headers))
      {
        std::tm tm = {};
        std::stringstream ss(utility::conversions::to_utf8string(date_str->second));
        ss >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
        server_date = std::chrono::system_clock::from_time_t(std::mktime(&tm));
      }

      if (remaining != std::end(headers))
      {
        auto rate_remaining = std::stoul(utility::conversions::to_utf8string(remaining->second));

        if (!rate_remaining)
        {
          auto rate_reset = std::stoul(utility::conversions::to_utf8string(reset->second));

          //  Get the time that the rate limit will reset.
          auto end_time = std::chrono::system_clock::time_point(std::chrono::seconds(rate_reset));

          //  Get the total amount of time to wait from this point.
          auto total_time = std::chrono::duration_cast<std::chrono::seconds>(end_time - std::chrono::system_clock::now()).count();

          LOG(WARNING) << "We hit the rate limit for endpoint " << endpoint << ". Sleeping for " << total_time << " seconds.";
          std::this_thread::sleep_until(end_time);
        }
      }

      if (retry != std::end(headers))
      {
        auto retry_after = std::stoul(utility::conversions::to_utf8string(retry->second));

        if (global == std::end(headers))
        {
          LOG(WARNING) << "Received a Retry-After header. Waiting for " << retry_after << "ms.";
          std::this_thread::sleep_for(std::chrono::milliseconds(retry_after));
          mutex->unlock();
          this->request(key, major, type, endpoint, std::move(data));
        }
        else
        {
          LOG(ERROR) << "Hit the global rate limit. Waiting for " << retry_after << "ms.";

          //  This is a global rate limit, lock the global mutex and wait.
          std::lock_guard<std::mutex> global_lock(m_global_mutex);
          std::this_thread::sleep_for(std::chrono::milliseconds(retry_after));
        }
      }

      mutex->unlock(); // Unlock mutex since we're past the rate-limiting section.
      response.status_code = res.status_code();

      if (res.status_code() == web::http::status_codes::OK)
      {
        auto bodyStream = res.body();
        Concurrency::streams::container_buffer<std::string> inStringBuffer;

        bodyStream.read_to_end(inStringBuffer).then([inStringBuffer](size_t bytesRead)
        {
          return inStringBuffer.collection();
        }).then([&response](std::string text)
        {
          LOG(DEBUG) << "Got API response: " << text;
          response.data.Parse(text.c_str(), text.size());
        }).get();
      }
      else if (res.status_code() != web::http::status_codes::NoContent)
      {
        auto json_str = utility::conversions::to_utf8string(res.extract_string().get());
        response.data.Parse(json_str.c_str(), json_str.size());

        auto code_member = response.data.FindMember("code");

        if (code_member != response.data.MemberEnd())
        {
          //  Try to find a name member which holds error data.
          auto found = response.data.FindMember("name");

          //  If the name member isn't found, try to find the content member instead.
          if (found == response.data.MemberEnd())
          {
            found = response.data.FindMember("content");
          }

          //  If we found either name or content, then concatenate their messages and throw an exception.
          if (found != response.data.MemberEnd())
          {
            std::string messages;
            for (auto& content : found->value.GetArray())
            {
              messages += std::string(content.GetString()) + "\n";
            }

            if (messages.size() > 0)
            {
              throw discord_exception(messages);
            }

            throw discord_exception("API call failed and response was null.");
          }

          //  Didn't find name or content member, throw based off error code instead.
          auto code = code_member->value.GetInt();
          std::string message = response.data["message"].GetString();

          if (code < 20000)
          {
            throw unknown_exception(message);
          }

          if (code < 30000)
          {
            throw too_many_exception(message);
          }

          switch (code)
          {
          case EmbedDisabled:
            throw embed_exception(message);
          case MissingPermissions:
          case ChannelVerificationTooHigh:
            throw permission_exception(message);
          case Unauthorized:
          case MissingAccess:
          case InvalidAuthToken:
            throw authorization_exception(message);
          default:
            //  No specially handled codes left, throw a default exception
            throw discord_exception(message);
          }
        }
      }

      return response;
    });
  }

  void connection_state::on_dispatch(std::string event_name, rapidjson::Value& data)
  {
    //LOG(INFO) << "Bot.handle_dispatch entered with " << event_name.c_str() << ".";

    if (event_name == "READY")
    {
      m_profile = std::make_unique<user>(this, data["user"]);

      for (auto& channel_data : data["private_channels"].GetArray())
      {
        snowflake id(channel_data["id"].GetString());
        channel chan(this, channel_data);
        m_private_channels[id] = chan;
      }

      //  Pass dummy to avoid compatibility problems with default values for references
      rapidjson::Value dummy;
      raise_event(Ready, dummy);
    }
    else if (event_name == "CHANNEL_CREATE")
    {
      channel chan(this, data);
      auto found = data.FindMember("guild_id");
      if (found != data.MemberEnd() && !found->value.IsNull())
      {
        //  This is a Guild Channel object, add it to its respective guild.
        auto guild_id = snowflake(data["guild_id"].GetString());

        m_channel_guilds[chan.id()] = guild_id;

        auto owner = m_guilds.find(guild_id);

        if (owner == std::end(m_guilds))
        {
          LOG(ERROR) << "Tried to add a channel from a non-existent guild.";
        }
        else
        {
          owner->second.add_channel(chan);
        }
      }
      else
      {
        //  This is a DM channel object. Add it to the bot's private channels.
        m_private_channels[chan.id()] = chan;
      }
    }
    else if (event_name == "CHANNEL_UPDATE")
    {
      channel chan(this, data);
      auto found = data.FindMember("guild_id");
      if (found != data.MemberEnd() && !found->value.IsNull())
      {
        //  This is a Guild Channel object, update it inside its respective guild.
        auto guild_id = snowflake(data["guild_id"].GetString());

        m_channel_guilds[chan.id()] = guild_id;

        auto owner = m_guilds.find(guild_id);

        if (owner == std::end(m_guilds))
        {
          LOG(ERROR) << "Tried to add a channel from a non-existent guild.";
        }
        else
        {
          owner->second.update_channel(chan);
        }
      }
      else
      {
        //  This is a DM channel object. Update it in the private channels list.
        m_private_channels[chan.id()] = chan;
      }
    }
    else if (event_name == "CHANNEL_DELETE")
    {
      channel chan(this, data);
      auto found = data.FindMember("guild_id");
      if (found != data.MemberEnd() && !found->value.IsNull())
      {
        //  This is a Guild Channel object, remove it from its respective guild.
        auto guild_id = snowflake(data["guild_id"].GetString());

        m_channel_guilds.erase(chan.id());

        auto owner = m_guilds.find(guild_id);

        if (owner == std::end(m_guilds))
        {
          LOG(ERROR) << "Tried to remove a channel from a non-existent guild.";
        }
        else
        {
          owner->second.remove_channel(chan);
        }
      }
      else
      {
        //  This is a DM channel object. Remove it from the private channels list.
        m_private_channels.erase(chan.id());
      }
    }
    else if (event_name == "GUILD_CREATE")
    {
      guild new_guild(this, data);
      m_guilds[new_guild.id()] = new_guild;
    }
    else if (event_name == "GUILD_UPDATE")
    {
      guild updated(this, data);
      m_guilds[updated.id()] = updated;
    }
    else if (event_name == "GUILD_DELETE")
    {
      snowflake id(data["id"].GetString());

      if (data.FindMember("unavailable") != data.MemberEnd())
      {
        m_guilds[id].set_unavailable(true);
      }
      else
      {
        auto guild = m_guilds.at(id);
        auto channel_ids = guild.channel_ids();

        for (auto& chan_id : channel_ids)
        {
          m_channel_guilds.erase(chan_id);
        }

        m_guilds.erase(id);
      }
    }
    else if (event_name == "GUILD_BAN_ADD")
    {
      user banned(this, data["user"]);
      snowflake guild_id(data["guild_id"].GetString());

      LOG(DEBUG) << "User " << banned.distinct()
        << " has been banned from "
        << m_guilds[guild_id].name() << ".";
    }
    else if (event_name == "GUILD_BAN_REMOVE")
    {
      user unbanned(this, data["user"]);
      snowflake guild_id(data["guild_id"].GetString());
      LOG(DEBUG) << "User " << unbanned.distinct()
        << " has been unbanned from "
        << m_guilds[guild_id].name();
    }
    else if (event_name == "GUILD_EMOJIS_UPDATE")
    {
      //  Update emoji data for the guild
      snowflake guild_id(data["guild_id"].GetString());
      auto owner = m_guilds[guild_id];

      std::vector<emoji> new_emojis;
      std::vector<emoji> deleted_emojis;
      auto old_emojis = owner.emojis();

      for (auto& emoji_data : data["emojis"].GetArray())
      {
        emoji emo(emoji_data);
        emoji found;
        new_emojis.push_back(emo);

        if (owner.find_emoji(emo.id(), found))
        {
          //  If emoji was already there, check if it has been updated.
          if (emo.name() != found.name() || emo.roles() != found.roles())
          {
            raise_event(EmojiEdited, emoji_data);
          }
        }
        else
        {
          //  Emoji was not found in guild, so raise a new emoji event.
          raise_event(EmojiCreated, emoji_data);
        }
      }

      owner.set_emojis(new_emojis);
    }
    else if (event_name == "GUILD_INTEGRATIONS_UPDATE")
    {
      LOG(DEBUG) << "Got a Guild Integrations Update, but left it unhandled.";
    }
    else if (event_name == "GUILD_MEMBER_ADD")
    {
      snowflake guild_id(data["guild_id"].GetString());
      member guild_member(this, data);
      m_guilds[guild_id].add_member(guild_member);
    }
    else if (event_name == "GUILD_MEMBER_REMOVE")
    {
      snowflake guild_id(data["guild_id"].GetString());
      member guild_member(this, data);
      m_guilds[guild_id].remove_member(guild_member);
    }
    else if (event_name == "GUILD_MEMBER_UPDATE")
    {
      snowflake guild_id(data["guild_id"].GetString());

      std::vector<snowflake> roles;
      std::string nick;
      user updated_user;

      set_from_json(nick, "nick", data);

      auto found = data.FindMember("user");
      if (found != data.MemberEnd() && !found->value.IsNull())
      {
        updated_user = user(this, data["user"]);
      }

      found = data.FindMember("roles");
      if (found != data.MemberEnd() && !found->value.IsNull())
      {
        for (auto& role_id : found->value.GetArray())
        {
          roles.push_back(snowflake(role_id.GetString()));
        }
      }

      m_guilds[guild_id].update_member(roles, updated_user, nick);
    }
    else if (event_name == "GUILD_MEMBERS_CHUNK")
    {
      snowflake guild_id(data["guild_id"].GetString());
      auto owner = m_guilds[guild_id];

      for (auto& member_data : data["members"].GetArray())
      {
        member guild_member(this, member_data);
        owner.add_member(guild_member);
      }
    }
    else if (event_name == "GUILD_ROLE_CREATE")
    {
      snowflake guild_id(data["guild_id"].GetString());
      role guild_role(data["role"]);
      m_guilds[guild_id].add_role(guild_role);
    }
    else if (event_name == "GUILD_ROLE_UPDATE")
    {
      snowflake guild_id(data["guild_id"].GetString());
      role guild_role(data["role"]);
      m_guilds[guild_id].update_role(guild_role);
    }
    else if (event_name == "GUILD_ROLE_DELETE")
    {
      snowflake guild_id(data["guild_id"].GetString());
      snowflake guild_role(data["role_id"].GetString());
      m_guilds[guild_id].remove_role(guild_role);
    }
    else if (event_name == "MESSAGE_CREATE")
    {
      raise_event(MessageCreated, data);
    }
    else if (event_name == "MESSAGE_UPDATE")
    {
      raise_event(MessageEdited, data);
    }
    else if (event_name == "MESSAGE_DELETE")
    {
      raise_event(MessageDeleted, data);
    }
    else if (event_name == "MESSAGE_DELETE_BULK")
    {
      raise_event(MessagesBulkDeleted, data);
    }
    else if (event_name == "PRESENCE_UPDATE")
    {
      presence presence(this, data);
      snowflake guild_id(data["guild_id"].GetString());

      m_guilds[guild_id].update_presence(presence);

      raise_event(Presence, data);
    }
    else if (event_name == "TYPING_START")
    {
      raise_event(Typing, data);
    }
    else if (event_name == "VOICE_STATE_UPDATE")
    {

    }
    else if (event_name == "VOICE_SERVER_UPDATE")
    {

    }
  }

  void connection_state::on_event(std::function<void(event_type, rapidjson::Value& data)> callback)
  {
    m_event_handler = callback;
  }

  const std::string& connection_state::token() const
  {
    return m_token;
  }

  const user& connection_state::profile() const
  {
    return *m_profile.get();
  }

  std::vector<guild> connection_state::guilds() const
  {
    std::vector<guild> guild_vec;

    for (auto& guild_kv : m_guilds)
    {
      guild_vec.push_back(guild_kv.second);
    }

    return guild_vec;
  }

  guild connection_state::find_guild(snowflake id) const
  {
    if (m_guilds.count(id))
    {
      return m_guilds.at(id);
    }

    return guild();
  }

  channel connection_state::find_channel(snowflake id) const
  {
    if (m_channel_guilds.count(id))
    {
      auto guild_id = m_channel_guilds.at(id);
      if (m_guilds.count(guild_id))
      {
        auto guild = m_guilds.at(guild_id);

        return guild.find_channel(id);
      }
    }
    else if (m_private_channels.count(id))
    {
      return m_private_channels.at(id);
    }

    return channel();
  }

  guild connection_state::find_guild_from_channel(snowflake id) const
  {
    if (m_channel_guilds.count(id))
    {
      auto guild_id = m_channel_guilds.at(id);

      if (m_guilds.count(guild_id))
      {
        return m_guilds.at(guild_id);
      }
    }

    return guild();
  }

  void connection_state::cache_channel_id(snowflake guild_id, snowflake channel_id)
  {
    m_channel_guilds[channel_id] = guild_id;
  }
}

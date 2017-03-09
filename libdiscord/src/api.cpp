#include "api.h"
#include "discord_exception.h"

namespace discord
{
  namespace api
  {
    using namespace web::http;
    using namespace web::http::client;
    using namespace Concurrency::streams;

    static std::mutex GlobalMutex;
    static std::map<size_t, std::unique_ptr<std::mutex>> APIMutex;
    static http_client Client(U("https://discordapp.com/api/v6"));

    pplx::task<api_response> request(std::string token, std::string endpoint, snowflake major, method type, const rapidjson::Value& data)
    {
      LOG(DEBUG) << "Request: " << endpoint << " - " << major.to_string() << " - " << json_to_string(data);
      auto map_key = std::hash<std::string>()(token + endpoint + major.to_string());
      auto mutex_it = APIMutex.find(map_key);

      //  If the cached mutex does not exist, create it.
      if (mutex_it == std::end(APIMutex))
      {
        APIMutex[map_key] = std::make_unique<std::mutex>();
      }

      auto mutex = APIMutex[map_key].get();

      std::lock_guard<std::mutex> api_lock(*mutex);

      if (GlobalMutex.try_lock())
      {
        GlobalMutex.unlock();
      }
      else
      {
        //  If we can't lock the global mutex, then we might be rate limited.
        //  Wait for the global mutex to become available again before continuing.
        LOG(DEBUG) << "Could not lock global mutex. Waiting for it to unlock.";
        std::lock_guard<std::mutex> global_lock(GlobalMutex);
        LOG(DEBUG) << "Global mutex unlocked.";
      }

      http_request request(type);
      request.set_request_uri(utility::conversions::to_string_t(endpoint));
      request.headers().add(U("Authorization"), utility::conversions::to_string_t(token));
      request.headers().add(U("Content-Type"), U("application/json"));

      if (!data.IsNull())
      {
        //  If there's data and the method is GET, then add data as query parameters.
        if (type == web::http::methods::GET)
        {
          LOG(DEBUG) << "Setting query parameters.";
          uri_builder builder(utility::conversions::to_string_t(endpoint));

          for (auto& param : data.GetObjectA())
          {
            std::string value;

            if (param.value.IsString())
            {
              value = param.value.GetString();
            }
            else if (param.value.IsInt())
            {
              value = std::to_string(param.value.GetInt());
            }

            builder.append_query(utility::conversions::to_string_t(param.name.GetString()),
              utility::conversions::to_string_t(value));
          }

          request.set_request_uri(builder.to_string());
        }
        else
        {
          auto body = json_to_string(data);
          LOG(DEBUG) << "Setting request data: " << body;
          request.set_body(body);
        }
      }

      return Client.request(request).then([=](http_response res) -> api_response
      {
        api_response response;
        auto headers = res.headers();

        //  Various rate-limit related headers
        auto remaining = headers.find(U("X-RateLimit-Remaining"));
        auto reset = headers.find(U("X-RateLimit-Reset"));
        auto retry = headers.find(U("Retry-After"));
        auto global = headers.find(U("X-RateLimit-Global"));

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
          }
          else
          {
            LOG(ERROR) << "Hit the global rate limit. Waiting for " << retry_after << "ms.";

            //  This is a global rate limit, lock the global mutex and wait.
            std::lock_guard<std::mutex> global_lock(GlobalMutex);
            std::this_thread::sleep_for(std::chrono::milliseconds(retry_after));
          }
        }

        response.status_code = res.status_code();

        LOG(INFO) << "Got Status Code " << response.status_code;

        if (res.status_code() == status_codes::OK)
        {
          LOG(INFO) << "Got into OK body reader.";
          auto bodyStream = res.body();
          container_buffer<std::string> inStringBuffer;

          bodyStream.read_to_end(inStringBuffer).then([inStringBuffer](size_t bytesRead)
          {
            return inStringBuffer.collection();
          }).then([&response](std::string text)
          {
            LOG(INFO) << "Got response: " << text;
            response.data.Parse(text.c_str(), text.size());
          }).get();
        }
        else if (res.status_code() != status_codes::NoContent)
        {
          auto json_str = utility::conversions::to_utf8string(res.extract_string().get());
          response.data.Parse(json_str.c_str(), json_str.size());

          auto code_member = response.data.FindMember("code");

          if (code_member != response.data.MemberEnd())
          {
            //  We got a response code from the request.
            auto name_member = response.data.FindMember("name");
            if (name_member != response.data.MemberEnd())
            {
              for (auto& name : name_member->value.GetArray())
              {
                throw discord_exception(name.GetString());
              }

              throw discord_exception("API call failed and response was null.");
            }

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
              throw discord_exception(message);
            }
          }
        }

        LOG(INFO) << "Returning response.";
        return response;
      });
    }

    std::string get_wss_url()
    {
      auto response = request("", "gateway", 0, methods::GET).get();

      return response.data["url"].GetString();
    }
  }
}
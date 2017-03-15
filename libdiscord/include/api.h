#pragma once

#include <cpprest/http_client.h>
#include <future>

#include "common.h"

namespace discord
{
  namespace api
  {
    /** Struct for holding API responses. Includes status code and JSON data. */
    struct api_response
    {
      rapidjson::Document data;
      web::http::status_code status_code;

      api_response()
      {
        status_code = 0;
      }

      api_response(const api_response& other)
      {
        data.CopyFrom(other.data, data.GetAllocator());
        status_code = other.status_code;
      }

      api_response& operator=(const api_response& other)
      {
        data.CopyFrom(other.data, data.GetAllocator());
        status_code = other.status_code;
        return *this;
      }
    };

    /** Responses that the Discord API can send. */
    enum response_code : uint32_t
    {
      UnknownAccount = 10001,
      UnknownApplication,
      UnknownChannel,
      UnknownGuild,
      UnknownIntegration,
      UnknownInvite,
      UnknownMember,
      UnknownMessage,
      UnknownOverwrite,
      UnknownProvider,
      UnknownRole,
      UnknownToken,
      UnknownUser,
      UnknownEmoji,
      UserOnlyEndpoint = 20001,
      BotOnlyEndpoint,
      MaxGuildsReached = 30001,
      MaxFriendsReached,
      MaxPinsReached,
      MaxGuildRolesReached = 30005,
      TooManyReactions = 30010,
      Unauthorized = 40001,
      MissingAccess = 50001,
      InvalidAccountType,
      CannotExecuteOnDM,
      EmbedDisabled,
      CannotEditAnotherUser,
      CannotSendEmptyMessage,
      CannotSendMessageToUser,
      CannotSendMessagesToVoiceChannel,
      ChannelVerificationTooHigh,
      OAuth2DoesNotHaveBot,
      OAuth2AppLimitReached,
      InvalidOAuth2State,
      MissingPermissions,
      InvalidAuthToken,
      NoteTooLong,
      TooFewOrTooManyMessagesToDelete,
      InvalidChannelForPin = 50019,
      TooOldToDelete = 50034,
      ReactionBlocked = 90001
    };

    /** Make a request to the Discord API. Returns a task that must be synced to get a value. 
     *
     * @param token The token of the bot or user that is calling the API.
     * @param endpoint The endpoint to query.
     * @param major The major parameter of the API call.
     * @param type The method to use when connecting to the API.
     * @param data The JSON payload to attach.
     */
    pplx::task<api_response> request(std::string token, std::string endpoint, snowflake major, web::http::method type, const rapidjson::Value& data = rapidjson::Value());

    /** Get a secure websocket URL from Discord. *
     *
     * @return A WSS URL that can be used to connect to the Discord gateway.
     */
    std::string get_wss_url();
  }
}

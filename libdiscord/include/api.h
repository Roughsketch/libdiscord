#pragma once

#include "common.h"

namespace discord
{
  class connection_state;

  /** Struct for holding API responses. Includes status code and JSON data. */
  struct api_response
  {
    rapidjson::Document data;
    uint16_t status_code;

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

  namespace api
  {
    /** Get a secure websocket URL from Discord. *
      *
      * @return A WSS URL that can be used to connect to the Discord gateway.
      */
    std::string get_wss_url(connection_state& conn, int* shards = nullptr);
  }
}

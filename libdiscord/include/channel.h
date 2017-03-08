#pragma once

#include "common.h"
#include "permission.h"
#include "user.h"

namespace discord
{
  class overwrite : public identifiable
  {
    std::string m_type;
    permission m_allow;
    permission m_deny;
  public:
    overwrite();
    explicit overwrite(rapidjson::Value& data);

    /** Get the type of this overwrite (either "role" or "member")

    @return The type of this overwrite.
    */
    std::string type() const;

    /** Get the permissions that are allowed.

    @return The permissions that are allowed.
    */
    std::shared_ptr<permission> allow() const;

    /** Get the permissions that are denied.

    @return The permissions that are denied.
    */
    std::shared_ptr<permission> deny() const;
  };

  /** Types of channels that can be sent by the API. */
  enum channel_type : uint8_t
  {
    Text = 0,
    Private,
    Voice,
    Group
  };

  class channel : public identifiable
  {
    std::string m_token;

    //  Constants
    const uint32_t MinNameSize = 2;
    const uint32_t MaxNameSize = 100;
    const uint32_t MaxTopicSize = 1024;
    const uint32_t MinBitRate = 8000;
    const uint32_t MaxBitRate = 96000;
    const uint32_t MaxVIPBitRate = 128000;
    const uint32_t MaxUserLimit = 99;

    //  All Channels
    bool m_is_private;
    snowflake m_last_message_id;

    //  Guild Channels
    snowflake m_guild_id;
    std::string m_name;
    channel_type m_type;
    uint32_t m_position;
    std::vector<overwrite> m_permission_overwrites;
    std::string m_topic;
    uint32_t m_bitrate;
    uint32_t m_user_limit;

    //  DM Specific
    user m_recipient;

    bool m_is_dm;
  public:
    channel();
    explicit channel(std::string token, snowflake guild_id, rapidjson::Value& data);

    snowflake guild_id() const;
  };
}
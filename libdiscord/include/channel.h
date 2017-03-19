#pragma once

#include "common.h"
#include "permission.h"
#include "user.h"

namespace discord
{
  enum class search_method
  {
    None,
    Before,
    After,
    Around
  };

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
    permission allow() const;

    /** Get the permissions that are denied.

    @return The permissions that are denied.
    */
    permission deny() const;
  };

  /** Types of channels that can be sent by the API. */
  enum channel_type : uint8_t
  {
    Text = 0,
    Private,
    Voice,
    Group
  };

  class channel : public identifiable, public connection_object
  {
    //  Constants
    static const uint32_t MinNameSize = 2;
    static const uint32_t MaxNameSize = 100;
    static const uint32_t MaxTopicSize = 1024;
    static const uint32_t MinBitRate = 8000;
    static const uint32_t MaxBitRate = 96000;
    static const uint32_t MaxVIPBitRate = 128000;
    static const uint32_t MaxUserLimit = 99;

    //  All Channels
    snowflake m_last_message_id;

    //  Guild Channels
    std::string m_name;
    channel_type m_type;
    int32_t m_position;
    std::vector<overwrite> m_permission_overwrites;
    std::string m_topic;
    uint32_t m_bitrate;
    uint32_t m_user_limit;

    //  DM Specific
    user m_recipient;

    bool m_is_dm;
  public:
    channel();
    explicit channel(connection_state* owner, rapidjson::Value& data);

    /** Gets the name of the channel.
     *
     * @return Name of the channel.
     */
    std::string name() const;

    /** Gets the type of the channel.
     *
     *  One of:
     *    Text
     *    Private
     *    Voice
     *    Group
     *    
     * @return The type of channel this is.
     */
    channel_type type() const;

    /** Gets the position this channel is in the guild list.
     *
     * @return The channel's position.
     */
    int32_t position() const;
    
    /** Gets the bitrate of the current channel.
     *
     * @return The bitrate of this channel, or 0 if not a voice channel.
     */
    uint32_t bitrate() const;

    /** Gets the user limit of the current channel.
     *
     * @return The user limit of this channel, or 0 if not a voice channel.
     */
    uint32_t user_limit() const;

    /** Gets the current set topic for the current channel.
     *
     * @return The channel's topic.
     */
    std::string topic() const;

    /** Gets the guild that owns this channel.
     *
     * @return The guild that owns this channel, or an empty guild if the channel is a DM.
     */
    guild guild() const;
  };
}

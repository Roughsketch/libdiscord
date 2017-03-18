#pragma once

#include "common.h"
#include "user.h"

namespace discord
{
  class message : identifiable, connection_object
  {
    snowflake m_channel_id;
    user m_author;
    std::string m_content;
    std::string m_timestamp;
    std::string m_edited_timestamp;
    bool m_tts;
    bool m_mention_everyone;
    std::vector<user> m_mentions;
    //std::vector<role> m_mention_roles;
    //std::vector<attachment> m_attachments;
    //std::vector<embed> m_embeds;
    //std::vector<reaction> m_reactions;
    snowflake m_nonce;
    bool m_pinned;
    std::string m_webhook_id;
  public:
    message();
    explicit message(connection_state* owner, rapidjson::Value& data);

    std::string content() const;

    const user& author() const;

    message respond(std::string content) const;
  };
}

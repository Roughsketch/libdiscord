#pragma once

#include <cpprest/http_client.h>

#include "common.h"
#include "channel.h"
#include "embed.h"
#include "user.h"

namespace discord
{
  class message : public identifiable, connection_object
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

    std::unique_ptr<channel> channel() const;

    std::unique_ptr<guild> guild() const;

    pplx::task<message> respond(std::string content, bool tts = false, discord::embed embed = discord::embed()) const;

    pplx::task<bool> react(emoji reaction) const;
    pplx::task<bool> react(std::string reaction) const;
  };
}

#pragma once

#include <cpprest/http_client.h>

#include "common.h"
#include "channel.h"
#include "embed.h"
#include "user.h"

namespace discord
{
  class Message : public Identifiable, ConnectionObject
  {
    Snowflake m_channel_id;
    User m_author;
    std::string m_content;
    std::string m_timestamp;
    std::string m_edited_timestamp;
    bool m_tts;
    bool m_mention_everyone;
    std::vector<User> m_mentions;
    //std::vector<role> m_mention_roles;
    //std::vector<attachment> m_attachments;
    //std::vector<Embed> m_embeds;
    //std::vector<reaction> m_reactions;
    Snowflake m_nonce;
    bool m_pinned;
    std::string m_webhook_id;
  public:
	Message();
    explicit Message(ConnectionState* owner, rapidjson::Value& data);

    std::string content() const;

    const User& author() const;

    std::unique_ptr<Channel> channel() const;

    std::unique_ptr<Guild> guild() const;

    pplx::task<Message> respond(std::string content, bool tts = false, Embed embed = Embed()) const;

    pplx::task<bool> react(Emoji reaction) const;
    pplx::task<bool> react(std::string reaction) const;
  };
}

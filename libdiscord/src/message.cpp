#include "message.h"
#include "api.h"
#include "connection_state.h"
#include "discord_exception.h"
#include "guild.h"
#include "user.h"
#include "api/channel_api.h"


namespace discord
{
  message::message()
  {
    m_tts = false;
    m_mention_everyone = false;
    m_pinned = false;
  }

  message::message(connection_state* owner, rapidjson::Value& data) : identifiable(data["id"]), connection_object(owner)
  {
    set_from_json(m_channel_id, "channel_id", data);
    set_from_json(m_content, "content", data);
    set_from_json(m_timestamp, "timestamp", data);
    set_from_json(m_edited_timestamp, "edited_timestamp", data);
    set_from_json(m_tts, "tts", data);
    set_from_json(m_mention_everyone, "mention_everyone", data);
    set_from_json(m_pinned, "pinned", data);
    set_from_json(m_webhook_id, "webhook_id", data);

    auto found = data.FindMember("author");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_author = user(owner, data["author"]);
    }

    found = data.FindMember("mentions");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      for (auto& mention : found->value.GetArray())
      {
        m_mentions.push_back(user(owner, mention));
      }
    }
    
    //set_from_json(m_mention_roles, "mention_roles", data);
    //set_from_json(m_attachments, "attachments", data);
    //set_from_json(m_embeds, "embeds", data);
    //set_from_json(m_reactions, "reactions", data);


    //  Handle nonce specially because it can be sent as both an integer and string.
    found = data.FindMember("nonce");

    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      if (found->value.IsInt64())
      {
        m_nonce = found->value.GetInt64();
      }
      else
      {
        m_nonce = snowflake(found->value.GetString());
      }
    }
  }

  std::string message::content() const
  {
    return m_content;
  }

  const user& message::author() const
  {
    return m_author;
  }

  channel message::channel() const
  {
    return m_owner->find_channel(m_channel_id);
  }

  guild message::guild() const
  {
    return m_owner->find_guild_from_channel(m_channel_id);
  }

  std::future<message> message::respond(std::string content, bool tts, embed embed) const
  {
    if (content.size() > 2000)
    {
      throw discord_exception("Messages must be fewer than 2000 characters.");
    }

    return std::async(std::launch::async, api::channel::create_message, m_owner, m_channel_id, content, tts, embed);
  }
}

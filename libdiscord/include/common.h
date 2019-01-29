#pragma once

#include "easylogging++.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "identifiable.h"
#include "snowflake.h"

namespace discord
{
	enum EventType
	{
		Ready,
		// Messages
		MessageCreated,
		MessageDeleted,
		MessageEdited,
		MessagesBulkDeleted,
		// Guild
		GuildCreated,
		// Emoji
		EmojiCreated,
		EmojiDeleted,
		EmojiEdited,
		// Misc
		PresenceUpdate,
		Typing
	};

  /** A wrapper for cpprestsdk's method values. Using this avoids library users 
   *  having to include cpprestsdk's headers in their project.
   */
  enum class Method
  {
    GET,
    POST,
    PUT,
    PATCH,
    DEL
  };

  std::string json_to_string(rapidjson::Value& data);
  std::string json_to_string(const rapidjson::Value& data);

  /** A convenience method that avoids boilerplate when assigning primitive types from JSON values.
  *
  * @param value A value to assign to.
  * @param key The key to read the data from.
  * @param data The data to read from.
  */
  template <typename Type, typename Key>
  void set_from_json(Type& value, Key key, rapidjson::Value& data)
  {
    auto iter = data.FindMember(key);

    if (iter == data.MemberEnd() || iter->value.IsNull())
    {
      value = Type{};
    }
    else
    {
      value = iter->value.template Get<Type>();
    }
  }

  /** Specialized set_from_json for snowflakes. 
   *
   * @param value A snowflake value to assign to.
   * @param key The key to read the data from.
   * @param data The data to read from.
   */
  template <typename Key>
  void set_from_json(Snowflake& value, Key key, rapidjson::Value& data)
  {
    auto iter = data.FindMember(key);

    if (iter == data.MemberEnd() || iter->value.IsNull())
    {
      value = Snowflake{};
    }
    else
    {
      value = Snowflake(iter->value.GetString());
    }
  }
}
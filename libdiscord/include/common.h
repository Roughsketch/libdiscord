#pragma once

#include "easylogging++.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "identifiable.h"
#include "snowflake.h"

namespace discord
{
  std::string json_to_string(rapidjson::Value& data);
  std::string json_to_string(const rapidjson::Value& data);

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

  template <typename Key>
  void set_from_json(snowflake& value, Key key, rapidjson::Value& data)
  {
    auto iter = data.FindMember(key);

    if (iter == data.MemberEnd() || iter->value.IsNull())
    {
      value = snowflake{};
    }
    else
    {
      value = snowflake(iter->value.GetString());
    }
  }
}
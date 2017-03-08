#include "guild.h"
#include "bot.h"

namespace discord
{
  guild::guild(std::string token, rapidjson::Value& data) : m_token(token)
  {
  }
}

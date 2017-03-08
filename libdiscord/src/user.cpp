#include "user.h"
#include "bot.h"

namespace discord
{
  user::user(std::string token, rapidjson::Value& data) : m_token(token)
  {
  }
}

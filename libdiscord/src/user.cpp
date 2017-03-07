#include "user.h"
#include "bot.h"

namespace discord
{
  user::user(bot& owner, rapidjson::Value& data) : m_owner(owner)
  {
  }
}

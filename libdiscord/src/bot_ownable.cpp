#include "bot_ownable.h"

namespace discord
{
  bot_ownable::bot_ownable() : m_owner(nullptr)
  {
  }

  bot_ownable::bot_ownable(const bot* owner) : m_owner(owner)
  {
  }

  const bot* bot_ownable::owner() const
  {
    return m_owner;
  }
}

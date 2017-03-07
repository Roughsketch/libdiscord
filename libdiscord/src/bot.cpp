#include "bot.h"
#include "gateway.h"

namespace discord
{
  bot::bot(std::string token, std::string prefix) : m_token("Bot " + token), m_prefix(prefix)
  {
  }

  std::shared_ptr<bot> bot::create(std::string token, std::string prefix)
  {
    auto bot_ptr = std::make_shared<bot>(token, prefix);

    bot_ptr->m_gateway = std::make_unique<gateway>(bot_ptr);

    return bot_ptr;
  }

  void bot::run(bool async) const
  {
    m_gateway->start();

    if (!async)
    {
      for (;;)
      {
        std::this_thread::sleep_for(std::chrono::seconds(100000));
      }
    }
  }

  std::string bot::token() const
  {
    return m_token;
  }
}

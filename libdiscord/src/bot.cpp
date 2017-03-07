#include "bot.h"
#include "gateway.h"

namespace discord
{
  bot::bot(std::string token, std::string prefix)
  {
    m_data.token = token;
    m_data.prefix = prefix;
    m_gateway = std::make_unique<gateway>(*this, m_data);
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
    return m_data.token;
  }
}

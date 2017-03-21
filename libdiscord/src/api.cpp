#include "api.h"
#include "common.h"
#include "connection_state.h"
#include "discord_exception.h"

namespace discord
{
  namespace api 
  {
    std::string get_wss_url(connection_state& conn, int* shards)
    {
      auto response = conn.request(Gateway_Bot, 0, method::GET, "gateway/bot").get();

      if (response.status_code != 200)
      {
        throw discord_exception("Could not connect to gateway endpoint.");
      }

      auto found = response.data.FindMember("shards");

      //  If a shards value was sent, then populate it with the given shard amount.
      if (found != response.data.MemberEnd() && !found->value.IsNull() && shards)
      {
        *shards = found->value.GetInt();
      }

      return response.data["url"].GetString();
    }
  }
}
